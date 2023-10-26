/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
/** @file
 * Everything needed to read a trace.
 */
#pragma once

#include "htf.h"
#include "htf_attribute.h"
#include "htf_timestamp.h"

#ifdef __cplusplus
namespace htf {
#endif
#define MAX_CALLSTACK_DEPTH 100

enum ThreadReaderOptions {
  None = 0,
  ShowStructure = 1,
  NoTimestamps = 2,
};

/** Only used when reading a trace, links an event with a timestamp. */
typedef struct EventOccurence {
  struct Event* event;
  htf_timestamp_t timestamp;
  htf_timestamp_t duration;
  AttributeList* attributes;
} EventOccurence;

typedef struct SequenceOccurence {
  struct Sequence* sequence;
  struct Savestate* savestate;
  htf_timestamp_t timestamp;
  htf_timestamp_t duration;
  /**Occurences of the events in this sequence. */
  struct TokenOccurence* full_sequence;
} SequenceOccurence;

typedef struct LoopOccurence {
  struct Loop* loop;
  unsigned int nb_iterations;
  htf_timestamp_t timestamp;
  htf_timestamp_t duration;
  struct SequenceOccurence* full_loop;
  struct SequenceOccurence loop_summary;
} LoopOccurence;

typedef union Occurence {
  struct LoopOccurence loop_occurence;
  struct SequenceOccurence sequence_occurence;
  struct EventOccurence event_occurence;
} Occurence;

typedef struct TokenOccurence {
  const Token* token;
  Occurence* occurence;
} TokenOccurence;

typedef struct ThreadReader {
  /** Archive being read by this reader. */
  struct Archive* archive;
  /** Thread being read. */
  struct Thread* thread_trace;
  /** The current referential timestamp. */
  htf_timestamp_t referential_timestamp;

  /** Stack containing the sequences/loops being read. */
  Token callstack_sequence[MAX_CALLSTACK_DEPTH];

  /** Stack containing the index in the sequence or the loop iteration. */
  int callstack_index[MAX_CALLSTACK_DEPTH];

  /** Stack containing the number of iteration of the loop at the corresponding frame */
  int callstack_loop_iteration[MAX_CALLSTACK_DEPTH];

  /** Current frame = index of the event/loop being read in the callstacks.
   * You can view this as the "depth" of the callstack. */
  int current_frame;

  /** At any point, a token t has been seen tokenCount[t] times. */
  DEFINE_TokenCountMap(tokenCount);

  int options;
#ifdef __cplusplus
  ThreadReader(Archive* archive, ThreadId threadId, int options);

 private:
  /* Returns the Sequence being run at the given frame. */
  [[nodiscard]] const Token& getFrameInCallstack(int frame_number) const;
  /* Returns the token being run at the given frame. */
  [[nodiscard]] const Token& getTokenInCallstack(int frame_number) const;
  /* Prints the current Token. */
  void printCurToken() const;
  /* Returns the current Sequence*/
  [[nodiscard]] const Token& getCurSequence() const;
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
  [[nodiscard]] htf_timestamp_t getLoopDuration(Token loop_id) const;

  /** Returns an EventOccurence for the given Token appearing at the given occurence_id.
   * Timestamp is set to Reader's referential timestamp.*/
  [[nodiscard]] struct EventOccurence& getEventOccurence(Token event_id, int occurence_id) const;
  /** Returns an SequenceOccurence for the given Token appearing at the given occurence_id.
   * Timestamp is set to Reader's referential timestamp.*/
  [[nodiscard]] struct SequenceOccurence& getSequenceOccurence(Token sequence_id, int occurence_id) const;
  /** Returns an LoopOccurence for the given Token appearing at the given occurence_id.
   * Timestamp is set to Reader's referential timestamp.*/
  [[nodiscard]] struct LoopOccurence& getLoopOccurence(Token loop_id, int occurence_id) const;

  /* TODO Write a description here. Also fix the way it's implemented: it does not fit the standard. */
  [[nodiscard]] AttributeList* getEventAttributeList(Token event_id, int occurence_id) const;

  /* Skips the given Token and updates the reader. */
  static void skipToken(Token token) { htf_error("Not implemented yet\n"); };

 public:
  /* Enter a block (push a new frame in the callstack) */
  void enterBlock(Token new_block);
  /* Leaves the current block */
  void leaveBlock();
  /** Moves the reader's position to the next token.
   * Moves to the next token in the current block, or exits it recursively as long as it's at the end of a block.*/
  void moveToNextToken();
  /** Updates the reader's callstacks et al. by reading the current token.
   * If the current Token is an event, its index is updated and the referential timestamp as well.
   * If it's a Loop or a Sequence, their indexes are updated, and the reader enters the corresponding block. */
  void updateReadCurToken();
  /** Fetches the next Token in the trace. Changes the state of the reader to match. */
  [[nodiscard]] Token getNextToken();
  /** Returns the current Token. */
  [[nodiscard]] const Token& getCurToken() const;
  /** Returns an Occurence for the given Token appearing at the given occurence_id.
   * Timestamp is set to Reader's referential timestamp.*/
  [[nodiscard]] union Occurence& getOccurence(Token id, int occurence_id) const;
  /** Loads the given savestate. */
  void loadSavestate(struct Savestate* savestate);
  /** Reads the current level of the thread, and returns it as an array of TokenOccurences. */
  [[nodiscard]] std::vector<TokenOccurence> readCurrentLevel();
  /** Skips the given Sequence and updates the reader. */
  void skipSequence(Token token) { htf_error("Not implemented yet\n"); };
#endif
} ThreadReader;

/**
 * A savestate of a htf_thread_reader.
 */
typedef struct Savestate {
  /** The current referential timestamp. */
  htf_timestamp_t referential_timestamp;

  /** Stack containing the sequences/loops being read. */
  Token* callstack_sequence;

  /** Stack containing the index in the sequence or the loop iteration. */
  int* callstack_index;

  /** Stack containing the number of iteration of the loop at the corresponding frame */
  int* callstack_loop_iteration;

  /** Current frame = index of the event/loop being read in the callstacks.
   * You can view this as the "depth" of the callstack. */
  int current_frame;

  DEFINE_TokenCountMap(tokenCount);
#ifdef __cplusplus
  /* Creates a Savestate (ie a screenshot) of the reader at the moment. */
  Savestate(const ThreadReader* reader);
#endif
} Savestate;

#ifdef __cplusplus
}; /* namespace htf */

