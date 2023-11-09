/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
/** @file
 * The main file of HTF. Here are defined the most basic elements of the trace
 * (Tokens, Events, Sequences and Loops), as well as Threads, representing an execution stream.
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

/**
 * A simple alias to make some code clearer. We use uint8 because they're the size of a byte.
 */
typedef uint8_t byte;

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
  HTF_EVENT_BUFFER_FLUSH = 0,                      /**< Event record identifier for the BufferFlush event. */
  HTF_EVENT_MEASUREMENT_ON_OFF = 1,                /**< Event record identifier for the MeasurementOnOff event. */
  HTF_EVENT_ENTER = 2,                             /**< Event record identifier for the Enter event. */
  HTF_EVENT_LEAVE = 3,                             /**< Event record identifier for the Leave event. */
  HTF_EVENT_MPI_SEND = 4,                          /**< Event record identifier for the MpiSend event. */
  HTF_EVENT_MPI_ISEND = 5,                         /**< Event record identifier for the MpiIsend event. */
  HTF_EVENT_MPI_ISEND_COMPLETE = 6,                /**< Event record identifier for the MpiIsendComplete event. */
  HTF_EVENT_MPI_IRECV_REQUEST = 7,                 /**< Event record identifier for the MpiIrecvRequest event. */
  HTF_EVENT_MPI_RECV = 8,                          /**< Event record identifier for the MpiRecv event. */
  HTF_EVENT_MPI_IRECV = 9,                         /**< Event record identifier for the MpiIrecv event. */
  HTF_EVENT_MPI_REQUEST_TEST = 10,                 /**< Event record identifier for the MpiRequestTest event. */
  HTF_EVENT_MPI_REQUEST_CANCELLED = 11,            /**< Event record identifier for the MpiRequestCancelled event. */
  HTF_EVENT_MPI_COLLECTIVE_BEGIN = 12,             /**< Event record identifier for the MpiCollectiveBegin event. */
  HTF_EVENT_MPI_COLLECTIVE_END = 13,               /**< Event record identifier for the MpiCollectiveEnd event. */
  HTF_EVENT_OMP_FORK = 14,                         /**< Event record identifier for the OmpFork event. */
  HTF_EVENT_OMP_JOIN = 15,                         /**< Event record identifier for the OmpJoin event. */
  HTF_EVENT_OMP_ACQUIRE_LOCK = 16,                 /**< Event record identifier for the OmpAcquireLock event. */
  HTF_EVENT_OMP_RELEASE_LOCK = 17,                 /**< Event record identifier for the OmpReleaseLock event. */
  HTF_EVENT_OMP_TASK_CREATE = 18,                  /**< Event record identifier for the OmpTaskCreate event. */
  HTF_EVENT_OMP_TASK_SWITCH = 19,                  /**< Event record identifier for the OmpTaskSwitch event. */
  HTF_EVENT_OMP_TASK_COMPLETE = 20,                /**< Event record identifier for the OmpTaskComplete event. */
  HTF_EVENT_METRIC = 21,                           /**< Event record identifier for the Metric event. */
  HTF_EVENT_PARAMETER_STRING = 22,                 /**< Event record identifier for the ParameterString event. */
  HTF_EVENT_PARAMETER_INT = 23,                    /**< Event record identifier for the ParameterInt event. */
  HTF_EVENT_PARAMETER_UNSIGNED_INT = 24,           /**< Event record identifier for the ParameterUnsignedInt event. */
  HTF_EVENT_THREAD_FORK = 25,                      /**< Event record identifier for the ThreadFork event. */
  HTF_EVENT_THREAD_JOIN = 26,                      /**< Event record identifier for the ThreadJoin event. */
  HTF_EVENT_THREAD_TEAM_BEGIN = 27,                /**< Event record identifier for the ThreadTeamBegin event. */
  HTF_EVENT_THREAD_TEAM_END = 28,                  /**< Event record identifier for the ThreadTeamEnd event. */
  HTF_EVENT_THREAD_ACQUIRE_LOCK = 29,              /**< Event record identifier for the ThreadAcquireLock event. */
  HTF_EVENT_THREAD_RELEASE_LOCK = 30,              /**< Event record identifier for the ThreadReleaseLock event. */
  HTF_EVENT_THREAD_TASK_CREATE = 31,               /**< Event record identifier for the ThreadTaskCreate event. */
  HTF_EVENT_THREAD_TASK_SWITCH = 32,               /**< Event record identifier for the ThreadTaskSwitch event. */
  HTF_EVENT_THREAD_TASK_COMPLETE = 33,             /**< Event record identifier for the ThreadTaskComplete event. */
  HTF_EVENT_THREAD_CREATE = 34,                    /**< Event record identifier for the ThreadCreate event. */
  HTF_EVENT_THREAD_BEGIN = 35,                     /**< Event record identifier for the ThreadBegin event. */
  HTF_EVENT_THREAD_WAIT = 36,                      /**< Event record identifier for the ThreadWait event. */
  HTF_EVENT_THREAD_END = 37,                       /**< Event record identifier for the ThreadEnd event. */
  HTF_EVENT_IO_CREATE_HANDLE = 38,                 /**< Event record identifier for the IoCreateHandle event. */
  HTF_EVENT_IO_DESTROY_HANDLE = 39,                /**< Event record identifier for the IoDestroyHandle event. */
  HTF_EVENT_IO_SEEK = 41,                          /**< Event record identifier for the IoSeek event. */
  HTF_EVENT_IO_CHANGE_STATUS_FLAGS = 42,           /**< Event record identifier for the IoChangeStatusFlags event. */
  HTF_EVENT_IO_DELETE_FILE = 43,                   /**< Event record identifier for the IoDeleteFile event. */
  HTF_EVENT_IO_OPERATION_BEGIN = 44,               /**< Event record identifier for the IoOperationBegin event. */
  HTF_EVENT_IO_DUPLICATE_HANDLE = 40,              /**< Event record identifier for the IoDuplicateHandle event. */
  HTF_EVENT_IO_OPERATION_TEST = 45,                /**< Event record identifier for the IoOperationTest event. */
  HTF_EVENT_IO_OPERATION_ISSUED = 46,              /**< Event record identifier for the IoOperationIssued event. */
  HTF_EVENT_IO_OPERATION_COMPLETE = 47,            /**< Event record identifier for the IoOperationComplete event. */
  HTF_EVENT_IO_OPERATION_CANCELLED = 48,           /**< Event record identifier for the IoOperationCancelled event. */
  HTF_EVENT_IO_ACQUIRE_LOCK = 49,                  /**< Event record identifier for the IoAcquireLock event. */
  HTF_EVENT_IO_RELEASE_LOCK = 50,                  /**< Event record identifier for the IoReleaseLock event. */
  HTF_EVENT_IO_TRY_LOCK = 51,                      /**< Event record identifier for the IoTryLock event. */
  HTF_EVENT_PROGRAM_BEGIN = 52,                    /**< Event record identifier for the ProgramBegin event. */
  HTF_EVENT_PROGRAM_END = 53,                      /**< Event record identifier for the ProgramEnd event. */
  HTF_EVENT_NON_BLOCKING_COLLECTIVE_REQUEST = 54,  /**< Event record identifier for the NonBlockingCollectiveRequest
                                                    * event. */
  HTF_EVENT_NON_BLOCKING_COLLECTIVE_COMPLETE = 55, /**< Event record identifier for the NonBlockingCollectiveComplete
                                                    * event. */
  HTF_EVENT_COMM_CREATE = 56,                      /**< Event record identifier for the CommCreate event. */
  HTF_EVENT_COMM_DESTROY = 57,                     /**< Event record identifier for the CommDestroy event. */

  HTF_EVENT_MAX_ID /**< Max Event Record ID */
};

