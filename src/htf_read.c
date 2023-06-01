#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "htf_read.h"
#include "htf_archive.h"

static void init_callstack(struct htf_thread_reader *reader) {
	reader->referential_timestamp = 0;
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
	reader->sequence_index = calloc(reader->thread_trace->nb_sequences, sizeof(int));

	if (htf_debug_level >= htf_dbg_lvl_verbose) {
		htf_log(htf_dbg_lvl_verbose, "init callstack for thread %d\n", thread_id);
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
	if (new_block.type == HTF_TYPE_SEQUENCE) {
		struct htf_sequence* cur_seq = htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(new_block));
		reader->referential_timestamp =
				*(htf_timestamp_t*)array_get(&cur_seq->timestamps, reader->sequence_index[new_block.id] - 1);

		htf_log(htf_dbg_lvl_debug, "Setting up new referential timestamp: %.9lf\n", reader->referential_timestamp / 1e9);
	}
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

static void _get_next_token(struct htf_thread_reader* reader) {
	int cur_frame;
	int cur_index;
	htf_token_t cur_seq_id;
	int cur_loop_iteration;

	if (reader->current_frame < 0)
		return;

	// Update Position
	cur_frame = reader->current_frame;
	cur_index = reader->callstack_index[cur_frame];
	cur_seq_id = reader->callstack_sequence[cur_frame];
	cur_loop_iteration = reader->callstack_loop_iteration[cur_frame];
	/* Did we reach the end of the trace ?  */
	if (reader->current_frame < 0) {
		htf_log(htf_dbg_lvl_debug, "End of trace %d!\n", __LINE__);
		reader->current_frame = -1;
		return;
	}

	htf_assert(HTF_TOKEN_TYPE(cur_seq_id) == HTF_TYPE_SEQUENCE || HTF_TOKEN_TYPE(cur_seq_id) == HTF_TYPE_LOOP);
	/* get to the next position */
	if (HTF_TOKEN_TYPE(cur_seq_id) == HTF_TYPE_SEQUENCE) {
		if (end_of_a_sequence(reader, cur_index + 1, cur_seq_id)) {
			/* we reached the end of a sequence. leave the block and get the next event */
			leave_block(reader);
			_get_next_token(reader);
			return;
		}

		/* just move to the next event in the sequence */
		reader->callstack_index[cur_frame]++;
	} else {
		if (end_of_a_loop(reader, cur_loop_iteration + 1, cur_seq_id)) {
			/* we reached the end of a sequence. leave the block and get the next event */
			leave_block(reader);
			_get_next_token(reader);
			return;
		}
		/* just move to the next iteration in the loop */
		reader->callstack_loop_iteration[cur_frame]++;
	}
}

int htf_move_to_next_token(struct htf_thread_reader* reader) {
	htf_token_t t = get_cur_token(reader);
	if (HTF_TOKEN_TYPE(t) == HTF_TYPE_SEQUENCE || HTF_TOKEN_TYPE(t) == HTF_TYPE_LOOP) {
		if (t.type == HTF_TYPE_SEQUENCE)
			reader->sequence_index[t.id]++;
		enter_block(reader, t);
	} else {
		reader->event_index[HTF_TOKEN_ID(t)]++;	 // "consume" the event occurence
		_get_next_token(reader);
	}
}
int htf_read_thread_cur_token(struct htf_thread_reader* reader,
															struct htf_token* token,
															struct htf_event_occurence* e) {
	if (reader->current_frame < 0) {
		return -1; /* TODO: return EOF */
	}

	/* Get the current event */
	htf_token_t t = get_cur_token(reader);
	memcpy(token, &t, sizeof(t));

	if (HTF_TOKEN_TYPE(t) == HTF_TYPE_EVENT) {
		int event_index = HTF_TOKEN_ID(t);
		struct htf_event_summary* es = &reader->thread_trace->events[event_index];
		memcpy(&e->event, &es->event, sizeof(e->event));
		e->timestamp = reader->referential_timestamp;
		e->duration = es->timestamps[reader->event_index[event_index]];
		reader->referential_timestamp += es->timestamps[reader->event_index[event_index]];
	}
	return 0;
}

htf_timestamp_t htf_get_starting_timestamp(struct htf_thread_reader* reader, struct htf_token token) {
	switch (token.type) {
		case HTF_TYPE_EVENT: {
			int event_index = reader->event_index[HTF_TOKEN_ID(token)];
			return reader->referential_timestamp + reader->thread_trace->events[token.id].timestamps[event_index];
		}
		case HTF_TYPE_SEQUENCE: {
			int sequence_index = reader->sequence_index[token.id];
			struct htf_sequence* seq = htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(token));
			return *(htf_timestamp_t*)array_get(&seq->timestamps, sequence_index);
		}
		case HTF_TYPE_LOOP: {
			struct htf_loop* loop = htf_get_loop(reader->thread_trace, HTF_TOKEN_TO_LOOP_ID(token));
			struct htf_sequence* seq = htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(loop->token));
			int sequence_index = reader->sequence_index[loop->token.id];
			return *(htf_timestamp_t*)array_get(&seq->timestamps, sequence_index);
		}
		case HTF_TYPE_INVALID:
			htf_error("Invalid token type\n");
	}
}

