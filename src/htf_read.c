#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "htf_read.h"
#include "htf_archive.h"

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

	reader->callstack_sequence = calloc(MAX_CALLSTACK_DEPTH, sizeof(htf_token_t));
	reader->callstack_index = calloc(MAX_CALLSTACK_DEPTH, sizeof(int));
	reader->callstack_loop_iteration = calloc(MAX_CALLSTACK_DEPTH, sizeof(int));
	reader->event_index = calloc(reader->thread_trace->nb_events, sizeof(int));

	if (htf_debug_level >= htf_dbg_lvl_verbose) {
		htf_log(htf_dbg_lvl_verbose, "init callstack for thread %s\n", htf_get_thread_name(reader->thread_trace));
		htf_log(htf_dbg_lvl_verbose, "The trace contains:\n");
		htf_print_sequence(reader->thread_trace, HTF_SEQUENCE_ID(0));
	}

	/* set the cursor on the first event */
	init_callstack(reader);
}

/* return the sequence being run at frame frame_no */
htf_token_t _htf_get_frame_in_callstack(struct htf_thread_reader *reader,
					int frame_no) {
  if(frame_no < 0)
    return HTF_TOKENIZE(HTF_TYPE_INVALID, HTF_TOKEN_ID_INVALID);
  return reader->callstack_sequence[frame_no];
}

/* return the token being run at frame frame_no */
htf_token_t _htf_get_token_in_callstack(struct htf_thread_reader* reader, int frame_no) {
	if (frame_no < 0)
		return HTF_TOKENIZE(HTF_TYPE_INVALID, HTF_TOKEN_ID_INVALID);

	htf_token_t seq = _htf_get_frame_in_callstack(reader, frame_no);

	htf_assert((HTF_TOKEN_TYPE(seq) == HTF_TYPE_LOOP) || (HTF_TOKEN_TYPE(seq) == HTF_TYPE_SEQUENCE));

	return htf_get_token(reader->thread_trace, seq, reader->callstack_index[frame_no]);
}

static htf_token_t get_cur_token(struct htf_thread_reader *reader) {
  int cur_frame = reader->current_frame;
  return _htf_get_token_in_callstack(reader, cur_frame);
}

static htf_token_t get_cur_sequence(struct htf_thread_reader *reader) {
  int cur_frame = reader->current_frame;
  return _htf_get_frame_in_callstack(reader, cur_frame);
}

static void print_current_event(struct htf_thread_reader *reader) {
  htf_token_t t = get_cur_token(reader);
  htf_print_token(reader->thread_trace, t);
}

static void print_current_sequence(struct htf_thread_reader *reader) {
  htf_token_t t = get_cur_sequence(reader);
  htf_print_token(reader->thread_trace, t);
}


static void print_callstack(struct htf_thread_reader *reader)  __attribute__((unused));
static void print_callstack(struct htf_thread_reader *reader) {
  printf("# Callstack (depth: %d) ------------\n",   reader->current_frame+1);
  for(int i=0; i < reader->current_frame+1; i++) {

    htf_token_t cur_seq_id = _htf_get_frame_in_callstack(reader, i);
    htf_token_t token = _htf_get_token_in_callstack(reader, i);
    
    printf("%.*s[%d] ", i*2, "                       ", i);

    htf_print_token(reader->thread_trace, cur_seq_id);

    if(HTF_TOKEN_TYPE(cur_seq_id) == HTF_TYPE_LOOP) {
      struct htf_loop *loop =  htf_get_loop(reader->thread_trace, HTF_TOKEN_TO_LOOP_ID(cur_seq_id));
      printf(" iter %d/%d", reader->callstack_loop_iteration[i], loop->nb_iterations);
      htf_assert(reader->callstack_loop_iteration[i]<MAX_CALLSTACK_DEPTH);
    } else if(HTF_TOKEN_TYPE(cur_seq_id) == HTF_TYPE_SEQUENCE) {
      struct htf_sequence *seq =  htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(cur_seq_id));
      printf(" pos %d/%d", reader->callstack_index[i], seq->size);
      htf_assert(reader->callstack_index[i]<MAX_CALLSTACK_DEPTH);
    }

    printf("\t-> ");
    htf_print_token(reader->thread_trace, token);

    printf("\n");
  } 
}

/* enter a block (push a new frame in the callstack) */
static void enter_block(struct htf_thread_reader *reader, htf_token_t new_block) {
	htf_assert(HTF_TOKEN_TYPE(new_block) == HTF_TYPE_SEQUENCE || HTF_TOKEN_TYPE(new_block) == HTF_TYPE_LOOP);
	if (htf_debug_level >= htf_dbg_lvl_debug) {
		htf_log(htf_dbg_lvl_debug, "[%d] Enter ", reader->current_frame);
		print_current_event(reader);
		printf("\n");
	}

	int cur_frame = ++reader->current_frame;	// Push frame
	reader->callstack_index[cur_frame] = 0;
	reader->callstack_loop_iteration[cur_frame] = 0;
	reader->callstack_sequence[cur_frame] = new_block;
}

