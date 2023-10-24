/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include <stdlib.h>
#include "htf/LinkedVector.h"
#include "htf/htf.h"
#include "htf/htf_dbg.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    htf_error("Not enough arguments ! 2 argument required.\n");
  }
  if (argc > 3) {
    htf_error("Too many arguments ! 2 argument required.\n");
  }
  long VECTOR_SIZE = strtol(argv[1], NULL, 10);
  size_t TEST_SIZE = strtol(argv[2], NULL, 10);

  LinkedVector* vector = linked_vector_new();
  int adding;
  DOFOR(i, TEST_SIZE) {
    linked_vector_add(vector, i);
  }

  htf_assert(vector->size == TEST_SIZE);
  //  htf_vector_print_as_int(&vector);
  printf("\n");
  return EXIT_SUCCESS;
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
