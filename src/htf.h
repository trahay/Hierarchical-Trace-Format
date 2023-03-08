#ifndef HTF_H
#define HTF_H

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

struct thread_writer {
  struct thread_trace thread_trace;
  struct sequence **og_seq;
  int cur_depth;
  int max_depth;
  int thread_rank;
};

struct thread_reader {
  struct trace *trace;
  struct thread_trace *thread_trace;

  token_t *callstack_sequence;	/* each entry contains the sequence/loop being read */
  int     *callstack_index;	/* each entry contains the index in the sequence or the loop iteration */
  int     *callstack_loop_iteration;	/* each entry contains the number of iteration of the loop at the corresponding frame */

  int     current_frame;

  int *event_index;
};

/* Initialize a trace in write mode */
void htf_write_init(struct trace *trace, const char* dirname);
void htf_write_init_thread(struct trace* trace,
			   struct thread_writer *thread_writer,
			   int thread_rank);

void htf_record_event(struct thread_writer* thread_writer,
		      enum event_type event_type,
		      int function_id);
void htf_write_finalize(struct trace* trace);

void htf_storage_init();

void htf_storage_finalize(struct trace*trace);




void htf_read_trace(struct trace* trace, char* filename);

void htf_read_thread_iterator_init(struct thread_reader *reader,
				   struct trace* trace,
				   int thread_index);

/* return the current event in a thread and move to the next one.
 * Return -1 in case of an error (such as the end of the trace)
 */
int htf_read_thread_next_event(struct thread_reader *reader,
			       struct event_occurence *e);
/* return the current event in a thread.
 * Return -1 in case of an error (such as the end of the trace)
 */
int htf_read_thread_cur_event(struct thread_reader *reader,
			      struct event_occurence *e);

#endif /* EVENT_H */
