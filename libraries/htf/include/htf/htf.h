/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
#pragma once

#include <pthread.h>
#include "LinkedVector.h"
#include "htf_dbg.h"
#include "htf_timestamp.h"

#ifdef __cplusplus
#include <cstring>
#include <map>
#else
#include <string.h>
#endif

#ifdef __cplusplus
namespace htf {
#endif

/*************************** Tokens **********************/

/**
 * A trace is composed of basic units called tokens.
 * A token is either:
 *   - an event
 *   - a sequence (ie a list of tokens)
 *   - a loop (a repetition of sequences)
 */

/**
 * Enumeration of token types
 */
enum TokenType { TypeInvalid = 0, TypeEvent = 1, TypeSequence = 2, TypeLoop = 3 };

/**
 * Match numerical token type with a character
 * TypeInvalid = 'I'
 * TypeEvent = 'E'
 * TypeSequence = 'S'
 * TypeLoop = 'L'
 * 'U' otherwise
 */
#define HTF_TOKEN_TYPE_C(t)        \
  (t.type) == TypeInvalid    ? 'I' \
  : (t.type) == TypeEvent    ? 'E' \
  : (t.type) == TypeSequence ? 'S' \
  : (t.type) == TypeLoop     ? 'L' \
                             : 'U'

/**
 * Useful macros
 */
#define HTF_TOKEN_ID_INVALID 0x3fffffff
#define HTF_EVENT_ID_INVALID HTF_TOKEN_ID_INVALID
#define HTF_SEQUENCE_ID_INVALID HTF_TOKEN_ID_INVALID
#define HTF_LOOP_ID_INVALID HTF_TOKEN_ID_INVALID

/**
 * Definition of the type for a token ID
 */
typedef uint32_t TokenId;

/**
 * A token is defined as a structure composed of
 *  - its type (2bits)
 *  - its id (30bits )
 */
typedef struct Token {
  enum TokenType type : 2;
  TokenId id : 30;
#ifdef __cplusplus
  Token(TokenType type, uint32_t id) {
    this->type = type;
    this->id = id;
  }
  // Default token is an invalid one
  Token() {
    type = TypeInvalid;
    id = HTF_TOKEN_ID_INVALID;
  }

 public:
  bool operator==(const Token& other) const { return (other.type == type && other.id == id); }
  bool operator<(const Token& other) const { return (type < other.type || (type == other.type && id < other.id)); }
  /* Returns true if the Token is a Sequence or a Loop. */
  inline bool isIterable() const { return type == TypeSequence || type == TypeLoop; }
#endif
} Token;

#define HTF_EVENT_ID(i) HTF(Token)(HTF(TypeEvent), i)
#define HTF_SEQUENCE_ID(i) HTF(Token)(HTF(TypeSequence), i)
#define HTF_LOOP_ID(i) HTF(Token)(HTF(TypeLoop), i)

/*************************** Events **********************/
/**
 * Enumeration of event types
 */
enum EventType {
  HTF_BLOCK_START,
  HTF_BLOCK_END,
  HTF_SINGLETON,
};

/**
 * Enumeration of the different events that are recorded by HTF
 */
enum Record {
  /** Event record identifier for the BufferFlush event. */
  HTF_EVENT_BUFFER_FLUSH = 0,
  /** Event record identifier for the MeasurementOnOff event. */
  HTF_EVENT_MEASUREMENT_ON_OFF = 1,
  /** Event record identifier for the Enter event. */
  HTF_EVENT_ENTER = 2,
  /** Event record identifier for the Leave event. */
  HTF_EVENT_LEAVE = 3,
  /** Event record identifier for the MpiSend event. */
  HTF_EVENT_MPI_SEND = 4,
  /** Event record identifier for the MpiIsend event. */
  HTF_EVENT_MPI_ISEND = 5,
  /** Event record identifier for the MpiIsendComplete event. */
  HTF_EVENT_MPI_ISEND_COMPLETE = 6,
  /** Event record identifier for the MpiIrecvRequest event. */
  HTF_EVENT_MPI_IRECV_REQUEST = 7,
  /** Event record identifier for the MpiRecv event. */
  HTF_EVENT_MPI_RECV = 8,
  /** Event record identifier for the MpiIrecv event. */
  HTF_EVENT_MPI_IRECV = 9,
  /** Event record identifier for the MpiRequestTest event. */
  HTF_EVENT_MPI_REQUEST_TEST = 10,
  /** Event record identifier for the MpiRequestCancelled event. */
  HTF_EVENT_MPI_REQUEST_CANCELLED = 11,
  /** Event record identifier for the MpiCollectiveBegin event. */
  HTF_EVENT_MPI_COLLECTIVE_BEGIN = 12,
  /** Event record identifier for the MpiCollectiveEnd event. */
  HTF_EVENT_MPI_COLLECTIVE_END = 13,
  /** Event record identifier for the OmpFork event. */
  HTF_EVENT_OMP_FORK = 14,
  /** Event record identifier for the OmpJoin event. */
  HTF_EVENT_OMP_JOIN = 15,
  /** Event record identifier for the OmpAcquireLock event. */
  HTF_EVENT_OMP_ACQUIRE_LOCK = 16,
  /** Event record identifier for the OmpReleaseLock event. */
  HTF_EVENT_OMP_RELEASE_LOCK = 17,
  /** Event record identifier for the OmpTaskCreate event. */
  HTF_EVENT_OMP_TASK_CREATE = 18,
  /** Event record identifier for the OmpTaskSwitch event. */
  HTF_EVENT_OMP_TASK_SWITCH = 19,
  /** Event record identifier for the OmpTaskComplete event. */
  HTF_EVENT_OMP_TASK_COMPLETE = 20,
  /** Event record identifier for the Metric event. */
  HTF_EVENT_METRIC = 21,
  /** Event record identifier for the ParameterString event. */
  HTF_EVENT_PARAMETER_STRING = 22,
  /** Event record identifier for the ParameterInt event. */
  HTF_EVENT_PARAMETER_INT = 23,
  /** Event record identifier for the ParameterUnsignedInt event. */
  HTF_EVENT_PARAMETER_UNSIGNED_INT = 24,
  /** Event record identifier for the ThreadFork event. */
  HTF_EVENT_THREAD_FORK = 25,
  /** Event record identifier for the ThreadJoin event. */
  HTF_EVENT_THREAD_JOIN = 26,
  /** Event record identifier for the ThreadTeamBegin event. */
  HTF_EVENT_THREAD_TEAM_BEGIN = 27,
  /** Event record identifier for the ThreadTeamEnd event. */
  HTF_EVENT_THREAD_TEAM_END = 28,
  /** Event record identifier for the ThreadAcquireLock event. */
  HTF_EVENT_THREAD_ACQUIRE_LOCK = 29,
  /** Event record identifier for the ThreadReleaseLock event. */
  HTF_EVENT_THREAD_RELEASE_LOCK = 30,
  /** Event record identifier for the ThreadTaskCreate event. */
  HTF_EVENT_THREAD_TASK_CREATE = 31,
  /** Event record identifier for the ThreadTaskSwitch event. */
  HTF_EVENT_THREAD_TASK_SWITCH = 32,
  /** Event record identifier for the ThreadTaskComplete event. */
  HTF_EVENT_THREAD_TASK_COMPLETE = 33,
  /** Event record identifier for the ThreadCreate event. */
  HTF_EVENT_THREAD_CREATE = 34,
  /** Event record identifier for the ThreadBegin event. */
  HTF_EVENT_THREAD_BEGIN = 35,
  /** Event record identifier for the ThreadWait event. */
  HTF_EVENT_THREAD_WAIT = 36,
  /** Event record identifier for the ThreadEnd event. */
  HTF_EVENT_THREAD_END = 37,
  /** Event record identifier for the IoCreateHandle event. */
  HTF_EVENT_IO_CREATE_HANDLE = 38,
  /** Event record identifier for the IoDestroyHandle event. */
  HTF_EVENT_IO_DESTROY_HANDLE = 39,
  /** Event record identifier for the IoSeek event. */
  HTF_EVENT_IO_SEEK = 41,
  /** Event record identifier for the IoChangeStatusFlags event. */
  HTF_EVENT_IO_CHANGE_STATUS_FLAGS = 42,
  /** Event record identifier for the IoDeleteFile event. */
  HTF_EVENT_IO_DELETE_FILE = 43,
  /** Event record identifier for the IoOperationBegin event. */
  HTF_EVENT_IO_OPERATION_BEGIN = 44,
  /** Event record identifier for the IoDuplicateHandle event. */
  HTF_EVENT_IO_DUPLICATE_HANDLE = 40,
  /** Event record identifier for the IoOperationTest event. */
  HTF_EVENT_IO_OPERATION_TEST = 45,
  /** Event record identifier for the IoOperationIssued event. */
  HTF_EVENT_IO_OPERATION_ISSUED = 46,
  /** Event record identifier for the IoOperationComplete event. */
  HTF_EVENT_IO_OPERATION_COMPLETE = 47,
  /** Event record identifier for the IoOperationCancelled event. */
  HTF_EVENT_IO_OPERATION_CANCELLED = 48,
  /** Event record identifier for the IoAcquireLock event. */
  HTF_EVENT_IO_ACQUIRE_LOCK = 49,
  /** Event record identifier for the IoReleaseLock event. */
  HTF_EVENT_IO_RELEASE_LOCK = 50,
  /** Event record identifier for the IoTryLock event. */
  HTF_EVENT_IO_TRY_LOCK = 51,
  /** Event record identifier for the ProgramBegin event. */
  HTF_EVENT_PROGRAM_BEGIN = 52,
  /** Event record identifier for the ProgramEnd event. */
  HTF_EVENT_PROGRAM_END = 53,
  /** Event record identifier for the NonBlockingCollectiveRequest event. */
  HTF_EVENT_NON_BLOCKING_COLLECTIVE_REQUEST = 54,
  /** Event record identifier for the NonBlockingCollectiveComplete event. */
  HTF_EVENT_NON_BLOCKING_COLLECTIVE_COMPLETE = 55,
  /** Event record identifier for the CommCreate event. */
  HTF_EVENT_COMM_CREATE = 56,
  /** Event record identifier for the CommDestroy event. */
  HTF_EVENT_COMM_DESTROY = 57,

