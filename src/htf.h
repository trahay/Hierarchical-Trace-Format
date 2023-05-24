#ifndef HTF_H
#define HTF_H

#include <pthread.h>
#include <string.h>

#include "htf.h"
#include "htf_dbg.h"
#include "htf_dynamic_array.h"
#include "htf_timestamp.h"

/* A token is either:
	 - an event
	 - a sequence (ie a list of tokens)
	 - a loop (a repetition of sequences)
*/

/* Token types */
typedef enum htf_token_type {
	HTF_TYPE_INVALID  = 0,
  HTF_TYPE_EVENT    = 1,
  HTF_TYPE_SEQUENCE = 2,
  HTF_TYPE_LOOP     = 3
} htf_token_type_t;

#define HTF_TOKEN_TYPE_C(t) (HTF_TOKEN_TYPE(t))==HTF_TYPE_INVALID? 'I':	\
    (HTF_TOKEN_TYPE(t))==HTF_TYPE_EVENT? 'E':				\
    (HTF_TOKEN_TYPE(t))==HTF_TYPE_SEQUENCE? 'S':			\
    (HTF_TOKEN_TYPE(t))==HTF_TYPE_LOOP? 'L':'U'

typedef uint32_t htf_token_id_t;

typedef struct htf_token {
  enum htf_token_type type : 2;
  htf_token_id_t id : 30;
} htf_token_t;


typedef struct htf_event_id {
  htf_token_id_t id : 30;
} htf_event_id_t;

typedef struct htf_sequence_id {
  htf_token_id_t id : 30;
} htf_sequence_id_t;

typedef struct htf_loop_id {
  htf_token_id_t id : 30;
} htf_loop_id_t;

#define HTF_TOKEN_ID_INVALID    0x3fffffff
#define HTF_EVENT_ID_INVALID    HTF_TOKEN_ID_INVALID
#define HTF_SEQUENCE_ID_INVALID HTF_TOKEN_ID_INVALID
#define HTF_LOOP_ID_INVALID     HTF_TOKEN_ID_INVALID

/* convert an id to an integer */
#define HTF_ID(_id) ((_id).id)

/* return the type/id of a token */
#define HTF_TOKEN_TYPE(t) ( (t).type)
#define HTF_TOKEN_ID(t) ( (t).id)

/* build a token */
static inline htf_token_t HTF_TOKENIZE(htf_token_type_t t, htf_token_id_t id) { return (htf_token_t) {.type=t, .id=id};} 

/* convert an index to an event_id_t */
static inline htf_event_id_t HTF_EVENT_ID(int index) { return (htf_event_id_t) {.id=index};}
static inline htf_sequence_id_t HTF_SEQUENCE_ID(int index) {return (htf_sequence_id_t) {.id=index};}
static inline htf_loop_id_t HTF_LOOP_ID(int index) { return (htf_loop_id_t) {.id=index};}

/* convert a token_t to an event_id_t */
static inline htf_event_id_t HTF_TOKEN_TO_EVENT_ID(htf_token_t t) { return (htf_event_id_t) {.id=HTF_TOKEN_ID(t)};}
static inline htf_sequence_id_t HTF_TOKEN_TO_SEQUENCE_ID(htf_token_t t) { return (htf_sequence_id_t) {.id=HTF_TOKEN_ID(t)};}
static inline htf_loop_id_t HTF_TOKEN_TO_LOOP_ID(htf_token_t t) { return (htf_loop_id_t) {.id=HTF_TOKEN_ID(t)};}


/*************************** Events **********************/
enum htf_event_type {
   htf_block_start,
   htf_block_end,
   htf_singleton,
};

