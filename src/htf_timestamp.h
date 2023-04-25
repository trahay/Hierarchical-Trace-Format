#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <stdint.h>
#include <limits.h>

typedef  uint64_t htf_timestamp_t;

#define HTF_TIMESTAMP_INVALID UINT64_MAX

/** return the current timestamp */
htf_timestamp_t htf_get_timestamp();

/** return t, or the current timestamp if t is invalid*/
htf_timestamp_t htf_timestamp(htf_timestamp_t t);

#endif /* TIMESTAMP_H */
