/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include "htf/htf_timestamp.h"
#include <chrono>
#include <vector>

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
#define NANOSECONDS(timestamp) std::chrono::duration_cast<std::chrono::nanoseconds>(timestamp).count()

static TimePoint firstTimestamp = {};
//thread_local static std::vector<htf_timestamp_t*> timestampsToDelta = std::vector<htf_timestamp_t*>();

htf_duration_t htf_get_duration(htf_timestamp_t t1, htf_timestamp_t t2) {
  return t2 - t1;
}

htf_timestamp_t htf_get_timestamp() {
  TimePoint start = std::chrono::high_resolution_clock::now();
  if (NANOSECONDS(firstTimestamp.time_since_epoch()) == 0) {
    firstTimestamp = start;
  }
  return NANOSECONDS(start - firstTimestamp);
}

htf_timestamp_t htf_timestamp(htf_timestamp_t t) {
  if (t == HTF_TIMESTAMP_INVALID)
    return htf_get_timestamp();
  return t;
}

void htf_delta_timestamp(htf_timestamp_t* t) {
//  if (!timestampsToDelta.empty()) {
//    htf_timestamp_t* front_ts = timestampsToDelta.front();
//    htf_timestamp_t startingTimestamp = *front_ts;
//    htf_timestamp_t startingDuration = 0;
//    for (auto& timestamp : timestampsToDelta) {
//      if (!startingDuration) {
//        *timestamp = *t - *timestamp;
//        startingDuration = *timestamp;
//      } else {
//        *timestamp += startingDuration;
//        *timestamp -= startingTimestamp;
//      }
//    }
//    timestampsToDelta.clear();
//  }
//  timestampsToDelta.push_back(t);
}

//void htf_add_timestamp_to_delta(htf_timestamp_t* t) {
//  timestampsToDelta.push_back(t);
//}

//void htf_finish_timestamp() {
//  *timestampsToDelta.front() = 0;
//}

/* -*-
   mode: cpp;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
