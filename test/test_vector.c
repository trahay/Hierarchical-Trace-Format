/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include <stdlib.h>
#include "htf/htf_linked_vector.h"
#include "htf/htf.h"
#include "htf/htf_dbg.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    htf_error("Not enough arguments ! 1 argument required.\n");
  }
  if (argc > 2) {
    htf_error("Too many arguments ! 1 argument required.\n");
  }
  int TEST_SIZE = atoi(argv[1]);

  LinkedVector* vector = linked_vector_new();

  DOFOR(i, TEST_SIZE) {
    linked_vector_add(vector, i);
  }

  htf_assert(vector->size == (size_t)TEST_SIZE);
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
