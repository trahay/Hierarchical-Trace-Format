/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include <stdlib.h>
#include "htf.h"
#include "htf_dbg.h"
#include "htf_vector.h"

int main(int argc __attribute__((unused)), char** argv __attribute__((unused))) {
  if (argc < 1) {
    htf_error("Not enough arguments ! 1 argument required.\n");
  }
  if (argc > 2) {
    htf_error("Too many arguments ! 1 argument required.\n");
  }
  long VECTOR_SIZE = strtol(argv[1], NULL, 10);

  htf_vector_t vector;
  htf_vector_new_with_size(&vector, sizeof(int), 1);
  int adding;
  DOFOR(i, VECTOR_SIZE) {
    adding = i;
    htf_vector_add(&vector, &adding);
  }

  htf_assert(vector.size == VECTOR_SIZE);
  htf_vector_print_as_int(&vector);
  printf("\n");
  DOFOR(i, VECTOR_SIZE / 2) {
    htf_vector_remove_at(&vector, i);
    // Should remove all the even numbers
  }
  htf_assert(vector.size == VECTOR_SIZE - VECTOR_SIZE / 2);
  htf_vector_print_as_int(&vector);
  return EXIT_SUCCESS;
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