/**
 * @brief Structure to store an event in HTF.
 */
typedef struct Event {
  enum Record record;      /**< ID of the event recorded in the above enumeration of events. */
  uint8_t event_size;      /**< Size of the event. */
  uint8_t event_data[256]; /**< data related to the events. (parameters of functions etc)*/
                           // todo: align on 256
} __attribute__((packed)) Event;

/*************************** Sequences **********************/
#ifdef __cplusplus
/**
 * @brief A Map for counting Tokens.
 *
 * This class exists to make token counting easier.
 * Instead of having arrays for each different type of token, we can just use that map.
 * This class also comes with addition and multiplication, so that we can easily use them.
 */
struct TokenCountMap : public std::map<Token, size_t> {
  /** Adds each (key, value) pair of the other map to this one. */
  void operator+=(const TokenCountMap& other) {
    for (auto keyValue : other) {
      if (this->count(keyValue.first) == 0) {
        this->insert(keyValue);
      } else {
        this->at(keyValue.first) += keyValue.second;
      }
    }
  }
  /** Returns a new map with the same keys, but each value has been multiplied by the given value. */
  TokenCountMap operator*(size_t multiplier) const {
    auto otherMap = TokenCountMap();
    for (auto keyValue : otherMap) {
      otherMap[keyValue.first] = keyValue.second * multiplier;
    }
    return otherMap;
  }
};
#endif
/** Defines a TokenCountMap. In C, defines a char[] of size sizeof(TokenCountMap). */
#define DEFINE_TokenCountMap(name) C_CXX(byte, TokenCountMap) name C([MAP_SIZE])
/** Defines a C++ vector. In C, defines a char[] of size sizeof(std::vector). */
#define DEFINE_Vector(type, name) C_CXX(byte, std::vector<type>) name C_CXX([VECTOR_SIZE], { std::vector<type>() })

