#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <pthread.h>

#include "tracer.h"

int (*pthread_mutex_lock_original)(pthread_mutex_t *mutex);
int (*pthread_mutex_trylock_original)(pthread_mutex_t *mutex);
int (*pthread_mutex_unlock_original)(pthread_mutex_t *mutex);
int (*pthread_mutex_init_original)(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int (*pthread_mutex_destroy_original)(pthread_mutex_t *mutex);


void init_callbacks() {
  pthread_mutex_lock_original = get_callback("pthread_mutex_lock");
  pthread_mutex_trylock_original = get_callback("pthread_mutex_trylock");
  pthread_mutex_unlock_original = get_callback("pthread_mutex_unlock");
  pthread_mutex_init_original = get_callback("pthread_mutex_init");
  pthread_mutex_destroy_original = get_callback("pthread_mutex_destroy");
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
  if(!pthread_mutex_lock_original) init_callbacks();
  enter_function(mutex_lock, mutex);
  int ret = pthread_mutex_lock_original(mutex);
  leave_function(mutex_lock, mutex);
  return ret;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
  if(!pthread_mutex_trylock_original) init_callbacks();

  enter_function(mutex_trylock, mutex);
  int ret = pthread_mutex_trylock_original(mutex);
  leave_function(mutex_unlock, mutex);
  return ret;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex){
  if(!pthread_mutex_unlock_original) init_callbacks();

  enter_function(mutex_unlock, mutex);
  int ret = pthread_mutex_unlock_original(mutex);
  leave_function(mutex_unlock, mutex);
  return ret;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr){
  if(!pthread_mutex_init_original) init_callbacks();

  enter_function(mutex_init, mutex);
  int ret = pthread_mutex_init_original(mutex, attr);
  leave_function(mutex_init, mutex);
  return ret;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex){
  if(!pthread_mutex_destroy_original) init_callbacks();

  enter_function(mutex_destroy, mutex);
  int ret = pthread_mutex_destroy_original(mutex);
  leave_function(mutex_destroy, mutex);
  return ret;
}

static void __mutex_init(void) __attribute__((constructor));
static void __mutex_init(void) {
  DEBUG_PRINTF("[LIBLOCK] initializing the mutex library\n");
  init_callbacks();
}

static void __mutex_conclude(void) __attribute__((destructor));
static void __mutex_conclude(void) {
  DEBUG_PRINTF("[LIBLOCK] finalizing the mutex library\n");
}
