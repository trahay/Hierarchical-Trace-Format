#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

#include "htf.h"
#include "htf_event.h"
#include "htf_write.h"

static struct htf_archive   trace;
static htf_location_group_id_t   process_id;

static int nb_iter_default = 100000;
static int nb_functions_default = 2;
static int nb_threads_default = 4;
static int pattern_default = 0;

static int nb_iter;
static int nb_functions;
static int nb_threads;
static int pattern;

struct htf_thread_writer **thread_writers;
static htf_region_ref_t *regions;
static htf_string_ref_t *strings;

static pthread_barrier_t bench_start;
static pthread_barrier_t bench_stop;

#define TIME_DIFF(t1, t2) (((t2).tv_sec-(t1).tv_sec) + ((t2).tv_nsec-(t1).tv_nsec)/1e9)

static htf_region_ref_t next_region_ref = 0;
static htf_string_ref_t next_string_ref = 0;
static _Atomic htf_thread_id_t next_thread_id = 0;

static htf_string_ref_t _register_string(char* str) {
  htf_string_ref_t ref = next_string_ref++;
  htf_archive_register_string(&trace, ref, str);
  return ref;
}

static htf_region_ref_t _register_region(htf_string_ref_t string_ref) {
  htf_region_ref_t id = next_region_ref++;
  htf_archive_register_region(&trace, id, string_ref);
  return id;
}

static htf_thread_id_t _new_thread() {
  htf_thread_id_t id = next_thread_id++;
  return id;
}

static int mpi_rank;
static int mpi_comm_size;

