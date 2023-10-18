/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
#pragma once

#include "htf.h"
#include "htf_attribute.h"
#include "htf_timestamp.h"

#ifdef __cplusplus
namespace htf {
#endif
#define MAX_CALLSTACK_DEPTH 100

#define ThreadReaderOptionsName C_CXX(htf_thread_reader_options, ThreadReaderOptions)
enum ThreadReaderOptionsName {
  C_CXX(htf_thread_reader_option_none, None) = 0,
  C_CXX(htf_thread_reader_option_show_structure, ShowStructure) = 1,
  C_CXX(htf_thread_reader_option_no_timestamps, NoTimestamps) = 2,
};

#define ThreadReaderName C_CXX(htf_thread_reader, ThreadReader)
typedef struct ThreadReaderName {
  /** Archive being read by this reader. */
  struct ArchiveName* archive;
  /** Thread being read. */
  struct ThreadName* thread_trace;
  /** The current referential timestamp. */
  htf_timestamp_t referential_timestamp;

  /** Stack containing the sequences/loops being read. */
  TokenName callstack_sequence[MAX_CALLSTACK_DEPTH];

  /** Stack containing the index in the sequence or the loop iteration. */
  int callstack_index[MAX_CALLSTACK_DEPTH];

  /** Stack containing the number of iteration of the loop at the corresponding frame */
  int callstack_loop_iteration[MAX_CALLSTACK_DEPTH];

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

  enum ThreadReaderOptionsName options;
#ifdef __cplusplus
  ThreadReader(Archive* archive, ThreadId threadId, ThreadReaderOptions options);

 private:
  /* Returns the Sequence being run at the given frame. */
  [[nodiscard]] Token getFrameInCallstack(int frame_number) const;
  /* Returns the token being run at the given frame. */
  [[nodiscard]] Token getTokenInCallstack(int frame_number) const;
  /* Returns the current Token. */
  [[nodiscard]] Token getCurToken() const;
  /* Prints the current Token. */
  void printCurToken() const;
  /* Returns the current Sequence*/
  [[nodiscard]] Token getCurSequence() const;
  /* Prints the current Sequence. */
  void printCurSequence() const;
  /* Prints the whole current callstack. */
  void printCallstack() const;
  /* Returns the EventSummary of the given Event. */
  [[nodiscard]] EventSummary* getEventSummary(Token event) const;
  /* Returns the timestamp of the given event occurring at the given index. */
  [[nodiscard]] htf_timestamp_t getEventTimestamp(Token event, int occurence_id) const;
  /* Returns whether the given sequence still has more Tokens after the given current_index. */
  [[nodiscard]] bool isEndOfSequence(int current_index, Token sequence_id) const;
  /* Returns whether the given loop still has more Tokens after the given current_index. */
  [[nodiscard]] bool isEndOfLoop(int current_index, Token loop_id) const;
  /* Returns the duration of the given Loop. */
  htf_timestamp_t getLoopDuration(Token loop_id);

  // TODO This is shitty.
  void copyEventOccurence(Token event_id, int occurence_id, struct EventOccurence* event_occurence) const;
  // TODO This is very shitty.
  void writeSequenceOccurence(struct SequenceOccurence* sequence_occurence, Token token);
  /* TODO Write a description here. Also fix the way it's implemented: it does not fit the standard. */
  [[nodiscard]] AttributeList* getEventAttributeList(Token event_id, int occurence_id) const;

  /* Skips the given Token and updates the reader. */
  static void skipToken(Token token) { htf_error("Not implemented yet\n"); };

 public:
  /* Enter a block (push a new frame in the callstack) */
  void enterBlock(Token new_block);
  /* Leaves the current block */
  void leaveBlock();
  /* Fetches the next Token in the trace. */
  [[nodiscard]] Token getNextToken();
  /* Loads the given savestate. */
  void loadSavestate(struct Savestate* savestate);
  /* Reads the current level of the thread. */
  [[nodiscard]] std::pair<Token, union Occurence>* readCurrentLevel();
  /* Skips the given Sequence and updates the reader. */
  void skipSequence(Token token) { htf_error("Not implemented yet\n"); };
#endif
} ThreadReaderName;

#define SavestateName C_CXX(htf_savestate, Savestate)
/**
 * A savestate of a htf_thread_reader.
 */
typedef struct SavestateName {
  /** The current referential timestamp. */
  htf_timestamp_t referential_timestamp;

  /** Stack containing the sequences/loops being read. */
  TokenName* callstack_sequence;

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
#ifdef __cplusplus
 public:
  /* Creates a Savestate (ie a screenshot) of the reader at the moment. */
  Savestate(ThreadReader* reader);
#endif
} SavestateName;

#define EventOccurenceName C_CXX(htf_event_occurence, EventOccurence)
/** Only used when reading a trace, links an event with a timestamp. */
typedef struct EventOccurenceName {
  struct EventName event;
  htf_timestamp_t timestamp;
  htf_timestamp_t duration;
  AttributeListName* attributes;
} EventOccurenceName;

#define SequenceOccurenceName C_CXX(htf_sequence_occurence, SequenceOccurence)
typedef struct SequenceOccurenceName {
  struct SequenceName* sequence;
  struct SavestateName* savestate;
  htf_timestamp_t timestamp;
  htf_timestamp_t duration;
  /**Occurences of the events in this sequence. */
  union OccurenceName* full_sequence;
} SequenceOccurenceName;

#define LoopOccurenceName C_CXX(htf_loop_occurence, LoopOccurence)
typedef struct LoopOccurenceName {
  struct LoopName* loop;
  unsigned int nb_iterations;
  htf_timestamp_t timestamp;
  htf_timestamp_t duration;
  struct SequenceOccurenceName* full_loop;
} LoopOccurenceName;

#define OccurenceName C_CXX(htf_occurence, Occurence)
typedef union OccurenceName {
  struct LoopOccurenceName loop_occurence;
  struct SequenceOccurenceName sequence_occurence;
  struct EventOccurenceName event_occurence;
} OccurenceName;

#ifdef __cplusplus
}; /* namespace htf */

