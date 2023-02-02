#include "liblock.h"
#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

_Atomic int nb_calls[NB_FUNCTIONS];
FILE* trace_file=NULL;

#define BUFFER_SIZE 1000000
struct lock_event buffer[BUFFER_SIZE];
_Atomic int next_event = 0;
_Atomic int nrecords=0;

#define TIME_DIFF(t1, t2) (t2.tv_sec - t1.tv_sec)*1000000000 + (t2.tv_nsec - t1.tv_nsec)
struct timespec first_timestamp={0, 0};

void flush_buffer() {
  printf("About to flush... nrecords=%d, buffer_size=%d\n", nrecords, BUFFER_SIZE);
  int ret = fwrite(buffer, sizeof(struct lock_event), nrecords, trace_file);
  assert(ret > 0);
  printf("flush done... ret = %d, nrecords= %d\n", ret, nrecords);
  nrecords -= BUFFER_SIZE;
  next_event -= BUFFER_SIZE;
}

void record_event(enum event_type event_type,
		  enum intercepted_function f,
		  void* ptr) {

  int event_index = next_event++;

  while(event_index >= BUFFER_SIZE) {
    if(event_index == BUFFER_SIZE) {
      flush_buffer();
    } else {
      while(next_event > BUFFER_SIZE) sched_yield();
    }
    
    event_index -= BUFFER_SIZE;
  }
  
  if(event_index < BUFFER_SIZE) {
    struct lock_event *event = &buffer[event_index];
    event->function=f;
    event->event_type = event_type;
    struct timespec timestamp;
    clock_gettime(CLOCK_MONOTONIC, &timestamp);
    if(first_timestamp.tv_sec == 0 && first_timestamp.tv_nsec == 0) {
      first_timestamp.tv_sec = timestamp.tv_sec;
      first_timestamp.tv_nsec = timestamp.tv_nsec;
    }
    event->timestamp = TIME_DIFF(first_timestamp, timestamp);

    event->ptr = ptr;
    event->tid = pthread_self();
    nrecords++;
    //    printf("record event %d / %d\n", event_index, nrecords);
  }
}

void enter_function(enum intercepted_function f, void* ptr) {
  DEBUG_PRINTF("Entering %s\n", function_names[f]);
  assert(trace_file);
  nb_calls[f]++;

  record_event(function_entry, f, ptr);
}

void leave_function(enum intercepted_function f, void* ptr) {
  DEBUG_PRINTF("Leaving %s\n", function_names[f]);
  assert(trace_file);

  record_event(function_exit, f, ptr);
}

static void __write_events_init(void) __attribute__((constructor));
static void __write_events_init(void) {
  DEBUG_PRINTF("[LIBLOCK] initializing write_events\n");
  for(int i=0; i<NB_FUNCTIONS; i++) {
    nb_calls[i]=0;
  }

  trace_file=fopen("trace.dat", "w");
  assert(trace_file);
}

static void __write_events_conclude(void) __attribute__((destructor));
static void __write_events_conclude(void) {
  DEBUG_PRINTF("[LIBLOCK] finalizing write_events\n");

  for(int i=0; i<NB_FUNCTIONS; i++) {
    if(nb_calls[i]!=0) {
      printf("%s: %d\n", function_names[i], nb_calls[i]);
    }
  }

  flush_buffer();

  assert(trace_file);
  fclose(trace_file);
}

