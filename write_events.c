#include "liblock.h"
#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include "timestamp.h"

static struct trace *trace = NULL;
static _Thread_local struct thread_trace* thread_trace = NULL;
static _Thread_local int thread_rank;

static int verbose=0;

#define NB_TOKEN_DEFAULT 100000
#define NB_EVENT_DEFAULT 1000
#define NB_SEQUENCE_DEFAULT 1000
#define NB_LOOP_DEFAULT 1000
#define NB_TIMESTAMP_DEFAULT 100000

event_id get_event_id(struct event *e) {
  if(verbose)
    printf("Searching for {.func=%d, .event_type=%d}\n", e->function_id, e->event_type);

  for(unsigned i = 0; i < thread_trace->nb_events; i++) {
    if(memcmp(e, &thread_trace->events[i].event, sizeof(struct event)) == 0) {
      if(verbose)
	printf("\t found with id=%u\n", i);
      return (event_id) i;
    }
  }

  if(thread_trace->nb_events >= thread_trace->nb_allocated_events) {
    fprintf(stderr, "error: too many event data!\n");
    abort();
  }

  event_id index = thread_trace->nb_events++;
  if(verbose)
    printf("\tNot found. Adding it with id=%u\n", index);
  struct event_summary *es = &thread_trace->events[index];

  memcpy(&es->event, e, sizeof(struct event));
  es->timestamps = malloc(sizeof(timestamp_t)* NB_TIMESTAMP_DEFAULT);
  es->nb_allocated_timestamps = NB_TIMESTAMP_DEFAULT;
  es->nb_timestamps = 0;

  return index;
}


void store_timestamp(event_id e_id, timestamp_t ts) {
  assert(e_id < thread_trace->nb_allocated_events);

  struct event_summary *es = &thread_trace->events[e_id];
  assert(es);

  if(es->nb_timestamps >= es->nb_allocated_timestamps) {
    fprintf(stderr, "Warning: realloc timestamps for event %u\n", e_id);
    es->nb_allocated_timestamps *= 2;
    es->timestamps = realloc(es->timestamps, es->nb_allocated_timestamps * sizeof(timestamp_t));
  }

  es->timestamps[es->nb_timestamps++] = ts;  
}

void store_token(token_t t) {
  if(thread_trace->nb_tokens >= thread_trace->nb_allocated_tokens) {
    fprintf(stderr, "Error: too many tokens\n");
    abort();
  }
  thread_trace->tokens[thread_trace->nb_tokens++] = t;
}

void store_event(event_id id) {
  //  assert((2<<30) > event_id);
  token_t t = {.token_type= TYPE_EVENT, .token_id = id };
  store_token(t);
}

static void _init_thread( );
void init_trace() {
  trace = malloc(sizeof(struct trace));
  trace->threads = NULL;
  trace->nb_threads = 0;
  pthread_mutex_init(&trace->lock, NULL);
}

void record_event(enum event_type event_type,
		  enum intercepted_function f,
		  void* ptr) {

  if(trace == NULL)
    init_trace();
  if(thread_trace == NULL)
    _init_thread();
  struct event e = {.function_id = (int)f, .event_type = event_type};
  event_id e_id = get_event_id(&e);

  store_timestamp(e_id, get_timestamp());
  store_event(e_id);
}

void enter_function(enum intercepted_function f, void* ptr) {
  DEBUG_PRINTF("Entering %s\n", function_names[f]);
  record_event(function_entry, f, ptr);
}

void leave_function(enum intercepted_function f, void* ptr) {
  DEBUG_PRINTF("Leaving %s\n", function_names[f]);
  record_event(function_exit, f, ptr);
}

static void _init_thread( ) {
  thread_trace = malloc(sizeof(struct thread_trace));

  thread_trace->tokens = malloc(sizeof(token_t)* NB_TOKEN_DEFAULT);
  thread_trace->nb_allocated_tokens = NB_TOKEN_DEFAULT;
  thread_trace->nb_tokens = 0;

  thread_trace->events = malloc(sizeof(struct event_summary) * NB_EVENT_DEFAULT);
  thread_trace->nb_allocated_events = NB_EVENT_DEFAULT;
  thread_trace->nb_events = 0;

  thread_trace->sequences = malloc(sizeof(struct sequence) * NB_SEQUENCE_DEFAULT);
  thread_trace->nb_allocated_sequences = NB_SEQUENCE_DEFAULT;
  thread_trace->nb_sequences = 0;

  thread_trace->loops = malloc(sizeof(struct loop) * NB_LOOP_DEFAULT);
  thread_trace->nb_allocated_loops = NB_LOOP_DEFAULT;
  thread_trace->nb_loops = 0;

  pthread_mutex_lock(&trace->lock);
  {
    thread_rank = trace->nb_threads++;
    trace->threads = realloc(trace->threads, sizeof(struct thread_trace *) * trace->nb_threads);
    trace->threads[thread_rank] = thread_trace;
  }
  pthread_mutex_unlock(&trace->lock);
}


static void _write_events_init(void) __attribute__((constructor));
static void _write_events_init(void) {
  DEBUG_PRINTF("[LIBLOCK] initializing write_events\n");

  char* verbose_str = getenv("VERBOSE");
  if(verbose_str)
    verbose = 1;

  trace_storage_init();
}

static void _write_events_conclude(void) __attribute__((destructor));
static void _write_events_conclude(void) {
  DEBUG_PRINTF("[LIBLOCK] finalizing write_events\n");

  write_trace(trace);
}