  HTF_EVENT_MAX_ID
};

/**
 * Structure to store an event in HTF
 *  - uint8_t event_size: the size of the event
 *  - enum Record record: the ID of the event recorded in the above enumeration of events
 *  - uint8_t event_data[256]: data related to the events (parameters of functions etc)
 */
typedef struct Event {
  enum Record record;
  uint8_t event_size;
  uint8_t event_data[256];  // todo: align on 256
} __attribute__((packed)) Event;

/*************************** Sequences **********************/
#ifdef __cplusplus
struct TokenCountMap : public std::map<Token, size_t> {
  void operator+=(const TokenCountMap& other) {
    for (auto keyValue : other) {
      if (this->count(keyValue.first) == 0) {
        this->insert(keyValue);
      } else {
        this->at(keyValue.first) += keyValue.second;
      }
    }
  }
  TokenCountMap operator*(size_t multiplier) const {
    auto otherMap = TokenCountMap();
    for (auto keyValue : otherMap) {
      otherMap[keyValue.first] = keyValue.second * multiplier;
    }
    return otherMap;
  }
};
#endif
/**
 * Structure to store a sequence in HTF format
 *  - LinkedVector* durations: array of durations for these types of sequences. (see htf_timestamp.h)
 *  - uint_32_t hash: Hash value according to the hash32 function. (see htf_hash.h)
 *  - std::vector* tokens: Array to store the sequence of tokens
 */
typedef struct Sequence {
  LinkedVector* durations CXX({new LinkedVector()});
  uint32_t hash CXX({0});
#ifdef __cplusplus
  std::vector<Token> tokens;
  [[nodiscard]] size_t size() const { return tokens.size(); }
  const TokenCountMap& getTokenCount(const struct Thread* thread);

 private:
  TokenCountMap tokenCount;
#endif
} Sequence;

/*************************** Loop **********************/

/**
 * Structure to store a loop in HTF format
 * // TODO Update this doc
 * - unsigned* nb_iterations:  Number of iterations for these loops
 * - unsigned nb_loops: Number of registered loops
 * - unsigned nb_allocated: Number of allocated loops
 * - htf_token_t repeated_token: Token of the sequence being repeated
 * - struct Token id: Self-id of the loop
 */
typedef struct Loop {
  Token repeated_token;
  Token self_id;
  CXX(std::vector<uint> nb_iterations;)
  CXX(void addIteration();)
} Loop;

/**
 * Summary for an event.
 */
typedef struct EventSummary {
  TokenId id;
  Event event;
  LinkedVector* durations CXX({new LinkedVector()});
  // TODO use NB_TIMESTAMP_DEFAULT as the default size for that
  size_t nb_occurences;

  uint8_t* attribute_buffer;
  size_t attribute_buffer_size;
  size_t attribute_pos;
} EventSummary;

/**
 * Define a thread structure for HTF format
 */
typedef uint32_t ThreadId;
#define HTF_THREAD_ID_INVALID ((HTF(ThreadId))HTF_UNDEFINED_UINT32)

typedef uint32_t LocationGroupId;
#define HTF_LOCATION_GROUP_ID_INVALID ((HTF(LocationGroupId))HTF_UNDEFINED_UINT32)
#define HTF_MAIN_LOCATION_GROUP_ID ((HTF(LocationGroupId))HTF_LOCATION_GROUP_ID_INVALID - 1)

/** A reference for everything after that. */
typedef uint32_t Ref;

#define HTF_UNDEFINED_UINT8 ((uint8_t)(~((uint8_t)0u)))
#define HTF_UNDEFINED_INT8 ((int8_t)(~(HTF_UNDEFINED_UINT8 >> 1)))
#define HTF_UNDEFINED_UINT16 ((uint16_t)(~((uint16_t)0u)))
#define HTF_UNDEFINED_INT16 ((int16_t)(~(HTF_UNDEFINED_UINT16 >> 1)))
#define HTF_UNDEFINED_UINT32 ((uint32_t)(~((uint32_t)0u)))
#define HTF_UNDEFINED_INT32 ((int32_t)(~(HTF_UNDEFINED_UINT32 >> 1)))
#define HTF_UNDEFINED_UINT64 ((uint64_t)(~((uint64_t)0u)))
#define HTF_UNDEFINED_INT64 ((int64_t)(~(HTF_UNDEFINED_UINT64 >> 1)))
#define HTF_UNDEFINED_TYPE HTF_UNDEFINED_UINT8

/**
 * Define a string reference structure used by HTF format
 * It has an ID and an associated char* with its length
 */
typedef Ref StringRef;
#define HTF_STRING_REF_INVALID ((StringRef)HTF_UNDEFINED_UINT32)

typedef struct String {
  StringRef string_ref;
  char* str;
  int length;
} String;

/**
 * Define a region that has an ID and a htf_string_ref_t description
 */
typedef Ref RegionRef;
#define HTF_REGIONREF_INVALID ((RegionRef)HTF_UNDEFINED_UINT32)

typedef struct Region {
  RegionRef region_ref;
  StringRef string_ref;
  /* TODO: add other information (eg. file, line number, etc.)  */
} Region;

typedef Ref AttributeRef;

/** @brief Wrapper for enum @eref{AttributeType}. */
typedef uint8_t htf_type_t;

typedef struct Attribute {
  AttributeRef attribute_ref;
  StringRef name;
  StringRef description;
  htf_type_t type;
} Attribute;

/**
 * A thread contains streams of events.
 * It can be a regular thread (eg. a pthread), or a GPU stream
 */
typedef struct Thread {
  struct Archive* archive;
  ThreadId id;

  EventSummary* events;
  unsigned nb_allocated_events;
  unsigned nb_events;

  Sequence** sequences;
  unsigned nb_allocated_sequences;
  unsigned nb_sequences;

  Loop* loops;
  unsigned nb_allocated_loops;
  unsigned nb_loops;
#ifdef __cplusplus
  TokenId getEventId(Event* e);
  [[nodiscard]] Event* getEvent(Token) const;
  [[nodiscard]] EventSummary* getEventSummary(Token) const;
  [[nodiscard]] Sequence* getSequence(Token) const;
  [[nodiscard]] Loop* getLoop(Token) const;
  /* Returns the n-th token in the given Sequence/Loop. */
  [[nodiscard]] Token& getToken(Token, int) const;

  void printToken(Token) const;
  void printTokenArray(Token* array, size_t start_index, size_t len) const;
  void printTokenVector(const std::vector<Token>&) const;
  void printSequence(Token) const;
  void printEvent(Event*) const;
  void printAttribute(AttributeRef) const;
  void printString(StringRef) const;
  void printAttributeRef(AttributeRef) const;
  void printLocation(Ref) const;
  void printRegion(RegionRef) const;
  void printAttributeValue(struct AttributeData* attr, htf_type_t type) const;
  void printAttribute(struct AttributeData* attr) const;
  void printAttributeList(struct AttributeList* attribute_list);
  void printEventAttribute(struct EventOccurence* es);
  [[nodiscard]] const char* getName() const;
  /* Search for a sequence_id that matches the given sequence.
   * If none of the registered sequence match, register a new Sequence.
   * TODO Speed this up using hash map and/or storing the sequence's id in the structure.
   */
  Token getSequenceId(Sequence* sequence);
  /* Search for a sequence_id that matches the given array as a Sequence.
   * If none of the registered sequence match, register a new Sequence.
   * TODO Speed this up using hashmap
   */
  Token getSequenceIdFromArray(Token* token_array, size_t array_len);
  /* Returns the duration for the given array. */
  htf_timestamp_t getSequenceDuration(Token* array, size_t size);
  void finalizeThread();
  /* Create a new Thread from an archive and an id. This is used when writing the trace. */
  Thread(Archive* a, ThreadId id);
  /* Create a blank new Thread. This is used when reading the trace. */
  Thread() = default;
#endif
} Thread;

CXX(
};) /* namespace htf */
#ifdef __cplusplus
extern "C" {
#endif
  /*************************** C Functions **********************/
  /* Allocates a new thread */
  extern HTF(Thread) * htf_thread_new(void);
  /**
   * Return the thread name of thread thread
   */
  extern const char* htf_thread_get_name(HTF(Thread) * thread);

  /**
   * Print the content of sequence seq_id
   */
  extern void htf_print_sequence(HTF(Thread) * thread, HTF(Token) seq_id);

  /**
   * Print the subset of a repeated_token array
   */
  extern void htf_print_token_array(HTF(Thread) * thread, HTF(Token) * token_array, int index_start, int index_stop);

  /**
   * Print a repeated_token
   */
  extern void htf_print_token(HTF(Thread) * thread, HTF(Token) token);

  /**
   * Print an event
   */
  extern void htf_print_event(HTF(Thread) * thread, HTF(Event) * e);

  /**
   * Return the loop whose id is loop_id
   *  - return NULL if loop_id is unknown
   */
  extern struct HTF(Loop) * htf_get_loop(HTF(Thread) * thread_trace, HTF(Token) loop_id);

  /*
   * Return the sequence whose id is sequence_id
   *  - return NULL if sequence_id is unknown
   */
  extern struct HTF(Sequence) * htf_get_sequence(HTF(Thread) * thread_trace, HTF(Token) seq_id);

  /**
   * Return the event whose id is event_id
   *  - return NULL if event_id is unknown
   */
  extern struct HTF(Event) * htf_get_event(HTF(Thread) * thread_trace, HTF(Token) evt_id);

  /**
   * Get the nth token of a given Sequence.
   */
  extern HTF(Token) htf_get_token(HTF(Thread) * trace, HTF(Token) sequence, int index);
  // Says here that we shouldn't send a htf::Token, but that's because it doesn't know
  // We made the htf_token type that matches it. This works as long as the C++ and C version
  // of the struct both have the same elements. Don't care about the rest.

  /* Returns the size of the given sequence. */
  extern size_t htf_sequence_get_size(HTF(Sequence) * sequence);
  /* Returns the nth token of the given sequence. */
  extern HTF(Token) htf_sequence_get_token(HTF(Sequence) * sequence, int index);
  /* Returns the number of similar loops. */
  extern size_t htf_loop_count(HTF(Loop) * loop);
  /* Returns the number of loops for the nth loop. */
  extern size_t htf_loop_get_count(HTF(Loop) * loop, size_t index);
  extern HTF(Token) * htf_loop_get_data();
  extern HTF(Token) * htf_sequence_get_data();

#ifdef __cplusplus
};
#endif
/**
 * Given a buffer, a counter that indicates the number of object it holds, and this object's datatype,
 * doubles the size of the buffer using realloc, or if it fails, malloc and memmove then frees the old buffer.
 * This is better than a realloc because it moves the data around, but it is also slower.
 * Checks for error at malloc.
 */