htf_timestamp_t htf_get_duration(struct htf_thread_reader* reader, struct htf_token token) {
	switch (token.type) {
		case HTF_TYPE_EVENT: {
			int event_index = reader->event_index[HTF_TOKEN_ID(token)];
			return reader->thread_trace->events[token.id].timestamps[event_index];
		}
		case HTF_TYPE_SEQUENCE: {
			int sequence_index = reader->sequence_index[token.id];
			struct htf_sequence* seq = htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(token));
			return seq->durations[sequence_index];
		}
		case HTF_TYPE_LOOP: {
			htf_timestamp_t sum = 0;
			struct htf_loop* loop = htf_get_loop(reader->thread_trace, HTF_TOKEN_TO_LOOP_ID(token));
			struct htf_sequence* seq = htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(loop->token));
			int sequence_index = reader->sequence_index[loop->token.id];
			DOFOR(i, loop->nb_iterations) {
				sum += seq->durations[sequence_index + i];
			}
			return sum;
		}
		case HTF_TYPE_INVALID:
			htf_error("Invalid token type\n");
	}
}

/** Increments the counters in the reader */
void __skip_token(struct htf_thread_reader* reader, htf_token_t token, int nb_times) {
	switch (token.type) {
		case HTF_TYPE_EVENT: {
			reader->event_index[token.id] += nb_times;
			break;
		}
		case HTF_TYPE_SEQUENCE: {
			struct htf_sequence* seq = htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(token));
			reader->sequence_index[token.id] += nb_times;
			DOFOR(i, seq->size) {
				__skip_token(reader, seq->token[i], nb_times);
			}
			break;
		}
		case HTF_TYPE_LOOP: {
			struct htf_loop* loop = htf_get_loop(reader->thread_trace, HTF_TOKEN_TO_LOOP_ID(token));
			struct htf_sequence* seq = htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(loop->token));
			reader->sequence_index[loop->token.id] += nb_times * loop->nb_iterations;
			DOFOR(i, seq->size) {
				__skip_token(reader, seq->token[i], nb_times * loop->nb_iterations);
			}
			break;
		}
		default:
			htf_error("This shouldn't have happened\n");
	}
}

void skip_token(struct htf_thread_reader* reader, htf_token_t token) {
	switch (token.type) {
		case HTF_TYPE_EVENT: {
			int event_index = reader->event_index[HTF_TOKEN_ID(token)] - 1;
			reader->referential_timestamp += reader->thread_trace->events[token.id].timestamps[event_index];
			break;
		}
		case HTF_TYPE_SEQUENCE: {
			int event_index = reader->sequence_index[HTF_TOKEN_ID(token)] - 1;
			struct htf_sequence* seq = htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(token));
			reader->referential_timestamp += seq->durations[event_index];
			__skip_token(reader, token, 1);
			break;
		}
		case HTF_TYPE_LOOP: {
			// TODO Fuck it we ball
			break;
		}
		default:
			htf_error("This shouldn't have happened\n");
	}
}