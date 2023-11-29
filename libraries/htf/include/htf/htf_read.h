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
/** Maximum Callstack Size. */
#define MAX_CALLSTACK_DEPTH 100

/**
 * Options for the ThreadReader. Values have to be powers of 2.
 */
enum ThreadReaderOptions {
  None = 0,          /**< No specific options. */
  ShowStructure = 1, /**< Read and return the structure (Sequences and Loop). */
  NoTimestamps = 2,  /**< Do not load the timestamps / durations. */
};

/** @brief Represents one occurence of an Event. */
typedef struct EventOccurence {
  struct Event* event;       /**< Pointer to the Event.*/
  htf_timestamp_t timestamp; /**< Timestamp for that occurence.*/
  htf_timestamp_t duration;  /**< Duration of that occurence.*/
  AttributeList* attributes; /**< Attributes for that occurence.*/
} EventOccurence;

/**
 * @brief Represents one occurence of a Sequence.
 */
typedef struct SequenceOccurence {
  struct Sequence* sequence;            /**< Pointer to the Sequence.*/
  struct Savestate* savestate;          /**< Savestate of the reader before entering the sequence.*/
  htf_timestamp_t timestamp;            /**< Timestamp for that occurence.*/
  htf_timestamp_t duration;             /**< Duration of that occurence.*/
  struct TokenOccurence* full_sequence; /** Array of the occurrences in this sequence. */
} SequenceOccurence;

/**
 * @brief Represents one occurence of a Loop.
 */
typedef struct LoopOccurence {
  struct Loop* loop;                     /**< Pointer to the Loop.*/
  unsigned int nb_iterations;            /**< Number of iterations for that occurence.*/
  htf_timestamp_t timestamp;             /**< Timestamp for that occurence.*/
  htf_timestamp_t duration;              /**< Duration for that occurence.*/
  struct SequenceOccurence* full_loop;   /**< Array of the Sequences in this loop.*/
  struct SequenceOccurence loop_summary; /**< False SequenceOccurence that represents a summary of all the
                                          * occurrences in full_loop. */
} LoopOccurence;

/**
 * @brief Represents any kind of Occurrence.
 */
typedef union Occurence {
  struct LoopOccurence loop_occurence;         /**< Occurence for a Loop.*/
  struct SequenceOccurence sequence_occurence; /**< Occurence for a Sequence.*/
  struct EventOccurence event_occurence;       /**< Occurence for an Event.*/
} Occurence;

/**
 * @brief Tuple containing a Token and its corresponding Occurence.
 */
typedef struct TokenOccurence {
  /** Token for the occurence. */
  const Token* token;
  /** Occurence corresponding to the Token. */
  Occurence* occurence;
} TokenOccurence;

/**
 * @brief Reads one thread from an HTF trace.
 */