enum htf_record {
    HTF_EVENT_BUFFER_FLUSH                     = 0, /** Event record identifier for the BufferFlush event. */
    HTF_EVENT_MEASUREMENT_ON_OFF               = 1, /** Event record identifier for the MeasurementOnOff event. */
    HTF_EVENT_ENTER                            = 2, /** Event record identifier for the Enter event. */
    HTF_EVENT_LEAVE                            = 3, /** Event record identifier for the Leave event. */
    HTF_EVENT_MPI_SEND                         = 4, /** Event record identifier for the MpiSend event. */
    HTF_EVENT_MPI_ISEND                        = 5, /** Event record identifier for the MpiIsend event. */
    HTF_EVENT_MPI_ISEND_COMPLETE               = 6, /** Event record identifier for the MpiIsendComplete event. */
    HTF_EVENT_MPI_IRECV_REQUEST                = 7, /** Event record identifier for the MpiIrecvRequest event. */
    HTF_EVENT_MPI_RECV                         = 8, /** Event record identifier for the MpiRecv event. */
    HTF_EVENT_MPI_IRECV                        = 9, /** Event record identifier for the MpiIrecv event. */
    HTF_EVENT_MPI_REQUEST_TEST                 = 10, /** Event record identifier for the MpiRequestTest event. */
    HTF_EVENT_MPI_REQUEST_CANCELLED            = 11, /** Event record identifier for the MpiRequestCancelled event. */
    HTF_EVENT_MPI_COLLECTIVE_BEGIN             = 12, /** Event record identifier for the MpiCollectiveBegin event. */
    HTF_EVENT_MPI_COLLECTIVE_END               = 13, /** Event record identifier for the MpiCollectiveEnd event. */
    HTF_EVENT_OMP_FORK                         = 14, /** Event record identifier for the OmpFork event. */
    HTF_EVENT_OMP_JOIN                         = 15, /** Event record identifier for the OmpJoin event. */
    HTF_EVENT_OMP_ACQUIRE_LOCK                 = 16, /** Event record identifier for the OmpAcquireLock event. */
    HTF_EVENT_OMP_RELEASE_LOCK                 = 17, /** Event record identifier for the OmpReleaseLock event. */
    HTF_EVENT_OMP_TASK_CREATE                  = 18, /** Event record identifier for the OmpTaskCreate event. */
    HTF_EVENT_OMP_TASK_SWITCH                  = 19, /** Event record identifier for the OmpTaskSwitch event. */
    HTF_EVENT_OMP_TASK_COMPLETE                = 20, /** Event record identifier for the OmpTaskComplete event. */
    HTF_EVENT_METRIC                           = 21, /** Event record identifier for the Metric event. */
    HTF_EVENT_PARAMETER_STRING                 = 22, /** Event record identifier for the ParameterString event. */
    HTF_EVENT_PARAMETER_INT                    = 23, /** Event record identifier for the ParameterInt event. */
    HTF_EVENT_PARAMETER_UNSIGNED_INT           = 24, /** Event record identifier for the ParameterUnsignedInt event. */
    HTF_EVENT_THREAD_FORK                      = 25, /** Event record identifier for the ThreadFork event. */
    HTF_EVENT_THREAD_JOIN                      = 26, /** Event record identifier for the ThreadJoin event. */
    HTF_EVENT_THREAD_TEAM_BEGIN                = 27, /** Event record identifier for the ThreadTeamBegin event. */
    HTF_EVENT_THREAD_TEAM_END                  = 28, /** Event record identifier for the ThreadTeamEnd event. */
    HTF_EVENT_THREAD_ACQUIRE_LOCK              = 29, /** Event record identifier for the ThreadAcquireLock event. */
    HTF_EVENT_THREAD_RELEASE_LOCK              = 30, /** Event record identifier for the ThreadReleaseLock event. */
    HTF_EVENT_THREAD_TASK_CREATE               = 31, /** Event record identifier for the ThreadTaskCreate event. */
    HTF_EVENT_THREAD_TASK_SWITCH               = 32, /** Event record identifier for the ThreadTaskSwitch event. */
    HTF_EVENT_THREAD_TASK_COMPLETE             = 33, /** Event record identifier for the ThreadTaskComplete event. */
    HTF_EVENT_THREAD_CREATE                    = 34, /** Event record identifier for the ThreadCreate event. */
    HTF_EVENT_THREAD_BEGIN                     = 35, /** Event record identifier for the ThreadBegin event. */
    HTF_EVENT_THREAD_WAIT                      = 36, /** Event record identifier for the ThreadWait event. */
    HTF_EVENT_THREAD_END                       = 37, /** Event record identifier for the ThreadEnd event. */
    HTF_EVENT_IO_CREATE_HANDLE                 = 38, /** Event record identifier for the IoCreateHandle event. */
    HTF_EVENT_IO_DESTROY_HANDLE                = 39, /** Event record identifier for the IoDestroyHandle event. */
    HTF_EVENT_IO_DUPLICATE_HANDLE              = 40, /** Event record identifier for the IoDuplicateHandle event. */
    HTF_EVENT_IO_SEEK                          = 41, /** Event record identifier for the IoSeek event. */
    HTF_EVENT_IO_CHANGE_STATUS_FLAGS           = 42, /** Event record identifier for the IoChangeStatusFlags event. */
    HTF_EVENT_IO_DELETE_FILE                   = 43, /** Event record identifier for the IoDeleteFile event. */
    HTF_EVENT_IO_OPERATION_BEGIN               = 44, /** Event record identifier for the IoOperationBegin event. */
    HTF_EVENT_IO_OPERATION_TEST                = 45, /** Event record identifier for the IoOperationTest event. */
    HTF_EVENT_IO_OPERATION_ISSUED              = 46, /** Event record identifier for the IoOperationIssued event. */
    HTF_EVENT_IO_OPERATION_COMPLETE            = 47, /** Event record identifier for the IoOperationComplete event. */
    HTF_EVENT_IO_OPERATION_CANCELLED           = 48, /** Event record identifier for the IoOperationCancelled event. */
    HTF_EVENT_IO_ACQUIRE_LOCK                  = 49, /** Event record identifier for the IoAcquireLock event. */
    HTF_EVENT_IO_RELEASE_LOCK                  = 50, /** Event record identifier for the IoReleaseLock event. */
    HTF_EVENT_IO_TRY_LOCK                      = 51, /** Event record identifier for the IoTryLock event. */
    HTF_EVENT_PROGRAM_BEGIN                    = 52, /** Event record identifier for the ProgramBegin event. */
    HTF_EVENT_PROGRAM_END                      = 53, /** Event record identifier for the ProgramEnd event. */
    HTF_EVENT_NON_BLOCKING_COLLECTIVE_REQUEST  = 54, /** Event record identifier for the NonBlockingCollectiveRequest event. */
    HTF_EVENT_NON_BLOCKING_COLLECTIVE_COMPLETE = 55, /** Event record identifier for the NonBlockingCollectiveComplete event. */
    HTF_EVENT_COMM_CREATE                      = 56, /** Event record identifier for the CommCreate event. */
    HTF_EVENT_COMM_DESTROY                     = 57, /** Event record identifier for the CommDestroy event. */

