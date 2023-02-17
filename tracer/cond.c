#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <assert.h>

#include <pthread.h>

#include "tracer.h"

int (*pthread_cond_wait_original)(pthread_cond_t *cond, pthread_mutex_t *mutex);
int (*pthread_cond_timedwait_original)(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
int (*pthread_cond_signal_original)(pthread_cond_t *cond);
int (*pthread_cond_broadcast_original)(pthread_cond_t *cond);
int (*pthread_cond_init_original)(pthread_cond_t *cond, const pthread_condattr_t *attr);
int (*pthread_cond_destroy_original)(pthread_cond_t *cond);

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
  enter_function(cond_wait, cond);
  int ret = pthread_cond_wait_original(cond, mutex);
  leave_function(cond_wait, cond);
  return ret;
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime) {
  enter_function(cond_timedwait, cond);
  int ret = pthread_cond_timedwait_original(cond, mutex, abstime);
  leave_function(cond_timedwait, cond);
  return ret;
}

int pthread_cond_signal(pthread_cond_t *cond) {
  enter_function(cond_signal, cond);
  int ret = pthread_cond_signal_original(cond);
  leave_function(cond_signal, cond);
  return ret;
}

int pthread_cond_broadcast(pthread_cond_t *cond) {
  enter_function(cond_broadcast, cond);
  int ret = pthread_cond_broadcast_original(cond);
  leave_function(cond_broadcast, cond);
  return ret;
}

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
  enter_function(cond_init, cond);
  int ret = pthread_cond_init_original(cond, attr);
  leave_function(cond_init, cond);
  return ret;
}

int pthread_cond_destroy(pthread_cond_t *cond) {
  enter_function(cond_destroy, cond);
  int ret = pthread_cond_destroy_original(cond);
  leave_function(cond_destroy, cond);
  return ret;
}

static void __cond_init(void) __attribute__((constructor));
static void __cond_init(void) {
  DEBUG_PRINTF("[LIBLOCK] initializing the cond library\n");

  pthread_cond_wait_original = get_callback("pthread_cond_wait");
  pthread_cond_timedwait_original = get_callback("pthread_cond_timedwait");
  pthread_cond_signal_original = get_callback("pthread_cond_signal");
  pthread_cond_broadcast_original = get_callback("pthread_cond_broadcast");
  pthread_cond_init_original = get_callback("pthread_cond_init");
  pthread_cond_destroy_original = get_callback("pthread_cond_destroy");
}

static void __cond_conclude(void) __attribute__((destructor));
static void __cond_conclude(void) {
  DEBUG_PRINTF("[LIBLOCK] finalizing the cond library\n");
}
