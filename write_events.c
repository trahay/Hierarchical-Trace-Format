#include "liblock.h"
#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#include "timestamp.h"
#include "trace_storage.h"

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
  printf("E#%d: (%d, %d)\n", thread_trace->nb_tokens, TOKEN_TYPE(t), TOKEN_ID(t));
  thread_trace->tokens[thread_trace->nb_tokens++] = t;
}

void store_event(event_id id) {
  //  assert((2<<30) > event_id);  
  token_t t = TOKENIZE(TYPE_EVENT, id);
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

  //#error Bug here: the first event is of a thread is not recorded
  if(trace == NULL)
    init_trace();
  if(thread_trace == NULL)
    _init_thread();
  struct event e = {.function_id = (int)f, .event_type = event_type};
  event_id e_id = get_event_id(&e);

  store_timestamp(e_id, get_timestamp());
  store_event(e_id);
}

static _Thread_local int recursion_shield = 0;
void enter_function(enum intercepted_function f, void* ptr) {
  if(recursion_shield)
    return;
  recursion_shield++;

  if(trace == NULL)
    init_trace();
  if(thread_trace == NULL)
    _init_thread();

  int index = thread_trace->nb_tokens;
  printf("Entering %s (%d)\n", function_names[f], index);
  fflush(stdout);
  record_event(function_entry, f, ptr);
  assert( thread_trace->nb_tokens > index);
  
  recursion_shield--;
}

void leave_function(enum intercepted_function f, void* ptr) {
  if(recursion_shield)
    return;
  recursion_shield++;

  printf("Leaving %s\n", function_names[f]);
  fflush(stdout);
  record_event(function_exit, f, ptr);

  recursion_shield--;
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

void thread_trace_reader_init(struct thread_trace_reader *reader,
			      struct trace* trace,
			      int thread_index) {
  reader->trace = trace;
  assert(thread_index < trace->nb_threads);
  reader->thread_trace = trace->threads[thread_index];
  reader->next_event = 0;
  reader->event_index = calloc(sizeof(int), trace->threads[thread_index]->nb_events);
}

int thread_trace_reader_next_event(struct thread_trace_reader *reader,
				   struct event_occurence *e) {
  if(reader->next_event < 0)
    return -1;			/* TODO: return EOF */

  if(reader->next_event >= reader->thread_trace->nb_tokens)
    return -1;			/* TODO: return EOF */

  token_t t = reader->thread_trace->tokens[reader->next_event];
  switch(TOKEN_TYPE(t)) {
  case TYPE_EVENT:
    memcpy(&e->event, &reader->thread_trace->events[TOKEN_ID(t)].event, sizeof(e->event));
#if 0
    e->timestamp = 0;
#else
    struct event_summary* es = &reader->thread_trace->events[TOKEN_ID(t)];
    e->timestamp = es->timestamps[reader->event_index[TOKEN_ID(t)]++];
#endif
    break;
  case TYPE_SEQUENCE:
    fprintf(stderr, "SEQUENCE events not supported yet !\n");
    abort();
    break;
  case TYPE_LOOP:
    fprintf(stderr, "LOOP events not supported yet !\n");
    abort();
    break;
  case TYPE_INVALID:
    fprintf(stderr, "INVALID event detected at position %d\n", reader->next_event);
    abort();
    break;
  }
  reader->next_event++;
  return 0;
}
