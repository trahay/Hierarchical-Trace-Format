/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "htf_archive.h"
#include "htf_read.h"

static void init_callstack(struct htf_thread_reader* reader) {
  reader->referential_timestamp = 0;
  reader->current_frame = 0;
  reader->callstack_index[0] = 0;
  reader->callstack_loop_iteration[0] = 0;
  reader->callstack_sequence[0].type = HTF_TYPE_SEQUENCE;
  reader->callstack_sequence[0].id = 0;
}

void htf_read_thread_iterator_init(struct htf_archive* archive,
                                   struct htf_thread_reader* reader,
                                   htf_thread_id_t thread_id,
                                   enum thread_reader_option options) {
  htf_assert(thread_id != HTF_THREAD_ID_INVALID);
  reader->archive = archive;
  reader->options = options;
  reader->thread_trace = htf_archive_get_thread(archive, thread_id);
  htf_assert(reader->thread_trace != NULL);

  reader->callstack_sequence = calloc(MAX_CALLSTACK_DEPTH, sizeof(htf_token_t));
  reader->callstack_index = calloc(MAX_CALLSTACK_DEPTH, sizeof(int));
  reader->callstack_loop_iteration = calloc(MAX_CALLSTACK_DEPTH, sizeof(int));
  reader->event_index = calloc(reader->thread_trace->nb_events, sizeof(int));
  reader->sequence_index = calloc(reader->thread_trace->nb_sequences, sizeof(int));
  reader->loop_index = calloc(reader->thread_trace->nb_sequences, sizeof(int));
  if (htf_debug_level >= htf_dbg_lvl_verbose) {
    htf_log(htf_dbg_lvl_verbose, "init callstack for thread %d\n", thread_id);
    htf_log(htf_dbg_lvl_verbose, "The trace contains:\n");
    htf_print_sequence(reader->thread_trace, HTF_SEQUENCE_ID(0));
  }

  /* set the cursor on the first event */
  init_callstack(reader);
}

/* return the sequence being run at frame frame_no */
htf_token_t _htf_get_frame_in_callstack(struct htf_thread_reader* reader, int frame_no) {
  if (frame_no < 0)
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

static htf_token_t get_cur_token(struct htf_thread_reader* reader) {
  int cur_frame = reader->current_frame;
  return _htf_get_token_in_callstack(reader, cur_frame);
}

static htf_token_t get_cur_sequence(struct htf_thread_reader* reader) {
  int cur_frame = reader->current_frame;
  return _htf_get_frame_in_callstack(reader, cur_frame);
}

static void print_current_event(struct htf_thread_reader* reader) {
  htf_token_t t = get_cur_token(reader);
  htf_print_token(reader->thread_trace, t);
}

static void print_current_sequence(struct htf_thread_reader* reader) {
  htf_token_t t = get_cur_sequence(reader);
  htf_print_token(reader->thread_trace, t);
}

static void print_callstack(struct htf_thread_reader* reader) __attribute__((unused));
static void print_callstack(struct htf_thread_reader* reader) {
  printf("# Callstack (depth: %d) ------------\n", reader->current_frame + 1);
  for (int i = 0; i < reader->current_frame + 1; i++) {
    htf_token_t cur_seq_id = _htf_get_frame_in_callstack(reader, i);
    htf_token_t token = _htf_get_token_in_callstack(reader, i);

    printf("%.*s[%d] ", i * 2, "                       ", i);

    htf_print_token(reader->thread_trace, cur_seq_id);

    if (HTF_TOKEN_TYPE(cur_seq_id) == HTF_TYPE_LOOP) {
      struct htf_loop* loop = htf_get_loop(reader->thread_trace, HTF_TOKEN_TO_LOOP_ID(cur_seq_id));
      printf(" iter %d/%d", reader->callstack_loop_iteration[i],
             *(int*)htf_vector_get(&loop->nb_iterations, reader->loop_index[cur_seq_id.id]));
      htf_assert(reader->callstack_loop_iteration[i] < MAX_CALLSTACK_DEPTH);
    } else if (HTF_TOKEN_TYPE(cur_seq_id) == HTF_TYPE_SEQUENCE) {
      struct htf_sequence* seq = htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(cur_seq_id));
      printf(" pos %d/%d", reader->callstack_index[i], seq->size);
      htf_assert(reader->callstack_index[i] < MAX_CALLSTACK_DEPTH);
    }

    printf("\t-> ");
    htf_print_token(reader->thread_trace, token);

    printf("\n");
  }
}

