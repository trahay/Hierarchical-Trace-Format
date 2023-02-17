#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "tracer.h"
#include "trace_storage.h"
#include "event.h"

#if 0
struct event_data *event_data = NULL;
int nb_event_data = 0;

void load_event_data(const char* filename) {
  int fd = open(filename, O_RDONLY);
  assert(fd>0);
  struct stat s;
  fstat(fd, &s);
  size_t file_size = s.st_size;
  event_data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if(event_data == MAP_FAILED) {
    fprintf(stderr, "Error: cannot mmap file %s\n", filename);
    abort();
  }

  nb_event_data =file_size / sizeof(struct event_data);
  close(fd); 

  printf("There are %d data events:\n", nb_event_data);
  for(int i=0; i<nb_event_data; i++) {
    struct event_data *data = &event_data[i];
    printf("[%d]  {.ptr=%p, .tid=%lx, .func=%d, .event_type=%d}\n", i,
	   data->ptr, data->tid, data->function, data->event_type);

  }
}

struct event_data* get_data(event_data_id data_id) {
  
  if(data_id > nb_event_data) {
    fprintf(stderr, "error: looking for data %d, but there are only %d data\n", data_id, nb_event_data);
    abort();
  }

  return &event_data[data_id];
}
#endif

#define ET2C(et) (((et) == function_entry? 'E':	\
    (et)==function_exit? 'L':			\
		   'S'))
void print_event(struct event_occurence *e) {
  int id = e->event.function_id;
  printf("%lf\t%c\t%d (%s)\n", e->timestamp/1e9, ET2C(e->event.event_type), id, function_names[id]);
}

void print_thread_trace(struct trace *trace, int thread_index) {
  printf("Events for thread %d:\n", thread_index);

  struct thread_trace_reader reader;
  thread_trace_reader_init(&reader, trace, thread_index);
  struct event_occurence e;

  while(thread_trace_reader_next_event(&reader, &e) == 0) {
    print_event(&e);
  }

}

int main(int argc, char**argv) {
  if(argc!=2) {
    printf("usage: %s trace\n", argv[0]);
    return EXIT_FAILURE;
  }
#if 0
  
  int fd = open(argv[1], O_RDONLY);
  assert(fd>0);
  load_event_data(argv[2]);
  struct event e;
  while(read(fd, &e, sizeof(e)) > 0) {
    struct event_data* data = get_data(e.data_id);
    if(data->event_type == function_entry)
      printf("[%lx] %lf\tenter %s(%p)\n", data->tid, e.timestamp/1e9, function_names[data->function], data->ptr);
    else
      printf("[%lx] %lf\texit %s(%p)\n", data->tid, e.timestamp/1e9, function_names[data->function], data->ptr);
  }
  close(fd);
#endif

  struct trace trace;
  read_trace(&trace, argv[1]);

  for(int i=0; i<trace.nb_threads; i++) {
    print_thread_trace(&trace, i);
  }

  return EXIT_SUCCESS;
}
