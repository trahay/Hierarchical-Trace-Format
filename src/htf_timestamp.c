/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include "htf_timestamp.h"
#include <stdlib.h>
#include <time.h>

#define TIME_DIFF(t1, t2) ((t2.tv_sec - t1.tv_sec) * 1000000000 + (t2.tv_nsec - t1.tv_nsec))
static struct timespec first_timestamp = {0, 0};
_Thread_local static htf_timestamp_t* last_timestamp = NULL;
htf_timestamp_t htf_get_timestamp() {
  struct timespec timestamp;
  clock_gettime(CLOCK_MONOTONIC, &timestamp);
  if (first_timestamp.tv_sec == 0 && first_timestamp.tv_nsec == 0) {
    first_timestamp.tv_sec = timestamp.tv_sec;
    first_timestamp.tv_nsec = timestamp.tv_nsec;
  }
  return TIME_DIFF(first_timestamp, timestamp);
}

htf_timestamp_t htf_timestamp(htf_timestamp_t t) {
  if (t == HTF_TIMESTAMP_INVALID)
    return htf_get_timestamp();
  return t;
}

void htf_delta_timestamp(htf_timestamp_t* t) {
  if (last_timestamp != NULL) {
    *last_timestamp = *t - *last_timestamp;
  }
  last_timestamp = t;
}

void htf_finish_timestamp() {
  *last_timestamp = 0;
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
