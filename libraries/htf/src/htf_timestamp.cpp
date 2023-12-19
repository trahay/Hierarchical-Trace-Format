/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
#include "htf/htf_timestamp.h"
#include "htf/htf_write.h"

#define NANOSECONDS(timestamp) std::chrono::duration_cast<std::chrono::nanoseconds>(timestamp).count()

htf_timestamp_t htf::ThreadWriter::getTimestamp() {
  Timepoint start = std::chrono::high_resolution_clock::now();
  if (NANOSECONDS(firstTimestamp.time_since_epoch()) == 0) {
    firstTimestamp = start;
  }
  return NANOSECONDS(start - firstTimestamp);
}

htf_timestamp_t htf::ThreadWriter::timestamp(htf_timestamp_t t) {
  if (t == HTF_TIMESTAMP_INVALID)
    return getTimestamp();
  return t;
}

void htf::ThreadWriter::completeDurations(htf_duration_t duration) {
  for (auto it : incompleteDurations) {
    *it += duration;
  }
  incompleteDurations.resize(0);
}

void htf::ThreadWriter::addDurationToComplete(htf_duration_t* duration) {
  incompleteDurations.push_back(duration);
}

// void htf_finish_timestamp() {
//   *timestampsToDelta.front() = 0;
// }

/* -*-
   mode: cpp;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
