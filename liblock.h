#ifndef LIBLOCK_H
#define LIBLOCK_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdint.h>
//#define DEBUG 1

#if DEBUG
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...) (void) 0
#endif


static void* get_callback(const char*fname) __attribute__((unused));
static void* get_callback(const char*fname){
  void* ret = dlsym(RTLD_NEXT, fname);
  if(!ret) {
    fprintf(stderr, "Warning: cannot find %s: %s\n", fname, dlerror());
  }
  return ret;
}


enum intercepted_function
  {
   mutex_lock,
   mutex_trylock,
   mutex_unlock,
   mutex_init,
   mutex_destroy,
   cond_wait,
   cond_timedwait,
   cond_signal,
   cond_broadcast,
   cond_init,
   cond_destroy,
   NB_FUNCTIONS,
  };

static char* function_names [] __attribute__((unused)) = {
   "mutex_lock",
   "mutex_trylock",
   "mutex_unlock",
   "mutex_init",
   "mutex_destroy",
   "cond_wait",
   "cond_timedwait",
   "cond_signal",
   "cond_broadcast",
   "cond_init",
   "cond_destroy"
};

enum event_type {
   function_entry,
   function_exit,
};

struct event_data {
  void* ptr;//8
  pthread_t tid; //8
  enum intercepted_function function;
  enum event_type event_type;
};

typedef int event_data_id;

struct event {
  uint64_t timestamp;//8
  event_data_id data_id;
} __attribute__((packed));


void enter_function(enum intercepted_function f, void* ptr);
void leave_function(enum intercepted_function f, void* ptr);

#endif