/**
 * @brief Structure to store a sequence in HTF format.
 */
typedef struct Sequence {
  LinkedVector* durations CXX({new LinkedVector()}); /**< Vector of durations for these type of sequences. */
  uint32_t hash CXX({0});                            /**< Hash value according to the hash32 function.*/
  DEFINE_Vector(Token, tokens);                      /**< Vector of Token to store the sequence of tokens */
  CXX(private:)
  /**
   * A TokenCountMap counting each token in this Sequence (recursively).
   * It might not be initialized, which is why ::getTokenCount exists.*/
  DEFINE_TokenCountMap(tokenCount);
#ifdef __cplusplus
 public:
  /** Getter for the size of that Sequence.
   * @returns Number of tokens in that Sequence. */
  [[nodiscard]] size_t size() const { return tokens.size(); }
  /** Getter for #tokenCountMap.
   * If need be, counts the number of Token in that Sequence to initialize it.
   * @returns Reference to #tokenCountMap.*/
  const TokenCountMap& getTokenCount(const struct Thread* thread);
#endif
} Sequence;

/*************************** Loop **********************/

/**
 * @brief Structure to store a Loop in HTF format.
 */
typedef struct Loop {
  Token repeated_token;                   /**< Token of the Sequence being repeated. */
  Token self_id;                          /**< Token identifying that Loop. */
  DEFINE_Vector(uint16_t, nb_iterations); /**< Vector of uint counting the number of iterations of that loop. */
  CXX(void addIteration();)               /**< Adds an iteration to the lastest occurence of that loop. */
} Loop;

/**
 * @brief Summary for an htf::Event.
 *
 * Contains the durations for each occurence of that event
 * as well as the number of occurences for that event,
 * and its attributes.
 */
typedef struct EventSummary {
  TokenId id;                                        /**< ID of the Event */
  Event event;                                       /**< The Event being summarized.*/
  LinkedVector* durations CXX({new LinkedVector()}); /**< Durations for each occurrence of that Event.*/
  size_t nb_occurences;                              /**< Number of times that Event has happened. */

  uint8_t* attribute_buffer;    /**< Storage for Attribute.*/
  size_t attribute_buffer_size; /**< Size of #attribute_buffer.*/
  size_t attribute_pos;         /**< Position of #attribute_buffer.*/
} EventSummary;

typedef uint32_t ThreadId;                                          /**< Reference for a htf::Thread. */
#define HTF_THREAD_ID_INVALID ((HTF(ThreadId))HTF_UNDEFINED_UINT32) /**< Invalid ThreadId. */
typedef uint32_t LocationGroupId;                                   /**< Reference for a htf::LocationGroup. */
#define HTF_LOCATION_GROUP_ID_INVALID ((HTF(LocationGroupId))HTF_UNDEFINED_UINT32) /**< Invalid LocationGroupId. */
#define HTF_MAIN_LOCATION_GROUP_ID ((HTF(LocationGroupId))HTF_LOCATION_GROUP_ID_INVALID - 1)
/**< Main LocationGroupId \todo What is that ?*/

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

typedef Ref StringRef;                                           /**< Reference for a htf::String */
#define HTF_STRING_REF_INVALID ((StringRef)HTF_UNDEFINED_UINT32) /**< Invalid StringRef */
/**
 * @brief Define a String reference structure used by HTF format
 *
 * It has an ID and an associated char* with its length
 */