/* enter a block (push a new frame in the callstack) */
void enter_block(struct htf_thread_reader* reader, htf_token_t new_block) {
  htf_assert(HTF_TOKEN_TYPE(new_block) == HTF_TYPE_SEQUENCE || HTF_TOKEN_TYPE(new_block) == HTF_TYPE_LOOP);
  if (htf_debug_level >= htf_dbg_lvl_debug) {
    htf_log(htf_dbg_lvl_debug, "[%d] Enter ", reader->current_frame);
    print_current_event(reader);
    printf("\n");
  }

  int cur_frame = ++reader->current_frame;  // Push frame
  reader->callstack_index[cur_frame] = 0;
  reader->callstack_loop_iteration[cur_frame] = 0;
  reader->callstack_sequence[cur_frame] = new_block;
  if (new_block.type == HTF_TYPE_SEQUENCE) {
    struct htf_sequence* cur_seq = htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(new_block));
    if ((reader->options & OPTION_NO_TIMESTAMPS) == 0) {
      // We shouldn't have to do anything, actually.
    }
    htf_log(htf_dbg_lvl_debug, "Setting up new referential timestamp: %.9lf\n", reader->referential_timestamp / 1e9);
  }
}

/* return 1 if there are more events in the current sequence */
static int end_of_a_sequence(struct htf_thread_reader* reader, int cur_index, htf_token_t seq_id) {
  if (HTF_TOKEN_TYPE(seq_id) == HTF_TYPE_SEQUENCE) {
    htf_sequence_id_t sequence = HTF_TOKEN_TO_SEQUENCE_ID(seq_id);
    struct htf_sequence* s = htf_get_sequence(reader->thread_trace, sequence);
    /* we are in a sequence and index is beyond the end of the sequence */
    return cur_index >= s->size;
  }
  return 0;
}

/* return 1 if there are more events in the current sequence */
static int end_of_a_loop(struct htf_thread_reader* reader, int cur_index, htf_token_t seq_id) {
  if (HTF_TOKEN_TYPE(seq_id) == HTF_TYPE_LOOP) {
    htf_loop_id_t loop = HTF_TOKEN_TO_LOOP_ID(seq_id);
    struct htf_loop* l = htf_get_loop(reader->thread_trace, loop);
    /* we are in a loop and index is beyond the number of iterations */
    return cur_index >= *(int*)htf_vector_get(&l->nb_iterations, reader->loop_index[seq_id.id] - 1);
  }

  return 0;
}

void leave_block(struct htf_thread_reader* reader) {
  if (htf_debug_level >= htf_dbg_lvl_debug) {
    htf_log(htf_dbg_lvl_debug, "[%d] Leave ", reader->current_frame);
    print_current_sequence(reader);
    printf("\n");
  }

  int cur_frame = reader->current_frame;
  if (htf_debug_level >= htf_dbg_lvl_debug) {
    reader->callstack_index[cur_frame] = INT_MAX;
    reader->callstack_sequence[cur_frame] = HTF_TOKENIZE(HTF_TYPE_INVALID, HTF_TOKEN_ID_INVALID);
    reader->callstack_loop_iteration[cur_frame] = INT_MAX;
  }

  reader->current_frame--;  // pop frame

  if (reader->current_frame < 0) {
    reader->current_frame = -1;
    htf_log(htf_dbg_lvl_debug, "End of trace !\n");
    return;
  }

  cur_frame = reader->current_frame;

  htf_token_t cur_seq = _htf_get_frame_in_callstack(reader, cur_frame);
  htf_assert((HTF_TOKEN_TYPE(cur_seq) == HTF_TYPE_LOOP) || (HTF_TOKEN_TYPE(cur_seq) == HTF_TYPE_SEQUENCE));
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
    if (_reader_show_structure(reader) || end_of_a_loop(reader, cur_loop_iteration + 1, cur_seq_id)) {
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
    if (t.type == HTF_TYPE_LOOP)
      reader->loop_index[t.id]++;

    enter_block(reader, t);
  } else {
    // Don't forget to update the timestamp
    struct htf_event_summary* es = &reader->thread_trace->events[t.id];
    if ((reader->options & OPTION_NO_TIMESTAMPS) == 0)
      reader->referential_timestamp += es->durations[reader->event_index[t.id]];
    reader->event_index[t.id]++;  // "consume" the event occurence
    _get_next_token(reader);
  }
  return 0;
}

