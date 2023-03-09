#ifndef HTF_H
#define HTF_H

#include <pthread.h>
#include <string.h>
#include "htf_timestamp.h"
#include "htf_dbg.h"

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

//#define TOKEN_ID_INVALID    0x3fffffff
#define TOKEN_ID_INVALID    0x3deadbee
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

static inline event_id_t TOKEN_TO_EVENT_ID(token_t t) { return (event_id_t) {.id=TOKEN_ID(t)};}
static inline sequence_id_t TOKEN_TO_SEQUENCE_ID(token_t t) { return (sequence_id_t) {.id=TOKEN_ID(t)};}
static inline loop_id_t TOKEN_TO_LOOP_ID(token_t t) { return (loop_id_t) {.id=TOKEN_ID(t)};}

#define IS_MAIN_SEQUENCE(_s) (SEQUENCE_ID(_s).id == SEQUENCE_ID_INVALID)

/*************************** Events **********************/

enum event_type {
   function_entry,
   function_exit,
   singleton,
};

struct event {
  enum event_type event_type;
  /* TODO: update the content of the event */

  int function_id;
  //uintptr_t ptr;
  //  pthread_t tid;
  //  enum intercepted_function function;
};

/*************************** Sequence **********************/

struct sequence {
  token_t *token;		/* TODO: don't use a pointer here! */
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
  _Atomic int nb_threads;
  pthread_mutex_t lock;
};



void htf_storage_init();

void htf_storage_finalize(struct trace*trace);



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

/* return the index_th token of a sequence/loop */
token_t htf_get_token(struct thread_trace *trace,
		  token_t sequence,
		  int index);


/* return the loop whose id is loop_id
 * return NULL if loop_id is unknown
 */
struct loop* htf_get_loop(struct thread_trace *thread_trace,
			  loop_id_t loop_id);

/* return the sequence whose id is sequence_id
 * return NULL if sequence_id is unknown
 */
struct sequence* htf_get_sequence(struct thread_trace *thread_trace,
				  sequence_id_t seq_id);

/* return the event whose id is event_id
 * return NULL if event_id is unknown
 */
struct event* htf_get_event(struct thread_trace *thread_trace,
			    event_id_t evt_id);




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
