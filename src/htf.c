#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "htf.h"
#include "htf_timestamp.h"

#define NB_TOKEN_DEFAULT 1000000
#define NB_EVENT_DEFAULT 1000
#define NB_SEQUENCE_DEFAULT 1000
#define NB_LOOP_DEFAULT 1000
#define NB_TIMESTAMP_DEFAULT 1000000

static int verbose=0;
static _Thread_local int recursion_shield = 0;

static inline event_id _htf_get_event_id(struct thread_trace *thread_trace,
					 struct event *e) {
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


void _htf_store_timestamp(struct thread_trace *thread_trace,
			  event_id e_id,
			  timestamp_t ts) {
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

void _htf_store_token(struct thread_trace *thread_trace,
		      token_t t) {
  if(thread_trace->nb_tokens >= thread_trace->nb_allocated_tokens) {
    fprintf(stderr, "Error: too many tokens\n");
    abort();
  }
  if(verbose)
    printf("E#%d: (%d, %d)\n", thread_trace->nb_tokens, TOKEN_TYPE(t), TOKEN_ID(t));
  thread_trace->tokens[thread_trace->nb_tokens++] = t;
}

void _htf_store_event(struct thread_trace *thread_trace,
		      event_id id) {
  //  assert((2<<30) > event_id);  
  token_t t = TOKENIZE(TYPE_EVENT, id);
  _htf_store_token(thread_trace, t);
}

void htf_record_event(struct thread_trace *thread_trace,
		      enum event_type event_type,
		      int function_id) {
  if(recursion_shield)
    return;
  recursion_shield++;

  struct event e = {.function_id = function_id, .event_type = event_type};
  event_id e_id = _htf_get_event_id(thread_trace, &e);

  _htf_store_timestamp(thread_trace, e_id, htf_get_timestamp());
  _htf_store_event(thread_trace, e_id);
  recursion_shield--;
}

void htf_write_finalize(struct trace *trace) {
  if(!trace)
    return;

  htf_storage_finalize(trace);
}

void htf_write_init(struct trace *trace, const char* dirname) {
  if(recursion_shield)
    return;
  recursion_shield++;

  trace->threads = NULL;
  trace->nb_threads = 0;
  pthread_mutex_init(&trace->lock, NULL);

  char* verbose_str = getenv("VERBOSE");
  if(verbose_str)
    verbose = 1;

  htf_storage_init(dirname);

  recursion_shield--;
}

void htf_write_init_thread(struct trace* trace,
			   struct thread_trace *thread_trace,
			   int thread_rank) {
  if(recursion_shield)
    return;
  recursion_shield++;

  if(verbose)
    printf("htf_write_init_thread\n");

  assert(thread_rank >= 0);
  assert(thread_rank >= trace->nb_threads);

  thread_trace->trace = trace;
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
    trace->nb_threads++;
    if(thread_rank > trace->nb_threads)
      trace->nb_threads = thread_rank;
    trace->threads = realloc(trace->threads, sizeof(struct thread_trace *) * trace->nb_threads);
    trace->threads[thread_rank] = thread_trace;
  }
  pthread_mutex_unlock(&trace->lock);
  recursion_shield--;
}


void htf_read_thread_iterator_init(struct thread_trace_reader *reader,
				   struct trace* trace,
				   int thread_index) {
  reader->trace = trace;
  assert(thread_index < trace->nb_threads);
  reader->thread_trace = trace->threads[thread_index];
  reader->next_event = 0;
  reader->event_index = calloc(sizeof(int), trace->threads[thread_index]->nb_events);
}


int _htf_read_thread_next_event(struct thread_trace_reader *reader,
				struct event_occurence *e,
				int move) {
  if(reader->next_event < 0)
    return -1;			/* TODO: return EOF */

  if(reader->next_event >= reader->thread_trace->nb_tokens)
    return -1;			/* TODO: return EOF */

  token_t t = reader->thread_trace->tokens[reader->next_event];
  switch(TOKEN_TYPE(t)) {
  case TYPE_EVENT:
    memcpy(&e->event, &reader->thread_trace->events[TOKEN_ID(t)].event, sizeof(e->event));
    struct event_summary* es = &reader->thread_trace->events[TOKEN_ID(t)];
    e->timestamp = es->timestamps[reader->event_index[TOKEN_ID(t)]];

    if(move)
      reader->event_index[TOKEN_ID(t)]++;
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
  if(move)
    reader->next_event++;
  return 0;
}

int htf_read_thread_cur_event(struct thread_trace_reader *reader,
			      struct event_occurence *e) {
  return _htf_read_thread_next_event(reader, e, 0);
}

int htf_read_thread_next_event(struct thread_trace_reader *reader,
			       struct event_occurence *e) {

  return _htf_read_thread_next_event(reader, e, 1);
}
