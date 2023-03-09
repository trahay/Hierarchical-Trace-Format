#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "htf.h"
#include "htf_write.h"
#include "tracer.h"

static struct trace trace;
static _Thread_local struct thread_writer* thread_writer = NULL;
static _Thread_local int thread_rank;
static _Atomic int nb_threads = 0;
static int verbose=0;
static int initialized = 0;

void _init_thread() {
  if(! initialized) return;

  thread_rank = nb_threads++;
  thread_writer = malloc(sizeof(struct thread_writer));
  htf_write_init_thread(&trace,
			thread_writer,
			thread_rank);    
}

void enter_function(enum intercepted_function f, void* ptr) {
  if(! initialized) return;

  if(thread_writer == NULL)
    _init_thread();

  htf_record_event(thread_writer, function_entry, (int)f);
}

void leave_function(enum intercepted_function f, void* ptr) {
  if(! initialized) return;

  htf_record_event(thread_writer, function_exit, (int)f);
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

  char* base_dirname = getenv("TRACE_FILENAME");
  if(! base_dirname) {
    base_dirname=malloc(sizeof(char)*100);
    snprintf(base_dirname, 100, "trace");
  }
  htf_write_init(&trace, base_dirname);

  initialized = 1;
}

static void _tracer_conclude(void) __attribute__((destructor));
static void _tracer_conclude(void) {
  printf("[TRACER] finalizing write_events\n");

  htf_write_finalize(&trace);
}
