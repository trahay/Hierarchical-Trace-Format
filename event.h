#ifndef EVENT_H
#define EVENT_H

#include "timestamp.h"
#include <uthash.h>

/* A token is either:
   - an event
   - a sequence (ie a list of tokens)
   - a loop (a repetition of sequences)
*/

typedef int event_id;
typedef int sequence_id;
typedef int loop_id;

#define EVENT_ID_INVALID ((unsigned)-1)

/* Token types */
#define TYPE_INVALID  0
#define TYPE_EVENT    1
#define TYPE_SEQUENCE 2
#define TYPE_LOOP     3

/* identify a token */
typedef struct token {
  unsigned token_type : 2; 	/* can be ETYPE_INVALID, ETYPE_EVENT, ETYPE_SEQUENCE, or ETYPE_LOOP */
  unsigned token_id   : 30;	/* can be an event_id, a sequence_id, or a loop_id */
} token_t;


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
  unsigned length;
  token_t *token;		/* TODO: don't use a pointer here! */
};

/*************************** Loop **********************/

struct loop {
  unsigned nb_iterations;
  token_t token;
};


struct event_summary {
  struct event event;
  timestamp_t *timestamps;
  unsigned nb_allocated_timestamps;
  unsigned nb_timestamps;
};

struct thread_trace {
  token_t *tokens;
  unsigned nb_allocated_tokens;
  unsigned nb_tokens;


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

#endif /* EVENT_H */