void _htf_write_sequence_occurence(struct htf_thread_reader* reader,
                                   struct htf_sequence_occurence* occurence,
                                   htf_token_t token) {
  struct htf_sequence* sequence = reader->thread_trace->sequences[token.id];

  // Write it to the occurence
  if ((reader->options & OPTION_NO_TIMESTAMPS) == 0) {
    occurence->timestamp = reader->referential_timestamp;
  }
  occurence->sequence = sequence;
  occurence->full_sequence = NULL;
  occurence->savestate = create_savestate(reader);

  // Update the reader
  if ((reader->options & OPTION_NO_TIMESTAMPS) == 0) {
    occurence->duration = *(htf_timestamp_t*)htf_vector_get(&sequence->durations, reader->sequence_index[token.id]);
    reader->referential_timestamp += occurence->duration;
  }
  skip_sequence(reader, token);
}

/* Get the duration of the given loop */
htf_timestamp_t _htf_get_loop_duration(struct htf_thread_reader* reader, struct htf_token token) {
  htf_assert(token.type == HTF_TYPE_LOOP);
  htf_timestamp_t sum = 0;
  struct htf_loop* loop = htf_get_loop(reader->thread_trace, HTF_TOKEN_TO_LOOP_ID(token));
  struct htf_sequence* seq = htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(loop->token));
  int sequence_index = reader->sequence_index[loop->token.id];
  DOFOR(i, *(int*)htf_vector_get(&loop->nb_iterations, reader->loop_index[token.id])) {
    sum += *(htf_timestamp_t*)htf_vector_get(&seq->durations, sequence_index + i);
  }
  return sum;
}

int htf_read_thread_cur_level(struct htf_thread_reader* reader,
                              htf_occurence** occurence_array,
                              struct htf_token** token_array,
                              int* length) {
  htf_token_t current_sequence_id = get_cur_sequence(reader);
  struct htf_sequence* current_sequence =
    htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(current_sequence_id));

  htf_assert(current_sequence->size > 0);
  memcpy(length, &current_sequence->size, sizeof(current_sequence->size));
  memcpy(token_array, &current_sequence->token, sizeof(void*));
  // We're copying the address, not the whole array.
  *occurence_array = malloc(sizeof(htf_occurence) * current_sequence->size);
  DOFOR(i, current_sequence->size) {
    htf_token_t token = current_sequence->token[i];
    switch (token.type) {
    case HTF_TYPE_EVENT: {
      // Get the info
      struct htf_event_occurence* occurence = &(*occurence_array)[i].event_occurence;
      struct htf_event_summary* es = &reader->thread_trace->events[token.id];

      // Write it to the occurence
      memcpy(&occurence->event, &es->event, sizeof(occurence->event));
      if ((reader->options & OPTION_NO_TIMESTAMPS) == 0) {
        occurence->timestamp = reader->referential_timestamp;
        occurence->duration = es->durations[reader->event_index[token.id]];
        // Update the reader
        reader->referential_timestamp += occurence->duration;
      }

      reader->event_index[token.id]++;
      break;
    }
    case HTF_TYPE_LOOP: {
      // Get the info
      struct htf_loop_occurence* occurence = &(*occurence_array)[i].loop_occurence;
      struct htf_loop* loop = &reader->thread_trace->loops[token.id];

      // Write it to the occurence
      occurence->loop = loop;
      occurence->nb_iterations = *(int*)htf_vector_get(&loop->nb_iterations, reader->loop_index[token.id]);
      if ((reader->options & OPTION_NO_TIMESTAMPS) == 0)
        occurence->timestamp = reader->referential_timestamp;

      // Write the loop
      enter_block(reader, token);

      occurence->full_loop = malloc(sizeof(struct htf_sequence_occurence) * occurence->nb_iterations);
      occurence->duration = 0;
      DOFOR(j, occurence->nb_iterations) {
        _htf_write_sequence_occurence(reader, &occurence->full_loop[j], loop->token);
        if ((reader->options & OPTION_NO_TIMESTAMPS) == 0) {
          occurence->duration += occurence->full_loop[j].duration;
        }
      }
      leave_block(reader);

      // The reader doesn't need to be updated since it just did it for all the sequences.
      // occurence.duration = skip_sequence(reader, token);
      reader->loop_index[token.id]++;
      break;
    }
    case HTF_TYPE_SEQUENCE: {
      // Get the info
      _htf_write_sequence_occurence(reader, &(*occurence_array)[i].sequence_occurence, token);
      break;
    }
    default:
      htf_error("Invalid token type\n;");
    }
  }
  return 0;
}

