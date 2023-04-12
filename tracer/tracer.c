#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "htf.h"
#include "htf_write.h"
#include "htf_archive.h"
#include "tracer.h"

static struct htf_archive trace;
static htf_location_group_id_t process_id;
static _Thread_local struct htf_thread_writer* thread_writer = NULL;
static _Thread_local htf_thread_id_t thread_id;
static _Thread_local int thread_rank;
static _Atomic int nb_threads = 0;
static struct htf_thread_writer** thread_writers = NULL;
static pthread_mutex_t lock;

static int verbose=0;
static int initialized = 0;

static htf_string_ref_t _register_string(char* str) {
  static htf_string_ref_t next_ref = 0;
  htf_string_ref_t ref = next_ref++;

  htf_archive_register_string(&trace, ref, str);
  return ref;
}

static htf_location_group_id_t _new_location_group() {
  static _Atomic htf_location_group_id_t next_id = 0;
  htf_location_group_id_t id = next_id++;
  return id;
}

static htf_thread_id_t _new_thread() {
  static _Atomic htf_thread_id_t next_id = 0;
  htf_thread_id_t id = next_id++;
  return id;
}

void _init_thread() {
  if(! initialized) return;

  pthread_mutex_lock(&lock);
  thread_rank = nb_threads++;
  thread_writer = malloc(sizeof(struct htf_thread_writer));
  thread_writers = realloc(thread_writers, sizeof(struct htf_thread_writer*)* nb_threads);
  thread_writers[thread_rank] = thread_writer;
  pthread_mutex_unlock(&lock);

  char thread_name[20];
  snprintf(thread_name, 20, "thread_%d", thread_rank);
  htf_string_ref_t thread_name_id = _register_string(thread_name);

  thread_id = _new_thread();
  htf_write_define_location(&trace,
			    thread_id,
			    thread_name_id,
			    process_id);

  htf_write_thread_open(&trace, thread_writer, thread_id);
}

void enter_function(enum intercepted_function f, void* ptr) {
  if(! initialized) return;

  if(thread_writer == NULL)
    _init_thread();

  htf_record_enter(thread_writer, NULL, HTF_TIMESTAMP_INVALID, (int)f);
}

void leave_function(enum intercepted_function f, void* ptr) {
  if(! initialized) return;

  htf_record_leave(thread_writer, NULL, HTF_TIMESTAMP_INVALID, (int)f);
}

void* get_callback(const char*fname){
  void* ret = dlsym(RTLD_NEXT, fname);
  if(!ret) {
    fprintf(stderr, "Warning: cannot find %s: %s\n", fname, dlerror());
  }
  return ret;
}


static void _get_trace_name(char* buffer) {
  char tmp_buffer[1024];
  FILE* f = fopen("/proc/self/cmdline", "r");
  fscanf(f, "%s", tmp_buffer);
  fclose(f);
  sprintf(buffer, "%s_trace", basename(tmp_buffer));
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

  pthread_mutex_init(&lock, NULL);

  char program_name[1024];
  _get_trace_name(program_name);
  
  htf_write_archive_open(&trace,
			 program_name,
			 "main",
			 0);

  process_id = _new_location_group();
  htf_write_define_location_group(&trace,
				  process_id,
				  _register_string("Process"),
				  HTF_LOCATION_GROUP_ID_INVALID);
  
  for(int i = 0; i<NB_FUNCTIONS; i++) {
    htf_string_ref_t string_ref = _register_string(function_names[i]);
    htf_archive_register_region(&trace, i, string_ref);
  }

  initialized = 1;
}

static void _tracer_conclude(void) __attribute__((destructor));
static void _tracer_conclude(void) {
  printf("[TRACER] finalizing write_events\n");

  for(int i=0; i<nb_threads; i++) {
    htf_write_thread_close(thread_writers[i]);
  }

  htf_write_archive_close(&trace);
}