#ifdef __cplusplus
#define DOUBLE_MEMORY_SPACE(buffer, counter, datatype)           \
  do {                                                           \
    auto new_buffer = new datatype[2 * counter];                 \
    if (new_buffer == nullptr) {                                 \
      htf_error("Failed to allocate memory\n");                  \
    }                                                            \
    std::memcpy(new_buffer, buffer, counter * sizeof(datatype)); \
    delete buffer;                                               \
    buffer = new_buffer;                                         \
    counter *= 2;                                                \
  } while (0)
#else
#define DOUBLE_MEMORY_SPACE(buffer, counter, datatype)                        \
  do {                                                                        \
    datatype* new_buffer = realloc(buffer, (counter * 2) * sizeof(datatype)); \
    if (new_buffer == NULL) {                                                 \
      new_buffer = malloc((counter * 2) * sizeof(datatype));                  \
      if (new_buffer == NULL) {                                               \
        htf_error("Failed to allocate memory using realloc AND malloc\n");    \
      }                                                                       \
      memmove(new_buffer, buffer, counter * sizeof(datatype));                \
      free(buffer);                                                           \
    }                                                                         \
    buffer = new_buffer;                                                      \
    counter *= 2;                                                             \
  } while (0)
#endif
/**
 * Given a buffer, a counter that indicates the number of object it holds, and this object's datatype,
 * Increments the size of the buffer by 1 using realloc, or if it fails, malloc and memmove then frees the old buffer.
 * This is better than a realloc because it moves the data around, but it is also slower.
 * Checks for error at malloc.
 */
