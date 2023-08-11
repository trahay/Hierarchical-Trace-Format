/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include <stdlib.h>
#include "htf.h"
#include "htf_dbg.h"
#include "htf_vector.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    htf_error("Not enough arguments ! 2 argument required.\n");
  }
  if (argc > 3) {
    htf_error("Too many arguments ! 2 argument required.\n");
  }
  long VECTOR_SIZE = strtol(argv[1], NULL, 10);
  long TEST_SIZE = strtol(argv[2], NULL, 10);

  htf_vector_t vector;
  htf_vector_new_with_size(&vector, sizeof(int), VECTOR_SIZE);
  int adding;
  DOFOR(i, TEST_SIZE) {
    adding = i;
    htf_vector_add(&vector, &adding);
  }

  htf_assert(vector.size == TEST_SIZE);
  htf_vector_print_as_int(&vector);
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