C_CXX(_Thread_local, thread_local) extern size_t savestate_memory;

extern "C" {
#endif

/* Creates and initializes a Thread Reader. */
extern HTF(ThreadReader) *
  htf_new_thread_reader(HTF(Archive) * archive, HTF(ThreadId) thread_id, enum HTF(ThreadReaderOptions) options);
/* Enter a block (push a new frame in the callstack) */
extern void htf_thread_reader_enter_block(HTF(ThreadReader) * reader, HTF(Token) new_block);
/* Leaves the current block */
extern void htf_thread_reader_leave_block(HTF(ThreadReader) * reader);

/** Creates a savestate from a reader. */
extern HTF(Savestate) * create_savestate(HTF(ThreadReader) * reader);
/** Loads a savestate to a reader. */
extern void load_savestate(HTF(ThreadReader) * reader, HTF(Savestate) * savestate);

/* Reads the current token and writes it somewhere / returns it. TODO: Implement this. */
extern int htf_read_thread_cur_token(HTF(ThreadReader) * reader);
/** Moves the reader to the next repeated_token in the thread. */
extern int htf_move_to_next_token(HTF(ThreadReader) * reader);

/** Reads the whole current level, and writes it somewhere / returns it. TODO: Implement this.*/
extern int htf_read_thread_cur_level(HTF(ThreadReader) * reader);

/** Skips the given sequence entirely. TODO: Implement this. */
extern void skip_sequence(HTF(ThreadReader) * reader, HTF(Token) token);

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