    HTF_EVENT_MAX_ID
};

struct htf_event {
  uint8_t event_size;
  enum htf_record record;
  
  uint8_t event_data[256]; // todo: align on 256
} __attribute__((packed));

/*************************** Sequence **********************/
struct htf_sequence {
	/** Store the sequence of tokens. */
	htf_token_t* token;
	/** Number of tokens in the sequence. */
	unsigned size;
	/** Number of tokens allocated in token. */
	unsigned allocated;
	/** Timestamps for the start of these types of sequence. */
	htf_array_t timestamps;
	/** Durations for these types of sequences. */
	htf_timestamp_t* durations;
};

/*************************** Loop **********************/
struct htf_loop {
	unsigned nb_iterations;
	/** Token of the sequence being repeated. */
	htf_token_t token;
	/** Self-id. */
	struct htf_token id;
};

/** Only used when reading a trace, links an event with a timestamp. */
struct htf_event_occurence {
  struct htf_event event;
	htf_timestamp_t timestamp;
	htf_timestamp_t duration;
};

/**
 * Summary for an event. The nth time the event appears, it is at timestamps timestamps[n].
 */
struct htf_event_summary {
  struct htf_event event;
  htf_timestamp_t *timestamps;
  unsigned nb_allocated_timestamps;
  unsigned nb_timestamps;
};