/* return 1 if there are more events in the current sequence */
static int end_of_a_sequence(struct htf_thread_reader *reader,
			     int cur_index,
			     htf_token_t seq_id) {
  
  if(HTF_TOKEN_TYPE(seq_id) == HTF_TYPE_SEQUENCE) {
    htf_sequence_id_t sequence = HTF_TOKEN_TO_SEQUENCE_ID(seq_id);
    struct htf_sequence* s = htf_get_sequence(reader->thread_trace, sequence);
    /* we are in a sequence and index is beyond the end of the sequence */
    return cur_index >= s->size;
  }
  return 0;
}

/* return 1 if there are more events in the current sequence */
static int end_of_a_loop(struct htf_thread_reader *reader,
			 int cur_index,
			 htf_token_t seq_id) {
  
  if(HTF_TOKEN_TYPE(seq_id) == HTF_TYPE_LOOP) {
    htf_loop_id_t loop = HTF_TOKEN_TO_LOOP_ID(seq_id);
    struct htf_loop* l = htf_get_loop(reader->thread_trace, loop);
    /* we are in a loop and index is beyond the number of iterations */
    return cur_index >= l->nb_iterations;
  }

  return 0;
}

static void leave_block(struct htf_thread_reader *reader) {
  if(htf_debug_level >= htf_dbg_lvl_debug) {
    htf_log( htf_dbg_lvl_debug, "[%d] Leave ", reader->current_frame);
    print_current_sequence(reader);
    printf("\n");
  }

  int cur_frame = reader->current_frame;
  if(htf_debug_level >= htf_dbg_lvl_debug) {
    reader->callstack_index[cur_frame] = INT_MAX;
    reader->callstack_sequence[cur_frame] = HTF_TOKENIZE(HTF_TYPE_INVALID, HTF_TOKEN_ID_INVALID);
    reader->callstack_loop_iteration[cur_frame] = INT_MAX;
  }

  reader->current_frame--; // pop frame

  if(reader->current_frame < 0) {
    reader->current_frame = -1;
    htf_log(htf_dbg_lvl_debug, "End of trace !\n");
    return;
  }

  cur_frame = reader->current_frame;

  htf_token_t cur_seq = _htf_get_frame_in_callstack(reader, cur_frame);
  htf_assert((HTF_TOKEN_TYPE(cur_seq) == HTF_TYPE_LOOP) ||
	     (HTF_TOKEN_TYPE(cur_seq) == HTF_TYPE_SEQUENCE));
}

static void _get_next_event(struct htf_thread_reader *reader) {
  int cur_frame;
  int cur_index;
  htf_token_t cur_seq_id;
  int cur_loop_iteration;

  if(reader->current_frame < 0)
    return;

#define UPDATE_POSITION() do {						\
    cur_frame = reader->current_frame;					\
    cur_index = reader->callstack_index[cur_frame];			\
    cur_seq_id = reader->callstack_sequence[cur_frame];			\
    cur_loop_iteration = reader->callstack_loop_iteration[cur_frame];	\
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
  /* get to the next position */
  if(HTF_TOKEN_TYPE(cur_seq_id) == HTF_TYPE_SEQUENCE) {
    if( end_of_a_sequence(reader, cur_index+1, cur_seq_id)) {
      /* we reached the end of a sequence. leave the block and get the next event */
      leave_block(reader);
      _get_next_event(reader);
      return;
    }

    /* just move to the next event in the sequence */
    reader->callstack_index[cur_frame]++;
  } else {
    if( end_of_a_loop(reader, cur_loop_iteration+1, cur_seq_id)) {
      /* we reached the end of a sequence. leave the block and get the next event */
      leave_block(reader);
      _get_next_event(reader);
      return;
    }
    /* just move to the next iteration in the sequence */   
    reader->callstack_loop_iteration[cur_frame]++;
  }

  /* We are at the next position.
   * This may be the start of a sequence/loop, so we need to enter until we reach an event
   */
  htf_token_t t = get_cur_token(reader);
  while((HTF_TOKEN_TYPE(t) == HTF_TYPE_SEQUENCE) ||
	(HTF_TOKEN_TYPE(t) == HTF_TYPE_LOOP) ) {
    enter_block(reader, t);
    t = get_cur_token(reader);
  }
}

static int _htf_read_thread_next_event(struct htf_thread_reader *reader,
				       struct htf_event_occurence *e,
				       int move) {
  if(reader->current_frame < 0) {
    return -1;			/* TODO: return EOF */
  }

  /* Get the current event */
  htf_token_t t = get_cur_token(reader);

	while (HTF_TOKEN_TYPE(t) == HTF_TYPE_SEQUENCE || HTF_TOKEN_TYPE(t) == HTF_TYPE_LOOP) {
		enter_block(reader, t);
		t = get_cur_token(reader);
	}

  int event_index = HTF_TOKEN_ID(t);
  struct htf_event_summary* es = &reader->thread_trace->events[event_index];
  memcpy(&e->event, &es->event, sizeof(e->event));
  e->timestamp = es->timestamps[reader->event_index[event_index]];

  if (move) {
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
