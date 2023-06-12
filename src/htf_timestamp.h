#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <stdint.h>
#include <limits.h>

typedef uint64_t htf_timestamp_t;

#define HTF_TIMESTAMP_INVALID UINT64_MAX

/** return the current timestamp */
htf_timestamp_t htf_get_timestamp(void);

/** return t, or the current timestamp if t is invalid*/
htf_timestamp_t htf_timestamp(htf_timestamp_t t);

/** Writes the difference between the given timestamp and the last timestamp to the adress of the last timestamp,
 * and then stores the given adress. */
void htf_delta_timestamp(htf_timestamp_t* t);

/** Writes 0 to the last timestamp */
void htf_finish_timestamp(void);
#endif /* TIMESTAMP_H */
