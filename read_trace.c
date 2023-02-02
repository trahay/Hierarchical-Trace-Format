#define _GNU_SOURCE

#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "liblock.h"

int main(int argc, char**argv) {
  if(argc!=2) {
    printf("usage: %s trace\n", argv[0]);
    return EXIT_FAILURE;
  }

  int fd = open(argv[1], O_RDONLY);
  assert(fd>0);

  struct lock_event e;
  while(read(fd, &e, sizeof(e)) > 0) {
    if(e.event_type == function_entry)
      printf("[%lx] %lf\tenter %s(%p)\n", e.tid, e.timestamp/1e9, function_names[e.function], e.ptr);
    else
      printf("[%lx] %lf\texit %s(%p)\n", e.tid, e.timestamp/1e9, function_names[e.function], e.ptr);
  }
  close(fd);
  return EXIT_SUCCESS;
}
