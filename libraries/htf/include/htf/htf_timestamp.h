/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
/** @file
 * Functions to handle timestamps.
 */
#pragma once

#include "htf/htf_dbg.h"

#ifdef __cplusplus
#include <chrono>
#include <cstdint>
#include <limits>
typedef std::chrono::time_point<std::chrono::high_resolution_clock> Timepoint;
#else
#include <limits.h>
#include <stdint.h>
#endif
typedef uint64_t htf_timestamp_t;
#define HTF_TIMESTAMP_INVALID UINT64_MAX

typedef uint64_t htf_duration_t;
#define HTF_DURATION_INVALID UINT64_MAX

/** return the time difference of two events. */
inline htf_duration_t htf_get_duration(htf_timestamp_t t1, htf_timestamp_t t2) {
  return t2 - t1;
}