void* worker(void* arg __attribute__((unused))) {
  static _Atomic int nb_threads = 0;
  int my_rank = nb_threads++;

  thread_writers[my_rank] =  malloc(sizeof(struct htf_thread_writer));
  struct htf_thread_writer *thread_writer = thread_writers[my_rank];
  char thread_name[20];
  snprintf(thread_name, 20, "P#%dT#%d", mpi_rank, my_rank);
  htf_string_ref_t thread_name_id = _register_string(thread_name);

  htf_thread_id_t thread_id = _new_thread();
  htf_write_define_location(&trace,
			    thread_id,
			    thread_name_id,
			    process_id);

  htf_write_thread_open(&trace, thread_writer, thread_id);
 
  struct timespec t1, t2;
  pthread_barrier_wait(&bench_start);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  for(int i=0; i<nb_iter; i++) {
    switch(pattern) {
    case 0:
      /* each iteration is a sequence of functions:
       * E_f1 L_f1 E_f2 L_f2 E_f3 L_f3 ...
       */
      for(int j = 0; j<nb_functions; j++) {
	htf_record_enter(thread_writer, NULL, HTF_TIMESTAMP_INVALID, regions[j]);
	htf_record_leave(thread_writer, NULL, HTF_TIMESTAMP_INVALID, regions[j]);
      }
      break;

    case 1:
      /* each iteration contains recursive function calls::
       * E_f1 E_f2 E_f3 ... L_f3 L_f2 L_f1
       */
      for(int j = 0; j<nb_functions; j++) {
	htf_record_enter(thread_writer, NULL, HTF_TIMESTAMP_INVALID, regions[j]);
      }
      for(int j = nb_functions-1; j>=0; j--) {
	htf_record_leave(thread_writer, NULL, HTF_TIMESTAMP_INVALID, regions[j]);
      }
      break;
    default:
      fprintf(stderr, "invalid pattern: %d\n", pattern);
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &t2);

  pthread_barrier_wait(&bench_stop);

  double duration = TIME_DIFF(t1, t2);
  int nb_event_per_iter = 2 * nb_functions;
  int nb_events = nb_iter * nb_event_per_iter;
  double duration_per_event = duration /nb_events;

  printf("T#%d: %d events in %lf s -> %lf ns per event\n",
	 my_rank, nb_events, duration, duration_per_event * 1e9);

  htf_write_thread_close(thread_writer);
  return NULL;
}


void usage(const char *prog_name) {
  printf("Usage: %s [OPTION] program [arg1 arg2 ...]\n", prog_name);
  printf("\t-n X    Set the number of iterations (default: %d)\n", nb_iter_default);
  printf("\t-f X    Set the number of functions (default: %d)\n", nb_functions_default);
  printf("\t-t X    Set the number of threads (default: %d)\n", nb_threads_default);
  printf("\t-p X    Select the event pattern\n");
  printf("\t-? -h   Display this help and exit\n");
}

int main(int argc, char**argv) {
  int nb_opts = 0; // options

  nb_iter = nb_iter_default;
  nb_functions = nb_functions_default;
  nb_threads= nb_threads_default;
  pattern = pattern_default;

  for(int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-n")) {
      nb_iter = atoi(argv[i+1]);
      nb_opts+=2; i++;
    } else if (!strcmp(argv[i], "-f")) {
      nb_functions = atoi(argv[i+1]);
      nb_opts+=2; i++;
    } else if (!strcmp(argv[i], "-t")) {
      nb_threads = atoi(argv[i+1]);
      nb_opts+=2; i++;
    } else if (!strcmp(argv[i], "-p")) {
      pattern = atoi(argv[i+1]);
      nb_opts+=2; i++;
    } else if (!strcmp(argv[i], "-?") || !strcmp(argv[i], "-h")) {
      usage(argv[0]);
      return EXIT_SUCCESS;
    } else {
      fprintf(stderr, "invalid option: %s\n", argv[i]);
      usage(argv[0]);
      return EXIT_FAILURE;
    }
  }

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_comm_size);

  printf("Hello from %d/%d\n", mpi_rank, mpi_comm_size);

  process_id = mpi_rank;

  int chunk_size = mpi_comm_size * 100;
  next_region_ref = mpi_rank * chunk_size;
  next_string_ref = mpi_rank * chunk_size;
  next_thread_id = mpi_rank * chunk_size;

  
  pthread_t tid[nb_threads];
  pthread_barrier_init(&bench_start, NULL, nb_threads+1);
  pthread_barrier_init(&bench_stop, NULL, nb_threads+1);
  thread_writers = malloc(sizeof(struct htf_thread_writer*) * nb_threads);

  printf("nb_iter = %d\n", nb_iter);
  printf("nb_functions = %d\n", nb_functions);
  printf("nb_threads = %d\n", nb_threads);
  printf("pattern = %d\n", pattern);
  printf("---------------------\n");

  htf_write_archive_open(&trace,
			 "mpi_benchmark_trace",
			 "main",
			 mpi_rank);

  regions = malloc(sizeof(htf_region_ref_t) * nb_functions);
  strings = malloc(sizeof(htf_string_ref_t) * nb_functions);
  for(int i=0; i<nb_functions; i++) {
    char str[50];
    snprintf(str, 50, "function_%d", i);
    strings[i] = _register_string(str);
    regions[i] = _register_region(strings[i]);
  }
  
  for(int i=0; i<nb_threads; i++)
    pthread_create(&tid[i], NULL, worker, NULL);

  struct timespec t1, t2;
  pthread_barrier_wait(&bench_start);

  clock_gettime(CLOCK_MONOTONIC, &t1);
  pthread_barrier_wait(&bench_stop);
  clock_gettime(CLOCK_MONOTONIC, &t2);

  for(int i=0; i<nb_threads; i++)
    pthread_join(tid[i], NULL);

  double duration = TIME_DIFF(t1, t2);
  int nb_event_per_iter = 2 * nb_functions;
  int nb_events = nb_iter * nb_event_per_iter * nb_threads;
  double events_per_second = nb_events / duration;

  printf("TOTAL: %d events in %lf s -> %lf Me/s \n",
	 nb_events, duration, events_per_second/1e6);

  htf_write_archive_close(&trace);

  if(mpi_rank==0) {
    struct htf_global_archive global_archive;
    htf_write_global_archive_open(&global_archive,
				  "mpi_benchmark_trace",
				  "main");

    for(int i = 0; i< mpi_comm_size; i++) {
      char rank_name_str[100];
      snprintf(rank_name_str, 100, "Rank#%d", i);
      htf_string_ref_t rank_name = _register_string(rank_name_str);
      htf_write_define_location_group(&trace,
				      i,
				      rank_name,
				      HTF_LOCATION_GROUP_ID_INVALID);
    }

    htf_write_global_archive_close(&global_archive);   
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}
