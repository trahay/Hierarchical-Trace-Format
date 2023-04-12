#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "htf_read.h"
#include "htf_event.h"

static void init_callstack(struct htf_thread_reader *reader) {
  reader->current_frame = 0;
  reader->callstack_index[0] = 0;
  reader->callstack_loop_iteration[0] = 0;
  reader->callstack_sequence[0].type = HTF_TYPE_SEQUENCE;
  reader->callstack_sequence[0].id = 0;
}

void htf_read_thread_iterator_init(struct htf_archive *archive,
				   struct htf_thread_reader *reader,
				   htf_thread_id_t thread_id) {
  htf_assert(thread_id != HTF_THREAD_ID_INVALID);
  reader->archive = archive;
  reader->thread_trace = htf_archive_get_thread(archive, thread_id);
  htf_assert(reader->thread_trace != NULL);

  /* TODO: don't hardcode the max depth */
  reader->callstack_sequence = calloc(sizeof(htf_token_t), 100);
  reader->callstack_index = calloc(sizeof(int), 100);
  reader->callstack_loop_iteration = calloc(sizeof(int), 100); 
  reader->event_index = calloc(sizeof(int), reader->thread_trace->nb_events);

  if(htf_debug_level >= htf_dbg_lvl_verbose) {
    htf_log( htf_dbg_lvl_verbose, "init callstack for thread %s\n", htf_get_thread_name(reader->thread_trace));
    htf_log( htf_dbg_lvl_verbose, "The trace contains:\n");
    htf_print_sequence(reader->thread_trace, HTF_SEQUENCE_ID(0));
  }

  /* set the cursor on the first event */
  init_callstack(reader);  
}

static htf_token_t get_cur_token(struct htf_thread_reader *reader) {
  int cur_frame = reader->current_frame;
  int cur_index = reader->callstack_index[cur_frame];
  htf_token_t cur_seq_id = reader->callstack_sequence[cur_frame];
  return htf_get_token(reader->thread_trace, cur_seq_id, cur_index);
}

static void print_current_event(struct htf_thread_reader *reader) __attribute__((unused));
static void print_current_event(struct htf_thread_reader *reader) {
  htf_token_t t = get_cur_token(reader);
  htf_print_token(reader->thread_trace, t);
}

static void print_callstack(struct htf_thread_reader *reader)  __attribute__((unused));
static void print_callstack(struct htf_thread_reader *reader) {
  printf("# Callstack (depth: %d) ------------\n",   reader->current_frame+1);
  for(int i=0; i < reader->current_frame+1; i++) {
    printf("%.*s[%d]", i*2, "                       ", i);
    htf_token_t sequence = reader->callstack_sequence[i];
    int index = reader->callstack_index[i];
    htf_token_t t = htf_get_token(reader->thread_trace, sequence, index);
    htf_print_token(reader->thread_trace, t);
    printf("\n");
  }
  
}

