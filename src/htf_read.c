#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "htf_read.h"

static void init_callstack(struct thread_reader *reader) {
  reader->current_frame = 0;
  reader->callstack_index[0] = 0;
  reader->callstack_loop_iteration[0] = 0;
  reader->callstack_sequence[0].type = TYPE_SEQUENCE;
  reader->callstack_sequence[0].id = 0;
}

void htf_read_thread_iterator_init(struct thread_reader *reader,
				   struct trace* trace,
				   int thread_index) {
  reader->trace = trace;
  htf_assert(thread_index < trace->nb_threads);
  reader->thread_trace = trace->threads[thread_index];

  reader->event_index = calloc(sizeof(int), trace->threads[thread_index]->nb_events);

  /* TODO: don't hardcode the max depth */
  reader->callstack_sequence = calloc(sizeof(token_t), 100);
  reader->callstack_index = calloc(sizeof(int), 100);
  reader->callstack_loop_iteration = calloc(sizeof(int), 100);

  if(htf_debug_level >= dbg_lvl_verbose) {
    htf_log( dbg_lvl_verbose, "init callstack for thread %d\n", thread_index);
    htf_log( dbg_lvl_verbose, "The trace contains:\n");
    htf_print_sequence(reader->thread_trace, SEQUENCE_ID(0));
  }

  /* set the cursor on the first event */
  init_callstack(reader);  
}


static token_t get_cur_token(struct thread_reader *reader) {
  int cur_frame = reader->current_frame;
  int cur_index = reader->callstack_index[cur_frame];
  token_t cur_seq_id = reader->callstack_sequence[cur_frame];
  return htf_get_token(reader->thread_trace, cur_seq_id, cur_index);
}

static void print_current_event(struct thread_reader *reader) __attribute__((unused));
static void print_current_event(struct thread_reader *reader) {
  token_t t = get_cur_token(reader);
  htf_print_token(reader->thread_trace, t);
}

static void print_callstack(struct thread_reader *reader)  __attribute__((unused));
static void print_callstack(struct thread_reader *reader) {
  printf("# Callstack (depth: %d) ------------\n",   reader->current_frame+1);
  for(int i=0; i < reader->current_frame+1; i++) {
    printf("%.*s[%d]", i*2, "                       ", i);
    token_t sequence = reader->callstack_sequence[i];
    int index = reader->callstack_index[i];
    token_t t = htf_get_token(reader->thread_trace, sequence, index);
    htf_print_token(reader->thread_trace, t);
    printf("\n");
  }
  
}

/* enter a block (push a new frame in the callstack) */
static void enter_block(struct thread_reader *reader, token_t new_block) {
  htf_assert(TOKEN_TYPE(new_block) == TYPE_SEQUENCE ||
	     TOKEN_TYPE(new_block) == TYPE_LOOP);

  int cur_frame = reader->current_frame;

  if(htf_debug_level >= dbg_lvl_debug) {
    printf("[%d] Enter ", reader->current_frame);
    print_current_event(reader);
    printf("\n");
  }

  reader->current_frame++;  // push frame

  cur_frame = reader->current_frame;
  reader->callstack_index[cur_frame] = 0;
  reader->callstack_loop_iteration[cur_frame] = 0;
  reader->callstack_sequence[cur_frame] = new_block;

}

static void leave_block(struct thread_reader *reader) {
  reader->current_frame--; // pop frame

  if(reader->current_frame < 0) {
    reader->current_frame = -1;
    htf_log(dbg_lvl_debug, "End of trace !\n");
    return;
  }

  if(htf_debug_level >= dbg_lvl_debug) {
    printf("[%d] Leave ", reader->current_frame);
    print_current_event(reader);
    printf("\n");
  }
}

/* return 1 if there are more events in the current sequence */
static int end_of_a_sequence(struct thread_reader *reader,
			     int cur_index,
			     token_t seq_id) {
  
  if(TOKEN_TYPE(seq_id) == TYPE_SEQUENCE) {
    sequence_id_t sequence = TOKEN_TO_SEQUENCE_ID(seq_id);
    struct sequence* s = htf_get_sequence(reader->thread_trace, sequence);
    return cur_index >= s->size;
  }

  return 1;
}

