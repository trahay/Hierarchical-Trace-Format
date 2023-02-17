#ifndef TRACER_H
#define TRACER_H

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdint.h>

#include "htf.h"

//#define DEBUG 1

#if DEBUG
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...) (void) 0
#endif


void* get_callback(const char *fname);
 

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

void enter_function(enum intercepted_function f, void* ptr);
void leave_function(enum intercepted_function f, void* ptr);

#endif	/* TRACER_H */
