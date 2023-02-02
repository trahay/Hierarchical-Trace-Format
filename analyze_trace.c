#define _GNU_SOURCE

#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include <pthread.h>
#include <sys/mman.h>
#include "liblock.h"

#define NB_THREAD_MAX 128

/* statistics on a function */
struct function_stats {
  int nb_calls;
  uint64_t total_duration;
  uint64_t min_duration;
  uint64_t max_duration;
};

/* statistics on a lock */
struct lock_stat {
  void* ptr;
  _Atomic int nb_lock;
  _Atomic uint64_t total_duration;
  _Atomic uint64_t min_duration;
  _Atomic uint64_t max_duration;
};

/* statistics on a thread */
struct thread_info {
  pthread_t tid;
  int nb_events;
  struct lock_event last_event;
  int pipefd[2];
  pthread_t tid_worker;
  uint64_t min_timestamp;
  uint64_t max_timestamp;
};

#define UPDATE_OP(cur_value, value, OP) if((cur_value) OP (value)) (value) = (cur_value)
#define UPDATE_MIN(cur_value, min_value) UPDATE_OP(cur_value, min_value, <)
#define UPDATE_MAX(cur_value, min_value) UPDATE_OP(cur_value, min_value, >)

struct function_stats f_stats[NB_FUNCTIONS];
struct lock_stat *locks = NULL;
int nb_locks = 0;
struct thread_info threads[NB_THREAD_MAX];
int nb_threads = 0;
pthread_rwlock_t lock_mutex;

struct lock_event *events = NULL;

void* thread_function(void*arg);

void __update_lock_stats(struct lock_stat* lock, uint64_t duration) {
  lock->nb_lock++;
  lock->total_duration += duration;
  UPDATE_MIN(duration, lock->min_duration);
  UPDATE_MAX(duration, lock->max_duration);
}

void update_lock_stats(void* ptr, uint64_t duration) {
  pthread_rwlock_rdlock(&lock_mutex);
  for(int i=0; i<nb_locks; i++) {
    if(locks[i].ptr == ptr) {
      __update_lock_stats(&locks[i], duration);
      pthread_rwlock_unlock(&lock_mutex);
      return ;
    }
  }

  pthread_rwlock_unlock(&lock_mutex);
  pthread_rwlock_wrlock(&lock_mutex);
  for(int i=0; i<nb_locks; i++) {
    if(locks[i].ptr == ptr) {
      __update_lock_stats(&locks[i], duration);
      pthread_rwlock_unlock(&lock_mutex);
      return ;
    }
  }


  /* lock not found, register it */
  int index = nb_locks++;
  locks = realloc(locks, sizeof(struct lock_stat)*nb_locks);
  assert(locks);
  locks[index].ptr = ptr;
  locks[index].nb_lock = 0;
  locks[index].total_duration = 0;
  locks[index].min_duration = UINT64_MAX;
  locks[index].max_duration = 0;

  __update_lock_stats(&locks[index], duration);

  pthread_rwlock_unlock(&lock_mutex);
}

struct thread_info* get_thread_info(pthread_t tid) {
  for(int i=0; i<nb_threads; i++)
    if(threads[i].tid == tid)
      return &threads[i];

  /* thread not found, register it */
  int index = nb_threads++;
  assert(index < NB_THREAD_MAX);
  threads[index].tid = tid;
  threads[index].nb_events = 0;
  threads[index].last_event.function = NB_FUNCTIONS;

  threads[index].min_timestamp = UINT64_MAX;
  threads[index].max_timestamp = 0;
  pipe(threads[index].pipefd);
  int ret = pthread_create(&threads[index].tid_worker, NULL, thread_function, &threads[index]);
  assert (ret == 0);
  return &threads[index];
}

