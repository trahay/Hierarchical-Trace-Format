/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#ifndef HTF_READ_H
#define HTF_READ_H

#include "htf.h"
#include "htf_timestamp.h"

enum thread_reader_option {
  OPTION_NONE = 0,
  OPTION_SHOW_STRUCTURE = 1,
};

struct htf_thread_reader {
  /** Archive being read by this reader. */
  struct htf_archive* archive;
  /** Thread being read. */
  struct htf_thread* thread_trace;
  /** The current referential timestamp. */
  htf_timestamp_t referential_timestamp;

  /** Stack containing the sequences/loops being read. */
  htf_token_t* callstack_sequence;

  /** Stack containing the index in the sequence or the loop iteration. */
  int* callstack_index;

  /** Stack containing the number of iteration of the loop at the corresponding frame */
  int* callstack_loop_iteration;

  /** Current frame = index of the event/loop being read in the callstacks.
   * You can view this as the "depth" of the callstack. */
  int current_frame;

  /** At any point, an event e has been seen event_index[e.id] times.
   * Use this to grab the timestamps and other information on the event. */
  int* event_index;
  /** At any point, a sequence s has been seen sequence_index[s.id] times.
   * Use this to grab the timestamps and other information on the sequence. */
  int* sequence_index;
  /** At any point, a loop l has been seen loop_index[l.id] times.
   * Use this to grab the timestamps and other information on the sequence. */
  int* loop_index;

  enum thread_reader_option options;
};

/**
 * A savestate of a htf_thread_reader.
 */
struct htf_savestate {
  /** The current referential timestamp. */
  htf_timestamp_t referential_timestamp;

  /** Stack containing the sequences/loops being read. */
  htf_token_t* callstack_sequence;

  /** Stack containing the index in the sequence or the loop iteration. */
  int* callstack_index;

  /** Stack containing the number of iteration of the loop at the corresponding frame */
  int* callstack_loop_iteration;

  /** Current frame = index of the event/loop being read in the callstacks.
   * You can view this as the "depth" of the callstack. */
  int current_frame;

  /** At any point, an event e has been seen event_index[e.id] times.
   * Use this to grab the timestamps and other information on the event. */
  int* event_index;
  /** At any point, a sequence s has been seen sequence_index[s.id] times.
   * Use this to grab the timestamps and other information on the sequence. */
  int* sequence_index;
  /** At any point, a loop l has been seen loop_index[l.id] times.
   * Use this to grab the timestamps and other information on the sequence. */
  int* loop_index;
};
/** Only used when reading a trace, links an event with a timestamp. */
struct htf_event_occurence {
  struct htf_event event;
  htf_timestamp_t timestamp;
  htf_timestamp_t duration;
};

struct htf_sequence_occurence {
  struct htf_sequence* sequence;
  struct htf_savestate* savestate;
  htf_timestamp_t timestamp;
  htf_timestamp_t duration;
  /**Occurences of the events in this sequence. */
  void* full_sequence;
  // TODO Probably not a good thing to make it a void * ?
  //  But I can't think of any other way to make it work
  //  Should ask Francois ?
};

struct htf_loop_occurence {
  struct htf_loop* loop;
  unsigned int nb_iterations;
  htf_timestamp_t timestamp;
  htf_timestamp_t duration;
  struct htf_sequence_occurence* full_loop;
};

typedef union {
  struct htf_loop_occurence loop_occurence;
  struct htf_sequence_occurence sequence_occurence;
  struct htf_event_occurence event_occurence;
} htf_occurence;

_Thread_local extern size_t savestate_memory;
void enter_block(struct htf_thread_reader* reader, htf_token_t new_block);
void leave_block(struct htf_thread_reader* reader);
/** Creates a savestate from a reader. */
struct htf_savestate* create_savestate(struct htf_thread_reader* reader);
/** Loads a savestate to a reader. */
void load_savestate(struct htf_thread_reader* reader, struct htf_savestate* savestate);

void htf_read_global_archive(struct htf_archive* archive, char* main_filename);

void htf_read_archive(struct htf_archive* archive, char* filename);

void htf_read_thread_iterator_init(struct htf_archive* archive,
                                   struct htf_thread_reader* reader,
                                   htf_thread_id_t thread_id,
                                   enum thread_reader_option options);

/**
 * Copies the value of the current token in a thread to the given pointer.
 * If it is an event, copies the informations to the event_occurence pointer.
 * Be aware that exiting a block is not a token, but entering is.
 * @return -1 in case of an error (such as the end of the trace), else 0.
 */
int htf_read_thread_cur_token(struct htf_thread_reader* reader, struct htf_token* t, htf_occurence* e);
/** Moves the reader to the next token in the thread. */
int htf_move_to_next_token(struct htf_thread_reader* reader);

/** Reads the whole current level of the thread and writes it to the level.
 * Does the malloc on its own, so you should pass a pointer to NULL to begin with.*/
int htf_read_thread_cur_level(struct htf_thread_reader* reader,
                              htf_occurence** occurence_array,
                              struct htf_token** token_array,
                              int* length);
/** Skips the given sequence entirely, returns how much duration was skipped.
 * The reader must be located at that token (contrary to htf_get_starting_timestamp). */
htf_timestamp_t skip_sequence(struct htf_thread_reader* reader, htf_token_t token);
/** Returns the timestamp at which the given Event, Sequence or Loop starts, without modifying the reader's
 * state. */
htf_timestamp_t htf_get_starting_timestamp(struct htf_thread_reader* reader, struct htf_token token);

/** Returns the duraiton of the given Event, Sequence or Loop, without modifying the reader's state. */
htf_timestamp_t htf_get_duration(struct htf_thread_reader* reader, struct htf_token token);
#define MAX_CALLSTACK_DEPTH 100

static int _reader_show_structure(struct htf_thread_reader* reader) __attribute__((unused));
static int _reader_show_structure(struct htf_thread_reader* reader) {
  return reader->options & OPTION_SHOW_STRUCTURE;
}
#endif /* HTF_READ_H */

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
