#include <time.h>
#include "timestamp.h"

#define TIME_DIFF(t1, t2) ((t2.tv_sec - t1.tv_sec)*1000000000 + (t2.tv_nsec - t1.tv_nsec))
static struct timespec first_timestamp={0, 0};

timestamp_t get_timestamp() {
    struct timespec timestamp;
    clock_gettime(CLOCK_MONOTONIC, &timestamp);
    if(first_timestamp.tv_sec == 0 && first_timestamp.tv_nsec == 0) {
      first_timestamp.tv_sec = timestamp.tv_sec;
      first_timestamp.tv_nsec = timestamp.tv_nsec;
    }
    return TIME_DIFF(first_timestamp, timestamp);
}
