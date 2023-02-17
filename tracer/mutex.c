#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include <pthread.h>

#include "liblock.h"

void enter_function(enum intercepted_function f, void* ptr) {

  if(trace == NULL)
    init_trace();
  if(thread_trace == NULL)
    _init_thread();

  int index = thread_trace->nb_tokens;
  printf("Entering %s (%d)\n", function_names[f], index);
  fflush(stdout);
  record_event(function_entry, f, ptr);
  assert( thread_trace->nb_tokens > index);
  
}

void leave_function(enum intercepted_function f, void* ptr) {
  if(recursion_shield)
    return;
  recursion_shield++;

  printf("Leaving %s\n", function_names[f]);
  fflush(stdout);
  record_event(function_exit, f, ptr);

  recursion_shield--;
}


int (*pthread_mutex_lock_original)(pthread_mutex_t *mutex);
int (*pthread_mutex_trylock_original)(pthread_mutex_t *mutex);
int (*pthread_mutex_unlock_original)(pthread_mutex_t *mutex);
int (*pthread_mutex_init_original)(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int (*pthread_mutex_destroy_original)(pthread_mutex_t *mutex);

int pthread_mutex_lock(pthread_mutex_t *mutex) {
  enter_function(mutex_lock, mutex);
  int ret = pthread_mutex_lock_original(mutex);
  leave_function(mutex_lock, mutex);
  return ret;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
  enter_function(mutex_trylock, mutex);
  int ret = pthread_mutex_trylock_original(mutex);
  leave_function(mutex_unlock, mutex);
  return ret;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex){
  enter_function(mutex_unlock, mutex);
  int ret = pthread_mutex_unlock_original(mutex);
  leave_function(mutex_unlock, mutex);
  return ret;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr){
  enter_function(mutex_init, mutex);
  int ret = pthread_mutex_init_original(mutex, attr);
  leave_function(mutex_init, mutex);
  return ret;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex){
  enter_function(mutex_destroy, mutex);
  int ret = pthread_mutex_destroy_original(mutex);
  leave_function(mutex_destroy, mutex);
  return ret;
}


static void __mutex_init(void) __attribute__((constructor));
static void __mutex_init(void) {
  DEBUG_PRINTF("[LIBLOCK] initializing the mutex library\n");

  pthread_mutex_lock_original = get_callback("pthread_mutex_lock");
  pthread_mutex_trylock_original = get_callback("pthread_mutex_trylock");
  pthread_mutex_unlock_original = get_callback("pthread_mutex_unlock");
  pthread_mutex_init_original = get_callback("pthread_mutex_init");
  pthread_mutex_destroy_original = get_callback("pthread_mutex_destroy");
}

static void __mutex_conclude(void) __attribute__((destructor));
static void __mutex_conclude(void) {
  DEBUG_PRINTF("[LIBLOCK] finalizing the mutex library\n");
}