int htf_read_thread_cur_token(struct htf_thread_reader* reader, struct htf_token* token, htf_occurence* e) {
  if (reader->current_frame < 0) {
    return -1; /* TODO: return EOF */
  }

  /* Get the current event */
  htf_token_t t = get_cur_token(reader);
  memcpy(token, &t, sizeof(t));
  int index = t.id;
  switch (t.type) {
  case HTF_TYPE_EVENT: {
    struct htf_event_summary* es = &reader->thread_trace->events[index];
    if (e) {
      memcpy(&e->event_occurence.event, &es->event, sizeof(e->event_occurence.event));
      if ((reader->options & OPTION_NO_TIMESTAMPS) == 0) {
        e->event_occurence.timestamp = reader->referential_timestamp;
        e->event_occurence.duration = es->durations[reader->event_index[index]];
      }
    }
    break;
  }
  case HTF_TYPE_SEQUENCE: {
    struct htf_sequence* seq = reader->thread_trace->sequences[index];
    if (e) {
      if ((reader->options & OPTION_NO_TIMESTAMPS) == 0) {
        e->sequence_occurence.timestamp = reader->referential_timestamp;
        e->sequence_occurence.duration =
          *(htf_timestamp_t*)htf_vector_get(&seq->durations, reader->sequence_index[index]);
      }
      e->sequence_occurence.sequence = seq;
      e->sequence_occurence.savestate = create_savestate(reader);
    }
    break;
  }
  case HTF_TYPE_LOOP: {
    struct htf_loop* loop = &reader->thread_trace->loops[index];
    if (e) {
      e->loop_occurence.loop = loop;
      e->loop_occurence.nb_iterations = *(int*)htf_vector_get(&loop->nb_iterations, reader->loop_index[loop->id.id]);
      if ((reader->options & OPTION_NO_TIMESTAMPS) == 0) {
        e->loop_occurence.timestamp = htf_get_starting_timestamp(reader, *token);
        e->loop_occurence.duration = _htf_get_loop_duration(reader, *token);
      }
    }
    break;
  }
  default:
    htf_error("Invalid token type\n");
  }
  return 0;
}

htf_timestamp_t htf_get_starting_timestamp(struct htf_thread_reader* reader, struct htf_token cur_token) {
  htf_timestamp_t duration;
  switch (cur_token.type) {
  case HTF_TYPE_EVENT: {
    int event_index = reader->event_index[HTF_TOKEN_ID(cur_token)];
    duration = reader->thread_trace->events[cur_token.id].durations[event_index];
    break;
  }
  case HTF_TYPE_SEQUENCE: {
    int sequence_index = reader->sequence_index[cur_token.id];
    struct htf_sequence* seq = htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(cur_token));
    duration = *(htf_timestamp_t*)htf_vector_get(&seq->durations, sequence_index);
    break;
  }
  case HTF_TYPE_LOOP: {
    struct htf_loop* loop = htf_get_loop(reader->thread_trace, HTF_TOKEN_TO_LOOP_ID(cur_token));
    struct htf_sequence* seq = htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(loop->token));
    int sequence_index = reader->sequence_index[loop->token.id];
    duration = *(htf_timestamp_t*)htf_vector_get(&seq->durations, sequence_index);
    break;
  }
  case HTF_TYPE_INVALID:
    htf_error("Invalid token type\n");
  }
  return reader->referential_timestamp + duration;
}