C_CXX(_Thread_local, thread_local) extern size_t savestate_memory;

extern "C" {
#endif

/* Creates and initializes a Thread Reader. */
extern HTF(ThreadReaderName) * htf_new_thread_reader(HTF(ArchiveName) * archive,
                                                     HTF(ThreadIdName) thread_id,
                                                     enum HTF(ThreadReaderOptionsName) options);
/* Enter a block (push a new frame in the callstack) */
extern void htf_thread_reader_enter_block(HTF(ThreadReaderName) * reader, HTF(TokenName) new_block);
/* Leaves the current block */
extern void htf_thread_reader_leave_block(HTF(ThreadReaderName) * reader);

/** Creates a savestate from a reader. */
extern HTF(SavestateName) * create_savestate(HTF(ThreadReaderName) * reader);
/** Loads a savestate to a reader. */
extern void load_savestate(HTF(ThreadReaderName) * reader, HTF(SavestateName) * savestate);

/* Reads the current token and writes it somewhere / returns it. TODO: Implement this. */
extern int htf_read_thread_cur_token(HTF(ThreadReaderName) * reader);
/** Moves the reader to the next repeated_token in the thread. */
extern int htf_move_to_next_token(HTF(ThreadReaderName) * reader);

/** Reads the whole current level, and writes it somewhere / returns it. TODO: Implement this.*/
extern int htf_read_thread_cur_level(HTF(ThreadReaderName) * reader);

/** Skips the given sequence entirely. TODO: Implement this. */
extern void skip_sequence(HTF(ThreadReaderName) * reader, HTF(TokenName) token);

#ifdef __cplusplus
};
#endif

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
