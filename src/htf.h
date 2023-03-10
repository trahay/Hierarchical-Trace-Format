#ifndef HTF_H
#define HTF_H

#include <pthread.h>
#include <string.h>

#include "htf_timestamp.h"
#include "htf_dbg.h"
#include "htf.h"

/* A token is either:
   - an event
   - a sequence (ie a list of tokens)
   - a loop (a repetition of sequences)
*/


/* Token types */
typedef enum token_type {
  TYPE_INVALID  = 0,
  TYPE_EVENT    = 1,
  TYPE_SEQUENCE = 2,
  TYPE_LOOP     = 3
} token_type_t;

typedef uint32_t token_id_t;

typedef struct token {
  enum token_type type : 2;
  token_id_t id : 30;
} token_t;


typedef struct event_id {
  token_id_t id : 30;
} event_id_t;

typedef struct sequence_id {
  token_id_t id : 30;
} sequence_id_t;

typedef struct loop_id {
  token_id_t id : 30;
} loop_id_t;

#define TOKEN_ID_INVALID    0x3fffffff
#define EVENT_ID_INVALID    TOKEN_ID_INVALID
#define SEQUENCE_ID_INVALID TOKEN_ID_INVALID
#define LOOP_ID_INVALID     TOKEN_ID_INVALID

/* convert an id to and integer */
#define ID(_id) ((_id).id)

/* return the type/id of a token */
#define TOKEN_TYPE(t) ( (t).type)
#define TOKEN_ID(t) ( (t).id)

/* build a token */
static inline token_t TOKENIZE(token_type_t t, token_id_t id) { return (token_t) {.type=t, .id=id};} 

/* convert an index to an event_id_t */
static inline event_id_t EVENT_ID(int index) { return (event_id_t) {.id=index};}
static inline sequence_id_t SEQUENCE_ID(int index) {return (sequence_id_t) {.id=index};}
static inline loop_id_t LOOP_ID(int index) { return (loop_id_t) {.id=index};}

/* convert a token_t to an event_id_t */
static inline event_id_t TOKEN_TO_EVENT_ID(token_t t) { return (event_id_t) {.id=TOKEN_ID(t)};}
static inline sequence_id_t TOKEN_TO_SEQUENCE_ID(token_t t) { return (sequence_id_t) {.id=TOKEN_ID(t)};}
static inline loop_id_t TOKEN_TO_LOOP_ID(token_t t) { return (loop_id_t) {.id=TOKEN_ID(t)};}


/*************************** Events **********************/
enum event_type {
   function_entry, /* TODO: rename block_start */
   function_exit,  /* TODO: rename block_end */
   singleton,
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

struct event {
  //  enum event_type event_type;
  /* TODO: update the content of the event */
  uint8_t event_size;
  enum htf_record record;
  
  uint8_t event_data[256]; // todo: align on 256

  //  int function_id;
  //uintptr_t ptr;
  //  pthread_t tid;
  //  enum intercepted_function function;
} __attribute__((packed));

/*************************** Sequence **********************/
struct sequence {
  token_t *token;
  unsigned size;
  unsigned allocated;
};

/*************************** Loop **********************/
struct loop {
  unsigned nb_iterations;
  token_t token;
};


struct event_occurence {
  struct event event;
  timestamp_t timestamp;
};

struct event_summary {
  struct event event;
  timestamp_t *timestamps;
  unsigned nb_allocated_timestamps;
  unsigned nb_timestamps;
};

struct thread_trace {
  struct trace *trace;

  struct event_summary *events;
  unsigned nb_allocated_events;
  unsigned nb_events;

  struct sequence *sequences;
  unsigned nb_allocated_sequences;
  unsigned nb_sequences;

  struct loop *loops;
  unsigned nb_allocated_loops;
  unsigned nb_loops;
};

struct trace {
  struct thread_trace **threads;
  _Atomic int allocated_threads;
  _Atomic int nb_threads;
  pthread_mutex_t lock;
};



/* Print the content of sequence seq_id */
void htf_print_sequence(struct thread_trace *thread_trace,
			sequence_id_t seq_id);

/* Print the subset of a token array */
void htf_print_token_array(struct thread_trace *thread_trace,
		       token_t* token_array,
		       int index_start,
		       int index_stop);

/* Print a token */
void htf_print_token(struct thread_trace *thread_trace,
		     token_t token);

void htf_print_event(struct thread_trace *thread_trace, struct event* e);


/* return the loop whose id is loop_id
 * return NULL if loop_id is unknown
 */
struct loop* htf_get_loop(struct thread_trace *thread_trace, loop_id_t loop_id);

/* return the sequence whose id is sequence_id
 * return NULL if sequence_id is unknown
 */
struct sequence* htf_get_sequence(struct thread_trace *thread_trace, sequence_id_t seq_id);

/* return the event whose id is event_id
 * return NULL if event_id is unknown
 */
struct event* htf_get_event(struct thread_trace *thread_trace, event_id_t evt_id);




/* return the index_th token of a sequence/loop */
token_t htf_get_token(struct thread_trace *trace,
		      token_t sequence,
		      int index);


/* return 1 if array1 and array1 are equal */
static inline int _htf_arrays_equal(token_t *array1, int size1,
				    token_t* array2, int size2) {
  if(size1 != size2)
    return 0;
  return memcmp(array1, array2, sizeof(token_t)*size1) == 0;
}


/* return 1 if s1 and s2 are equal */
static inline int _htf_sequences_equal(struct sequence *s1,
				   struct sequence *s2) {
  if((!s1) || (!s2))
    return 0;
  return _htf_arrays_equal(s1->token, s1->size, s2->token, s2->size);
}

#endif /* EVENT_H */