/** Increments the counters in the reader. */
void _skip_token(struct htf_thread_reader* reader, htf_token_t token, int nb_times) {
  switch (token.type) {
  case HTF_TYPE_EVENT: {
    reader->event_index[token.id] += nb_times;
    break;
  }
  case HTF_TYPE_SEQUENCE: {
    struct htf_sequence* seq = htf_get_sequence(reader->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(token));
    DOFOR(i, seq->size) {
      _skip_token(reader, seq->token[i], nb_times);
    }
    reader->sequence_index[token.id] += nb_times;
    htf_assert((reader->options & OPTION_NO_TIMESTAMPS) != 0 ||
               reader->sequence_index[token.id] <= seq->durations.size);
    break;
  }
  case HTF_TYPE_LOOP: {
    struct htf_loop* loop = htf_get_loop(reader->thread_trace, HTF_TOKEN_TO_LOOP_ID(token));
    htf_assert(loop->token.type == HTF_TYPE_SEQUENCE);
    int n = 0;
    DOFOR(i, nb_times) {
      // For this loop's iteration, the number of iteration of the sequence.
      n += *(int*)htf_vector_get(&loop->nb_iterations, reader->loop_index[token.id]);
      reader->loop_index[token.id]++;
    }
    _skip_token(reader, loop->token, n);
    break;
  }
  default:
    htf_error("This shouldn't have happened\n");
  }
}
void skip_sequence(struct htf_thread_reader* reader, htf_token_t token) {
  if (token.type == HTF_TYPE_SEQUENCE) {
    _skip_token(reader, token, 1);
  } else {
    htf_error("Asked to skip something that wasn't a sequence.\n");
  }
}

_Thread_local size_t savestate_memory = 0;
struct htf_savestate* create_savestate(struct htf_thread_reader* reader) {
  struct htf_savestate* new_savestate = malloc(sizeof(struct htf_savestate));
  if ((reader->options & OPTION_NO_TIMESTAMPS) == 0) {
    new_savestate->referential_timestamp = reader->referential_timestamp;
  }
  savestate_memory += sizeof(struct htf_savestate);

  new_savestate->callstack_sequence = malloc(sizeof(htf_token_t) * MAX_CALLSTACK_DEPTH);
  memcpy(new_savestate->callstack_sequence, reader->callstack_sequence, sizeof(int) * MAX_CALLSTACK_DEPTH);

  new_savestate->callstack_index = malloc(sizeof(int) * MAX_CALLSTACK_DEPTH);
  memcpy(new_savestate->callstack_index, reader->callstack_index, sizeof(int) * MAX_CALLSTACK_DEPTH);

  new_savestate->callstack_loop_iteration = malloc(sizeof(int) * MAX_CALLSTACK_DEPTH);
  memcpy(new_savestate->callstack_loop_iteration, reader->callstack_loop_iteration, sizeof(int) * MAX_CALLSTACK_DEPTH);
  savestate_memory += MAX_CALLSTACK_DEPTH * 3 * sizeof(int);

  new_savestate->current_frame = reader->current_frame;

  new_savestate->event_index = malloc(sizeof(int) * reader->thread_trace->nb_events);
  memcpy(new_savestate->event_index, reader->event_index, sizeof(int) * reader->thread_trace->nb_events);
  savestate_memory += reader->thread_trace->nb_events * sizeof(int);

  new_savestate->sequence_index = malloc(sizeof(int) * reader->thread_trace->nb_sequences);
  memcpy(new_savestate->sequence_index, reader->sequence_index, sizeof(int) * reader->thread_trace->nb_sequences);
  savestate_memory += reader->thread_trace->nb_sequences * sizeof(int);

  new_savestate->loop_index = malloc(sizeof(int) * reader->thread_trace->nb_loops);
  memcpy(new_savestate->loop_index, reader->loop_index, sizeof(int) * reader->thread_trace->nb_loops);
  savestate_memory += reader->thread_trace->nb_loops * sizeof(int);

  return new_savestate;
}

void load_savestate(struct htf_thread_reader* reader, struct htf_savestate* savestate) {
  if ((reader->options & OPTION_NO_TIMESTAMPS) == 0)
    reader->referential_timestamp = savestate->referential_timestamp;
  memcpy(reader->callstack_sequence, savestate->callstack_sequence, sizeof(int) * MAX_CALLSTACK_DEPTH);
  memcpy(reader->callstack_index, savestate->callstack_index, sizeof(int) * MAX_CALLSTACK_DEPTH);
  memcpy(reader->callstack_loop_iteration, savestate->callstack_loop_iteration, sizeof(int) * MAX_CALLSTACK_DEPTH);
  reader->current_frame = savestate->current_frame;
  memcpy(reader->event_index, savestate->event_index, sizeof(int) * reader->thread_trace->nb_events);
  memcpy(reader->sequence_index, savestate->sequence_index, sizeof(int) * reader->thread_trace->nb_sequences);
  memcpy(reader->loop_index, savestate->loop_index, sizeof(int) * reader->thread_trace->nb_loops);
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
