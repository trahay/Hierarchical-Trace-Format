#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "htf.h"
#include "tracer.h"

static struct trace trace;
static _Thread_local struct thread_trace* thread_trace = NULL;
static _Thread_local int thread_rank;
static _Atomic int nb_threads = 0;
static int verbose=0;
static int initialized = 0;

void _init_thread() {
  if(! initialized) return;

  thread_rank = nb_threads++;
  thread_trace = malloc(sizeof(struct thread_trace));
  htf_write_init_thread(&trace,
			thread_trace,
			thread_rank);    
}

void enter_function(enum intercepted_function f, void* ptr) {
  if(! initialized) return;

  if(thread_trace == NULL)
    _init_thread();

  int index = thread_trace->nb_tokens;
  printf("Entering %s (%d)\n", function_names[f], index);
  fflush(stdout);
  htf_record_event(thread_trace, function_entry, (int)f);
  //  assert( thread_trace->nb_tokens > index);
  
}

void leave_function(enum intercepted_function f, void* ptr) {
  if(! initialized) return;

  printf("Leaving %s\n", function_names[f]);
  fflush(stdout);
  htf_record_event(thread_trace, function_exit, (int)f);
}

void* get_callback(const char*fname){
  void* ret = dlsym(RTLD_NEXT, fname);
  if(!ret) {
    fprintf(stderr, "Warning: cannot find %s: %s\n", fname, dlerror());
  }
  return ret;
}


static void _tracer_init(void) __attribute__((constructor));
static void _tracer_init(void) {
  printf("[TRACER] initializing\n");

  char* verbose_str = getenv("VERBOSE");
  if(verbose_str)
    verbose = 1;

  htf_write_init(&trace);

  initialized = 1;
}

static void _tracer_conclude(void) __attribute__((destructor));
static void _tracer_conclude(void) {
  printf("[TRACER] finalizing write_events\n");

  htf_write_finalize(&trace);
}