static void _get_next_event(struct thread_reader *reader) {
  int cur_frame = reader->current_frame;
  int cur_index = reader->callstack_index[cur_frame];
  token_t cur_seq_id = reader->callstack_sequence[cur_frame];
  htf_assert(TOKEN_TYPE(cur_seq_id) == TYPE_SEQUENCE ||
	     TOKEN_TYPE(cur_seq_id) == TYPE_LOOP);
  
  while( end_of_a_sequence(reader, cur_index+1, cur_seq_id)) {
    /* we reached the end of the current sequence */

    leave_block(reader);
    cur_frame = reader->current_frame;
    cur_index = reader->callstack_index[cur_frame];
    cur_seq_id = reader->callstack_sequence[cur_frame];

    /* Did we reach the end of the trace ?  */
    if(reader->current_frame < 0) {
      htf_log(dbg_lvl_debug, "End of trace %d!\n", __LINE__);
      reader->current_frame = -1;
      return;
    }

    /* Is the sequence in a loop ? */
    token_t t = get_cur_token(reader);
    while(TOKEN_TYPE(t) == TYPE_LOOP) {
      /* if we are in the loop, increment the iteration count and
       * check if we reached the end of the loop
       */
      cur_frame = reader->current_frame;
      reader->callstack_loop_iteration[cur_frame]++; // increment the iteration count

      loop_id_t l = TOKEN_TO_LOOP_ID(t);
      struct loop* loop = htf_get_loop(reader->thread_trace, l);
      if( reader->callstack_loop_iteration[cur_frame] < loop->nb_iterations) {
	/* there are more iterations in the loop */
	goto enter_sequence;
      }

      /* end of the loop */
      leave_block(reader);
      if(reader->current_frame < 0) {
	reader->current_frame = -1;
	return;
      }
      t = get_cur_token(reader);
    }
  }

  /* we are in the middle of a sequence */

  /* just move to the next event in the sequence */
  cur_frame = reader->current_frame;
  cur_index = reader->callstack_index[cur_frame];
  cur_seq_id = reader->callstack_sequence[cur_frame];

  reader->callstack_index[cur_frame]++;

 enter_sequence:
  /* if it's a loop/sequence, find the first event */
  token_t t = get_cur_token(reader);
  while(TOKEN_TYPE(t) == TYPE_SEQUENCE ||
	TOKEN_TYPE(t) == TYPE_LOOP) {
    enter_block(reader, t);
    t = get_cur_token(reader);
  }    
}

static int _htf_read_thread_next_event(struct thread_reader *reader,
				       struct event_occurence *e,
				       int move) {
  if(reader->current_frame < 0)
    return -1;			/* TODO: return EOF */

  /* Get the current event */
  token_t t = get_cur_token(reader);

  while(TOKEN_TYPE(t) == TYPE_SEQUENCE ||
	TOKEN_TYPE(t) == TYPE_LOOP) {
    enter_block(reader, t);
    t = get_cur_token(reader);
  }

  int event_index = TOKEN_ID(t);
  event_id_t eid = EVENT_ID(event_index);
  struct event_summary* es = &reader->thread_trace->events[event_index];
  memcpy(&e->event, &es->event, sizeof(e->event));
  e->timestamp = es->timestamps[reader->event_index[ID(eid)]];

  if(move) {
    /* Move to the next event */
    reader->event_index[event_index]++; // "consume" the event occurence
    _get_next_event(reader);
  }

  return 0;
}

int htf_read_thread_cur_event(struct thread_reader *reader,
			      struct event_occurence *e) {
  return _htf_read_thread_next_event(reader, e, 0);
}

int htf_read_thread_next_event(struct thread_reader *reader,
			       struct event_occurence *e) {

  return _htf_read_thread_next_event(reader, e, 1);
}
