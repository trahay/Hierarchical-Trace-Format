/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <limits.h>
#include <stdint.h>
#include "htf/htf_dbg.h"
typedef uint64_t htf_timestamp_t;

#define HTF_TIMESTAMP_INVALID UINT64_MAX
CXX(extern "C" {)
/** return the current timestamp */
htf_timestamp_t htf_get_timestamp();

  /** return t, or the current timestamp if t is invalid*/
htf_timestamp_t htf_timestamp(htf_timestamp_t t);

/** Writes the difference between the given timestamp and the last timestamp to the adress of the last timestamp,
 * and then stores the given adress. */
void htf_delta_timestamp(htf_timestamp_t* t);

void htf_add_timestamp_to_delta(htf_timestamp_t* t);

/** Writes 0 to the last timestamp */
  void htf_finish_timestamp();
CXX(
};)
#endif /* TIMESTAMP_H */

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