/* enter a block (push a new frame in the callstack) */
static void enter_block(struct htf_thread_reader *reader, htf_token_t new_block) {
  htf_assert(HTF_TOKEN_TYPE(new_block) == HTF_TYPE_SEQUENCE ||
	     HTF_TOKEN_TYPE(new_block) == HTF_TYPE_LOOP);

  int cur_frame = reader->current_frame;

  if(htf_debug_level >= htf_dbg_lvl_debug) {
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

static void leave_block(struct htf_thread_reader *reader) {
  reader->current_frame--; // pop frame

  if(reader->current_frame < 0) {
    reader->current_frame = -1;
    htf_log(htf_dbg_lvl_debug, "End of trace !\n");
    return;
  }

  if(htf_debug_level >= htf_dbg_lvl_debug) {
    printf("[%d] Leave ", reader->current_frame);
    print_current_event(reader);
    printf("\n");
  }
}

/* return 1 if there are more events in the current sequence */
static int end_of_a_sequence(struct htf_thread_reader *reader,
			     int cur_index,
			     htf_token_t seq_id) {
  
  if(HTF_TOKEN_TYPE(seq_id) == HTF_TYPE_SEQUENCE) {
    htf_sequence_id_t sequence = HTF_TOKEN_TO_SEQUENCE_ID(seq_id);
    struct htf_sequence* s = htf_get_sequence(reader->thread_trace, sequence);
    return cur_index >= s->size;
  }

  return 1;
}

static void _get_next_event(struct htf_thread_reader *reader) {
  int cur_frame;
  int cur_index;
  htf_token_t cur_seq_id;

#define UPDATE_POSITION() do {						\
    cur_frame = reader->current_frame;					\
    cur_index = reader->callstack_index[cur_frame];			\
    cur_seq_id = reader->callstack_sequence[cur_frame];			\
    /* Did we reach the end of the trace ?  */				\
    if(reader->current_frame < 0) {					\
      htf_log(htf_dbg_lvl_debug, "End of trace %d!\n", __LINE__);	\
      reader->current_frame = -1;					\
      return;								\
    }									\
  } while(0)

  UPDATE_POSITION();
  htf_assert(HTF_TOKEN_TYPE(cur_seq_id) == HTF_TYPE_SEQUENCE ||
	     HTF_TOKEN_TYPE(cur_seq_id) == HTF_TYPE_LOOP);
  
  while( end_of_a_sequence(reader, cur_index+1, cur_seq_id)) {
    /* we reached the end of the current sequence */

    leave_block(reader);
    UPDATE_POSITION();

    /* Is the sequence in a loop ? */
    htf_token_t t = get_cur_token(reader);
    while(HTF_TOKEN_TYPE(t) == HTF_TYPE_LOOP) {
      /* if we are in the loop, increment the iteration count and
       * check if we reached the end of the loop
       */
      cur_frame = reader->current_frame;
      reader->callstack_loop_iteration[cur_frame]++; // increment the iteration count

      htf_loop_id_t l = HTF_TOKEN_TO_LOOP_ID(t);
      struct htf_loop* loop = htf_get_loop(reader->thread_trace, l);
      if( reader->callstack_loop_iteration[cur_frame] < loop->nb_iterations) {
	/* there are more iterations in the loop */
	goto enter_sequence;
      }

      /* end of the loop */
      leave_block(reader);
      UPDATE_POSITION();

      t = get_cur_token(reader);
    }
  }

  /* we are in the middle of a sequence */

  /* just move to the next event in the sequence */
  reader->callstack_index[cur_frame]++;
  UPDATE_POSITION();

 enter_sequence:
  {
    /* if it's a loop/sequence, find the first event */
    htf_token_t t = get_cur_token(reader);
    while(HTF_TOKEN_TYPE(t) == HTF_TYPE_SEQUENCE ||
	  HTF_TOKEN_TYPE(t) == HTF_TYPE_LOOP) {
      enter_block(reader, t);
      t = get_cur_token(reader);
    }
  }
}

static int _htf_read_thread_next_event(struct htf_thread_reader *reader,
				       struct htf_event_occurence *e,
				       int move) {
  if(reader->current_frame < 0)
    return -1;			/* TODO: return EOF */

  /* Get the current event */
  htf_token_t t = get_cur_token(reader);

  while(HTF_TOKEN_TYPE(t) == HTF_TYPE_SEQUENCE ||
	HTF_TOKEN_TYPE(t) == HTF_TYPE_LOOP) {
    enter_block(reader, t);
    t = get_cur_token(reader);
  }

  int event_index = HTF_TOKEN_ID(t);
  htf_event_id_t eid = HTF_EVENT_ID(event_index);
  struct htf_event_summary* es = &reader->thread_trace->events[event_index];
  memcpy(&e->event, &es->event, sizeof(e->event));
  e->timestamp = es->timestamps[reader->event_index[HTF_ID(eid)]];

  if(move) {
    /* Move to the next event */
    reader->event_index[event_index]++; // "consume" the event occurence
    _get_next_event(reader);
  }

  return 0;
}

int htf_read_thread_cur_event(struct htf_thread_reader *reader,
			      struct htf_event_occurence *e) {
  return _htf_read_thread_next_event(reader, e, 0);
}

int htf_read_thread_next_event(struct htf_thread_reader *reader,
			       struct htf_event_occurence *e) {

  return _htf_read_thread_next_event(reader, e, 1);
}
