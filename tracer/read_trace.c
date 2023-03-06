#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#include "tracer.h"
#include "htf.h"


#define ET2C(et) (((et) == function_entry? 'E':	\
		   (et)==function_exit? 'L':	\
		   'S'))

/* Print one event */
static void print_event(struct event_occurence *e, int thread_index) {
  static int first_time = 1;
  
  int id = e->event.function_id;
  if(first_time == 1) {
    printf("#timestamp\tthread_index\tevent_type\tevent_id\tevent_name\n");
    first_time = 0;
  }
  printf("%.9lf\t%d\t%c\t%d (%s)\n", e->timestamp/1e9, thread_index, ET2C(e->event.event_type),
	 id, function_names[id]);
}

/* Print all the events of a thread */
static void print_thread_trace(struct trace *trace, int thread_index) {
  printf("Reading events for thread %d:\n", thread_index);

  struct thread_reader reader;
  htf_read_thread_iterator_init(&reader, trace, thread_index);
  struct event_occurence e;

  while(htf_read_thread_next_event(&reader, &e) == 0) {
    print_event(&e, thread_index);
  }
}

/* compare the timestamps of the current event on each thread and select the smallest timestamp
 * This fills the event_occurence e and returns the index of the selected thread (or -1 at the end
 * of the trace)
 */
static int get_next_event(struct thread_reader *readers,
			  int nb_threads,
			  struct event_occurence *e) {

  struct event_occurence cur_e;
  timestamp_t min_ts = TIMESTAMP_INVALID;
  int min_index = -1;

  for(int i=0; i< nb_threads; i++) {
    if(htf_read_thread_cur_event(&readers[i], &cur_e) == 0) {
      if( min_ts == TIMESTAMP_INVALID || min_ts > cur_e.timestamp) {
	min_index = i;
	min_ts = cur_e.timestamp;
      }
    }
  }

  if(min_index>=0) {
    htf_read_thread_next_event(&readers[min_index], e);
  }

  return  min_index;
}

/* Print all the events of all the threads sorted by timestamp */
void print_trace(struct trace *trace) {
  struct thread_reader *readers = malloc(sizeof(struct thread_reader)* trace->nb_threads);
  for(int i=0; i< trace->nb_threads; i++) {
    htf_read_thread_iterator_init(&readers[i], trace, i);
  }

  struct event_occurence e;
  int thread_index = -1;
  while((thread_index = get_next_event(readers, trace->nb_threads, &e)) >= 0) {
    print_event(&e, thread_index);
  }
}

void usage(const char *prog_name) {
  printf("Usage: %s [OPTION] trace_file\n", prog_name);
  printf("\t-T          Print events per thread\n");
  printf("\t-v          Verbose mode\n");
  printf("\t-?  -h      Display this help and exit\n");
}

int main(int argc, char**argv) {
  int per_thread = 0;
  int nb_opts = 0;
  char* trace_name = NULL;

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-v")) {
      htf_debug_level_set(dbg_lvl_verbose);
      nb_opts++;
    } else if (!strcmp(argv[i], "-T")) {
      per_thread = 1;
      nb_opts++;
    } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-?") ) {
      usage(argv[0]);
      return EXIT_SUCCESS;
    } else {
      /* Unknown parameter name. It's probably the program name. We can stop
       * parsing the parameter list.
       */
      break;
    }
  }

  trace_name = argv[nb_opts + 1];
  if (trace_name == NULL) {
    usage(argv[0]);
    return EXIT_SUCCESS;
  }

  struct trace trace;
  htf_read_trace(&trace, trace_name);

  if(per_thread) {
    for(int i=1; i<trace.nb_threads; i++) {
      print_thread_trace(&trace, i);
    }
  } else {
    print_trace(&trace);
  }

  return EXIT_SUCCESS;
}