#define HTF_UNDEFINED_UINT8  ( ( uint8_t )( ~( ( uint8_t )0u ) ) )
#define HTF_UNDEFINED_INT8  ( ( int8_t )( ~( HTF_UNDEFINED_UINT8 >> 1 ) ) )
#define HTF_UNDEFINED_UINT16 ( ( uint16_t )( ~( ( uint16_t )0u ) ) )
#define HTF_UNDEFINED_INT16  ( ( int16_t )( ~( HTF_UNDEFINED_UINT16 >> 1 ) ) )
#define HTF_UNDEFINED_UINT32 ( ( uint32_t )( ~( ( uint32_t )0u ) ) )
#define HTF_UNDEFINED_INT32  ( ( int32_t )( ~( HTF_UNDEFINED_UINT32 >> 1 ) ) )
#define HTF_UNDEFINED_UINT64 ( ( uint64_t )( ~( ( uint64_t )0u ) ) )
#define HTF_UNDEFINED_INT64  ( ( int64_t )( ~( HTF_UNDEFINED_UINT64 >> 1 ) ) )
#define HTF_UNDEFINED_TYPE HTF_UNDEFINED_UINT8

typedef uint32_t htf_string_ref_t;
#define HTF_STRING_REF_INVALID ( ( htf_string_ref_t )HTF_UNDEFINED_UINT32 )

struct htf_string {
  htf_string_ref_t string_ref;
  char* str;
  int length;
};

typedef uint32_t htf_region_ref_t;
#define HTF_REGIONREF_INVALID ( ( htf_region_ref_t )HTF_UNDEFINED_UINT32 )

struct htf_region {
  htf_region_ref_t region_ref;
  htf_string_ref_t string_ref;
  /* TODO: add other information (eg. file, line number, etc.)  */
};

typedef uint32_t htf_type_t;
typedef uint32_t htf_attribute_ref_t;
typedef uint32_t htf_attribute_value_t;

typedef struct htf_attribute_list {
  htf_type_t type_id;
  htf_attribute_ref_t attribute_ref;
  htf_attribute_value_t value;
  struct htf_attribute_list* next;
} htf_attribute_list_t;


typedef uint32_t htf_thread_id_t;
#define HTF_THREAD_ID_INVALID ( ( htf_thread_id_t )HTF_UNDEFINED_UINT32 )

typedef uint32_t htf_location_group_id_t;
#define HTF_LOCATION_GROUP_ID_INVALID ( ( htf_location_group_id_t )HTF_UNDEFINED_UINT32 )
#define HTF_MAIN_LOCATION_GROUP_ID ( ( htf_location_group_id_t ) HTF_LOCATION_GROUP_ID_INVALID - 1 )

/* A thread contains streams of events.
 * It can be a regular thread (eg. a pthread), or a GPU stream
 */
struct htf_thread {
  struct htf_archive *archive;
  htf_thread_id_t id;

  struct htf_event_summary *events;
  unsigned nb_allocated_events;
  unsigned nb_events;

	struct htf_sequence** sequences;
	unsigned nb_allocated_sequences;
	unsigned nb_sequences;

	struct htf_loop* loops;
	unsigned nb_allocated_loops;
	unsigned nb_loops;
};


/* a location_group can be a process, a machine, etc. */
struct htf_location_group {
  htf_location_group_id_t id;
  htf_string_ref_t        name;
  htf_location_group_id_t parent;
};

/* a location is basically a thread (or GPU stream) */
struct htf_location {
  htf_thread_id_t         id;
  htf_string_ref_t        name;
  htf_location_group_id_t parent;
};

struct htf_definition {
  struct htf_string *strings;
  int nb_strings;
  int nb_allocated_strings;

  struct htf_region *regions;
  int nb_regions;
  int nb_allocated_regions;
};

struct htf_archive {
  char* dir_name;
  char* trace_name;
  char* fullpath;