typedef struct String {
  StringRef string_ref; /** Id of that String.*/
  char* str;            /** Actual C String */
  int length;           /** Length of #str.*/
} String;

typedef Ref RegionRef;                                          /**< Reference for a htf::Region */
#define HTF_REGIONREF_INVALID ((RegionRef)HTF_UNDEFINED_UINT32) /**< Invalid RegionRef */
/**
 * @brief Define a Region that has an ID and a description.
 */
typedef struct Region {
  RegionRef region_ref; /**< Id of that Region. */
  StringRef string_ref; /**< Description of that Region. */
  /* TODO: add other information (eg. file, line number, etc.)  */
} Region;

/** Reference for an htf::Attribute. */
typedef Ref AttributeRef;

/** Wrapper for enum htf::AttributeType. */
typedef uint8_t htf_type_t;

/**
 * @brief Define an Attribute of a function call.
 */
typedef struct Attribute {
  AttributeRef attribute_ref; /**< Id of that Attribute. */
  StringRef name;             /**< Name of that Attribute. */
  StringRef description;      /**< Description of that Attribute. */
  htf_type_t type;            /**< Type of that Attribute. */
} Attribute;

/**
 * @brief A thread contains streams of events.
 *
 * It can be a regular thread (eg. a pthread), or a GPU stream.
 */
typedef struct Thread {
  struct Archive* archive; /**< htf::Archive containing this Thread. */
  ThreadId id;             /**< Id of this Thread. */

  EventSummary* events;         /**< Array of events recorded in this Thread. */
  unsigned nb_allocated_events; /**< Size of #events. */
  unsigned nb_events;           /**< Number of htf::EventSummary in #events. */

  Sequence** sequences;            /**< Array of htf::Sequence recorded in this Thread. */
  unsigned nb_allocated_sequences; /**< Size of #sequences. */
  unsigned nb_sequences;           /**< Number of htf::Sequence in #sequences. */

  Loop* loops;                 /**< Array of htf::Loop recorded in this Thread. */
  unsigned nb_allocated_loops; /**< Size of #loops. */
  unsigned nb_loops;           /**< Number of htf::Loop in #loops. */
#ifdef __cplusplus
  TokenId getEventId(Event* e);
  [[nodiscard]] Event* getEvent(Token) const;
  [[nodiscard]] EventSummary* getEventSummary(Token) const;
  [[nodiscard]] Sequence* getSequence(Token) const;
  [[nodiscard]] Loop* getLoop(Token) const;
  /** Returns the n-th token in the given Sequence/Loop. */
  [[nodiscard]] Token& getToken(Token, int) const;

  void printToken(Token) const;
  void printTokenArray(const Token* array, size_t start_index, size_t len) const;
  void printTokenVector(const std::vector<Token>&) const;
  void printSequence(Token) const;
  void printEvent(Event*) const;
  void printAttribute(AttributeRef) const;
  void printString(StringRef) const;
  void printAttributeRef(AttributeRef) const;
  void printLocation(Ref) const;
  void printRegion(RegionRef) const;
  void printAttributeValue(const struct AttributeData* attr, htf_type_t type) const;
  void printAttribute(const struct AttributeData* attr) const;
  void printAttributeList(const struct AttributeList* attribute_list) const;
  void printEventAttribute(const struct EventOccurence* es) const;
  [[nodiscard]] const char* getName() const;
  /** Search for a sequence_id that matches the given sequence.
   * If none of the registered sequence match, register a new Sequence.
   * \todo Speed this up using hash map and/or storing the sequence's id in the structure.
   */
  Token getSequenceId(Sequence* sequence);
  /** Search for a sequence_id that matches the given array as a Sequence.
   * If none of the registered sequence match, register a new Sequence.
   * \todo Speed this up using hashmap
   */
  Token getSequenceIdFromArray(Token* token_array, size_t array_len);
  /** Returns the duration for the given array. */
  htf_timestamp_t getSequenceDuration(Token* array, size_t size);
  void finalizeThread();
  /** Create a new Thread from an archive and an id. This is used when writing the trace. */
  Thread(Archive* a, ThreadId id);
  /** Create a blank new Thread. This is used when reading the trace. */
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
