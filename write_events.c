#include "liblock.h"
#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

_Atomic int nb_calls[NB_FUNCTIONS];
FILE* events_file=NULL;
FILE* event_data_file=NULL;

int verbose=0;

#define BUFFER_SIZE 1000000
struct event buffer[BUFFER_SIZE];
struct event_data event_data_buffer[BUFFER_SIZE];

_Atomic uint32_t next_data = 0;
_Atomic int next_event = 0;
_Atomic int nrecords=0;

#define TIME_DIFF(t1, t2) (t2.tv_sec - t1.tv_sec)*1000000000 + (t2.tv_nsec - t1.tv_nsec)
struct timespec first_timestamp={0, 0};

void flush_buffer() {
  printf("About to flush... nrecords=%d, buffer_size=%d\n", nrecords, BUFFER_SIZE);
  int ret = fwrite(buffer, sizeof(struct event), nrecords, events_file);
  assert(ret > 0);
  printf("flush done... ret = %d, nrecords= %d\n", ret, nrecords);
  nrecords -= BUFFER_SIZE;
  next_event -= BUFFER_SIZE;
}

void flush_data_buffer() {
  printf("About to flush events data...nrecords=%d, buffer_size=%d\n", next_data, BUFFER_SIZE);
  int ret = fwrite(event_data_buffer, sizeof(struct event_data), next_data, event_data_file);
  assert(ret > 0);
  printf("flush done... ret = %d, nrecords= %d\n", ret, next_data);
}

event_data_id get_data_id(struct event_data *data) {
  if(verbose)
    printf("Searching for {.ptr=%p, .tid=%lx, .func=%d, .event_type=%d}\n", data->ptr, data->tid, data->function, data->event_type);

  for(event_data_id i = 0; i < next_data; i++) {
    if(memcmp(data, &event_data_buffer[i], sizeof(struct event_data)) == 0) {
      if(verbose)
	printf("\t found with id=%d\n", i);
      return i;
    }
  }

  if(next_data >= BUFFER_SIZE) {
    fprintf(stderr, "error: too many event data!\n");
    abort();
  }

  event_data_id index = next_data++;
  if(verbose)
    printf("\tNot found. Adding it with id=%d\n", index);
  memcpy(&event_data_buffer[index], data, sizeof(struct event_data));
  return index;
}


void record_event(enum event_type event_type,
		  enum intercepted_function f,
		  void* ptr) {
  struct event_data data;
  data.ptr = ptr;
  data.tid = pthread_self();
  data.function = f;
  data.event_type = event_type;

  event_data_id data_id = get_data_id(&data);

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
    struct event *event = &buffer[event_index];
    event->data_id = data_id;
    struct timespec timestamp;
    clock_gettime(CLOCK_MONOTONIC, &timestamp);
    if(first_timestamp.tv_sec == 0 && first_timestamp.tv_nsec == 0) {
      first_timestamp.tv_sec = timestamp.tv_sec;
      first_timestamp.tv_nsec = timestamp.tv_nsec;
    }
    event->timestamp = TIME_DIFF(first_timestamp, timestamp);
    nrecords++;
    //    printf("record event %d / %d\n", event_index, nrecords);
  }
}

void enter_function(enum intercepted_function f, void* ptr) {
  DEBUG_PRINTF("Entering %s\n", function_names[f]);
  nb_calls[f]++;

  record_event(function_entry, f, ptr);
}

void leave_function(enum intercepted_function f, void* ptr) {
  DEBUG_PRINTF("Leaving %s\n", function_names[f]);

  record_event(function_exit, f, ptr);
}

static void __write_events_init(void) __attribute__((constructor));
static void __write_events_init(void) {
  DEBUG_PRINTF("[LIBLOCK] initializing write_events\n");
  for(int i=0; i<NB_FUNCTIONS; i++) {
    nb_calls[i]=0;
  }

  char* verbose_str = getenv("VERBOSE");
  if(verbose_str)
    verbose = 1;
  events_file=fopen("events.dat", "w");
  assert(events_file);

  event_data_file=fopen("event_data.dat", "w");
  assert(event_data_file);
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
  flush_data_buffer();

  assert(events_file);
  assert(event_data_file);
  fclose(events_file);
  fclose(event_data_file);
}