typedef struct ThreadReader {
  /** Archive being read by this reader. */
  const struct Archive* archive;
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

  /**
   * @brief Options as defined in htf::ThreadReaderOptions.
   */
  int options;
#ifdef __cplusplus
  /**
   * @brief Make a new ThreadReader from an Archive and a threadId.
   * @param archive Archive to read.
   * @param threadId Id of the thread to read.
   * @param options Options as defined in ThreadReaderOptions.
   */
  ThreadReader(const Archive* archive, ThreadId threadId, int options);

 private:
  /** Returns the Sequence being run at the given frame. */
  [[nodiscard]] const Token& getFrameInCallstack(int frame_number) const;
  /** Returns the token being run at the given frame. */
  [[nodiscard]] const Token& getTokenInCallstack(int frame_number) const;
  /** Prints the current Token. */
  void printCurToken() const;
  /** Returns the current Sequence*/
  [[nodiscard]] const Token& getCurSequence() const;
  /** Prints the current Sequence. */
  void printCurSequence() const;
  /** Prints the whole current callstack. */
  void printCallstack() const;
  /** Returns the EventSummary of the given Event. */
  [[nodiscard]] EventSummary* getEventSummary(Token event) const;
  /** Returns the timestamp of the given event occurring at the given index. */
  [[nodiscard]] htf_timestamp_t getEventTimestamp(Token event, int occurence_id) const;
  /** Returns whether the given sequence still has more Tokens after the given current_index. */
  [[nodiscard]] bool isEndOfSequence(int current_index, Token sequence_id) const;
  /** Returns whether the given loop still has more Tokens after the given current_index. */
  [[nodiscard]] bool isEndOfLoop(int current_index, Token loop_id) const;
  /** Returns the duration of the given Loop. */
  [[nodiscard]] htf_timestamp_t getLoopDuration(Token loop_id) const;

  /** Returns an EventOccurence for the given Token appearing at the given occurence_id.
   * Timestamp is set to Reader's referential timestamp.*/
  [[nodiscard]] EventOccurence getEventOccurence(Token event_id, int occurence_id) const;
  /** Returns an SequenceOccurence for the given Token appearing at the given occurence_id.
   * Timestamp is set to Reader's referential timestamp.*/
  [[nodiscard]] SequenceOccurence getSequenceOccurence(Token sequence_id, int occurence_id) const;
  /** Returns an LoopOccurence for the given Token appearing at the given occurence_id.
   * Timestamp is set to Reader's referential timestamp.*/
  [[nodiscard]] LoopOccurence getLoopOccurence(Token loop_id, int occurence_id) const;

  /** \todo Write a description here. Also fix the way it's implemented: it does not fit the standard. */
  [[nodiscard]] AttributeList* getEventAttributeList(Token event_id, int occurence_id) const;

  /** Skips the given Token and updates the reader. */
  static void skipToken([[maybe_unused]] Token token) { htf_error("Not implemented yet\n"); };

 public:
  /** @brief Enter a block (push a new frame in the callstack) */
  void enterBlock(Token new_block);
  /** @brief Leaves the current block */
  void leaveBlock();
  /** @brief Moves the reader's position to the next token.
   *
   * Moves to the next token in the current block, or exits it recursively as long as it's at the end of a block.*/
  void moveToNextToken();
  /** @brief Updates the reader's callstacks et al. by reading the current token.
   *
   * If the current Token is an event, its index is updated and the referential timestamp as well.
   * If it's a Loop or a Sequence, their indexes are updated, and the reader enters the corresponding block. */
  void updateReadCurToken();
  /** @brief Fetches the next Token in the trace. Changes the state of the reader to match. */
  [[nodiscard]] Token getNextToken();
  /** @brief Returns the current Token. */
  [[nodiscard]] const Token& getCurToken() const;
  /** @brief Returns an Occurence for the given Token appearing at the given occurence_id.
   *
   * Timestamp is set to Reader's referential timestamp.*/
  [[nodiscard]] union Occurence* getOccurence(Token id, int occurence_id) const;
  /** @brief Loads the given savestate. */
  void loadSavestate(struct Savestate* savestate);
  /** @brief Reads the current level of the thread, and returns it as an array of TokenOccurences. */
  [[nodiscard]] std::vector<TokenOccurence> readCurrentLevel();
  /** @brief Skips the given Sequence and updates the reader. */
  void skipSequence([[maybe_unused]] Token token) { htf_error("Not implemented yet\n"); };
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
  /** @brief Creates a savestate of the given reader.
   * @param reader Reader whose state of reading we want to take a screenshot. */
  Savestate(const ThreadReader* reader);
#endif
} Savestate;

#ifdef __cplusplus
}; /* namespace htf */

/**
 * Counter for how much memory the savestates consume.
 */
C_CXX(_Thread_local, thread_local) extern size_t savestate_memory;

extern "C" {
#endif

/** Creates and initializes a ThreadReader. */
extern HTF(ThreadReader) * htf_new_thread_reader(const HTF(Archive) * archive, HTF(ThreadId) thread_id, int options);
/** Enter a block (push a new frame in the callstack) */
extern void htf_thread_reader_enter_block(HTF(ThreadReader) * reader, HTF(Token) new_block);
/** Leaves the current block */
extern void htf_thread_reader_leave_block(HTF(ThreadReader) * reader);

/** @brief Moves the reader's position to the next token.
 *
 * Moves to the next token in the current block, or exits it recursively as long as it's at the end of a block.*/
extern void htf_thread_reader_move_to_next_token(HTF(ThreadReader) * reader);

/** @brief Updates the reader's callstacks et al. by reading the current token.
 *
 * If the current Token is an event, its index is updated and the referential timestamp as well.
 * If it's a Loop or a Sequence, their indexes are updated, and the reader enters the corresponding block. */
extern void htf_thread_reader_update_reader_cur_token(HTF(ThreadReader) * reader);

/** @brief Fetches the next Token in the trace. Changes the state of the reader to match. */
extern HTF(Token) htf_thread_reader_get_next_token(HTF(ThreadReader) * reader);

/** @brief Returns the current Token. */
extern HTF(Token) htf_read_thread_cur_token(const HTF(ThreadReader) * reader);

/** @brief Returns an Occurence for the given Token appearing at the given occurence_id.
 *
 * Timestamp is set to Reader's referential timestamp.*/
extern union HTF(Occurence) *
  htf_thread_reader_get_occurence(const HTF(ThreadReader) * reader, HTF(Token) id, int occurence_id);

/** @brief Creates a new savestate from the reader. */
extern HTF(Savestate) * create_savestate(const HTF(ThreadReader) * reader);

/** @brief Loads the given savestate. */
extern void load_savestate(HTF(ThreadReader) * reader, HTF(Savestate) * savestate);

/** @brief Reads the current level of the thread, and returns it as an array of TokenOccurences. */
extern HTF(TokenOccurence) * htf_thread_reader_read_current_level(HTF(ThreadReader) * reader);

/** @brief Skips the given Sequence and updates the reader. */
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