void process_event(struct thread_info* info, struct lock_event *e) {
  UPDATE_MIN(e->timestamp, info->min_timestamp);
  UPDATE_MAX(e->timestamp, info->max_timestamp);
    
  info->nb_events++;
  if(e->event_type == function_entry) {
    memcpy(&info->last_event, e, sizeof(struct lock_event));
  } else {
    assert(info->last_event.function == e->function);

    /* update function stats */
    f_stats[e->function].nb_calls++;
    uint64_t function_duration =  e->timestamp - info->last_event.timestamp;
    f_stats[e->function].total_duration += function_duration;

    UPDATE_MIN(function_duration, f_stats[e->function].min_duration);
    UPDATE_MAX(function_duration, f_stats[e->function].max_duration);

    /* update lock stats */
    if(e->function == mutex_lock ||
       e->function == mutex_trylock ||
       e->function == cond_wait ||
       e->function == cond_timedwait) {
      update_lock_stats(e->ptr, function_duration);
    }
  }
}


void* thread_function(void*arg) {
  struct thread_info *info = arg;
  printf("New thread for tid %lx\n", info->tid);

  while(1) {
    int index;
    int ret = read(info->pipefd[0], &index, sizeof(index));
    if(index < 0 || ret <=0 )
      break;
    process_event(info, &events[index]);
  }
  printf("End of thread %lx\n", info->tid);
  return NULL;
}


int main(int argc, char**argv) {
  if(argc != 2) {
    printf("usage: %s trace\n", argv[0]);
    return EXIT_FAILURE;
  }

  for(int i=0; i<NB_FUNCTIONS; i++) {
    f_stats[i].nb_calls=0;
    f_stats[i].total_duration = 0;
    f_stats[i].min_duration = UINT64_MAX;
    f_stats[i].max_duration = 0;
  }

  int fd = open(argv[1], O_RDONLY);
  assert(fd>0);
#if 1
  size_t file_length=lseek(fd, 0, SEEK_END);
  int nb_events = file_length/sizeof(struct lock_event);

  events = mmap(NULL, file_length, PROT_READ, MAP_PRIVATE, fd, 0);
  assert(events != MAP_FAILED);

  for(int i=0; i<nb_events; i++) {
    struct lock_event *e = &events[i];
    struct thread_info*  thread = get_thread_info(e->tid);
    write(thread->pipefd[1], &i, sizeof(int));
  }

#else
  struct lock_event e;
  while(read(fd, &e, sizeof(e)) > 0) {
    process_event(&e);
  }

#endif
  close(fd);

  uint64_t min_timestamp = UINT64_MAX;
  uint64_t max_timestamp = 0;

  for(int i = 0; i<nb_threads; i++) {
    struct thread_info* thread = &threads[i];
    int value = -1;
    write(thread->pipefd[1], &value, sizeof(int));
    pthread_join(thread->tid_worker, NULL);

    UPDATE_MIN(thread->min_timestamp, min_timestamp);
    UPDATE_MAX(thread->max_timestamp, max_timestamp);

    printf("thread %lx: %d events\n", threads[i].tid, threads[i].nb_events);
  }
  
  for(int i = 0; i<NB_FUNCTIONS; i++) {
    if(f_stats[i].nb_calls > 0) {
      printf("%s: %d calls (%lf us per call, min: %lf us, max: %lf us\n",
	     function_names[i], f_stats[i].nb_calls,
	     (1e-3*f_stats[i].total_duration) / f_stats[i].nb_calls,
	     1e-3*f_stats[i].min_duration,
	     1e-3*f_stats[i].max_duration);
    }
  }

  uint64_t app_duration=max_timestamp - min_timestamp;
  for(int i = 0; i<nb_locks; i++) {
    uint64_t contention_duration = locks[i].total_duration-(locks[i].nb_lock*locks[i].min_duration);
    float contention_percent = (double)contention_duration / app_duration;

    printf("lock %p: %d calls (%lf us per call, min: %lf us, max: %lf us). contention score: %f %%\n",
	   locks[i].ptr, locks[i].nb_lock,
	   1e6*locks[i].total_duration / locks[i].nb_lock,
	   1e-3*locks[i].min_duration, locks[i].max_duration*1e-3,
	   contention_percent);
  }

  return EXIT_SUCCESS;
}