#ifdef __cplusplus
#define INCREMENT_MEMORY_SPACE(buffer, counter, datatype)        \
  do {                                                           \
    auto new_buffer = new datatype[counter + 1];                 \
    if (new_buffer == nullptr) {                                 \
      htf_error("Failed to allocate memory\n");                  \
    }                                                            \
    std::memcpy(new_buffer, buffer, counter * sizeof(datatype)); \
    delete buffer;                                               \
    buffer = new_buffer;                                         \
    counter++;                                                   \
  } while (0)
#else
#define INCREMENT_MEMORY_SPACE(buffer, counter, datatype)                     \
  do {                                                                        \
    datatype* new_buffer = realloc(buffer, (counter + 1) * sizeof(datatype)); \
    if (new_buffer == NULL) {                                                 \
      new_buffer = malloc((counter + 1) * sizeof(datatype));                  \
      if (new_buffer == NULL) {                                               \
        htf_error("Failed to allocate memory using realloc AND malloc\n");    \
      }                                                                       \
      memmove(new_buffer, buffer, counter * sizeof(datatype));                \
      free(buffer);                                                           \
    }                                                                         \
    buffer = new_buffer;                                                      \
    counter++;                                                                \
  } while (0)
#endif
/**
 * Primitive for DOFOR loops
 */
#define DOFOR(var_name, max) for (int var_name = 0; var_name < max; var_name++)

#define NB_EVENT_DEFAULT 1000
#define NB_SEQUENCE_DEFAULT 1000
#define NB_LOOP_DEFAULT 1000
#define NB_STRING_DEFAULT 100
#define NB_REGION_DEFAULT 100
#define NB_TIMESTAMP_DEFAULT 1000
#define NB_ATTRIBUTE_DEFAULT 1000
#define SEQUENCE_SIZE_DEFAULT 1024
#define LOOP_SIZE_DEFAULT 16
#define CALLSTACK_DEPTH_DEFAULT 128
#define NB_ARCHIVES_DEFAULT 1
#define NB_THREADS_DEFAULT 16
#define NB_LOCATION_GROUPS_DEFAULT 16
#define NB_LOCATIONS_DEFAULT NB_THREADS_DEFAULT

/* -*-
   mode: c++;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
