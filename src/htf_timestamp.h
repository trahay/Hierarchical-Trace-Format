#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <stdint.h>
#include <limits.h>
typedef  uint64_t timestamp_t;

#define TIMESTAMP_INVALID UINT64_MAX

timestamp_t htf_get_timestamp();

#endif /* TIMESTAMP_H */