  htf_location_group_id_t id;
  struct htf_archive* global_archive;

  pthread_mutex_t lock;

  struct htf_definition definitions;

  struct htf_location_group* location_groups;
  int nb_location_groups;
  int nb_allocated_location_groups;

  struct htf_location* locations;
  int nb_locations;
  int nb_allocated_locations;

  /* a list of threads */
  struct htf_thread **threads;
  int nb_threads;
  int nb_allocated_threads;

  struct htf_archive **archive_list;
  int nb_archives;
	int nb_allocated_archives;
};


const char* htf_get_thread_name(struct htf_thread* thread);

/* Print the content of sequence seq_id */
void htf_print_sequence(struct htf_thread *thread,
			htf_sequence_id_t seq_id);

/* Print the subset of a token array */
void htf_print_token_array(struct htf_thread *thread,
			   htf_token_t* token_array,
			   int index_start,
			   int index_stop);

/* Print a token */
void htf_print_token(struct htf_thread *thread,
		     htf_token_t token);

void htf_print_event(struct htf_thread *thread,
		     struct htf_event* e);


/* return the loop whose id is loop_id
 * return NULL if loop_id is unknown
 */
struct htf_loop* htf_get_loop(struct htf_thread *thread_trace, htf_loop_id_t loop_id);

/* return the sequence whose id is sequence_id
 * return NULL if sequence_id is unknown
 */
struct htf_sequence* htf_get_sequence(struct htf_thread *thread_trace, htf_sequence_id_t seq_id);

/* return the event whose id is event_id
 * return NULL if event_id is unknown
 */
struct htf_event* htf_get_event(struct htf_thread *thread_trace, htf_event_id_t evt_id);




/* return the index_th token of a sequence/loop */
htf_token_t htf_get_token(struct htf_thread *trace,
			  htf_token_t sequence,
			  int index);


/* return 1 if array1 and array1 are equal */
static inline int _htf_arrays_equal(htf_token_t *array1, int size1,
				    htf_token_t* array2, int size2) {
  if(size1 != size2)
    return 0;
  return memcmp(array1, array2, sizeof(htf_token_t)*size1) == 0;
}


/* return 1 if s1 and s2 are equal */
static inline int _htf_sequences_equal(struct htf_sequence *s1,
				       struct htf_sequence *s2) {
  if((!s1) || (!s2))
    return 0;
  return _htf_arrays_equal(s1->token, s1->size, s2->token, s2->size);
}

/**
 * Given a buffer, a counter that indicates the number of object it holds, and this object's datatype,
 * doubles the size of the buffer using realloc, or if it fails, malloc and memmove then frees the old buffer.
 * This is better than a realloc because it moves the data around, but it is also slower.
 * Checks for error at malloc.
 */
#define DOUBLE_MEMORY_SPACE(buffer, counter, datatype) do {               \
  datatype * new_buffer = realloc(buffer, 2 * counter * sizeof(datatype));\
  if (new_buffer == NULL) {                                               \
    new_buffer = malloc(counter * sizeof(datatype) * 2);                  \
    if (new_buffer == NULL) {                                             \
      htf_error("Failed to allocate memory using realloc AND malloc\n");  \
    }                                                                     \
    memmove(new_buffer, buffer, counter * sizeof(datatype));              \
    free(buffer);                                                         \
  }                                                                       \
	buffer = new_buffer;                                                    \
  counter *= 2;                                                           \
} while (0)
/**
 * Given a buffer, a counter that indicates the number of object it holds, and this object's datatype,
 * Increments the size of the buffer by 1 using realloc, or if it fails, malloc and memmove then frees the old buffer.
 * This is better than a realloc because it moves the data around, but it is also slower.
 * Checks for error at malloc.
 */
#define INCREMENT_MEMORY_SPACE(buffer, counter, datatype) do {            \
  datatype * new_buffer = realloc(buffer, (counter + 1) * sizeof(datatype));\
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

#define DOFOR(var_name, max) for (int var_name = 0; var_name < max; var_name++)

#endif /* EVENT_H */
