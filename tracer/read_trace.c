#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "tracer.h"
#include "htf.h"


#define ET2C(et) (((et) == function_entry? 'E':	\
    (et)==function_exit? 'L':			\
		   'S'))
void print_event(struct event_occurence *e) {
  static timestamp_t last_ts = 0;
  int id = e->event.function_id;
  printf("%.9lf\t%.9lf\t%c\t%d (%s)\n", e->timestamp/1e9, (e->timestamp-last_ts)/1e9, ET2C(e->event.event_type), id, function_names[id]);
  last_ts = e->timestamp;
}

void print_thread_trace(struct trace *trace, int thread_index) {
  printf("Events for thread %d:\n", thread_index);

  struct thread_trace_reader reader;
  htf_read_thread_iterator_init(&reader, trace, thread_index);
  struct event_occurence e;

  while(htf_read_thread_next_event(&reader, &e) == 0) {
    print_event(&e);
  }

}

int main(int argc, char**argv) {
  if(argc!=2) {
    printf("usage: %s trace\n", argv[0]);
    return EXIT_FAILURE;
  }

  struct trace trace;
  htf_read_trace(&trace, argv[1]);

  for(int i=0; i<trace.nb_threads; i++) {
    print_thread_trace(&trace, i);
  }

  return EXIT_SUCCESS;
}
