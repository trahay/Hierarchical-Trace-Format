/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include <stdlib.h>
#include "htf_dbg.h"
#include "htf_vector.h"

int main(int argc __attribute__((unused)), char** argv __attribute__((unused))) {
  htf_vector_t vector;
  int adding;
  htf_vector_new_with_size(&vector, sizeof(int), 1);
  adding = 10;
  htf_vector_add(&vector, &adding);
  adding = 20;
  htf_vector_add(&vector, &adding);
  adding = 30;
  htf_vector_add(&vector, &adding);
  adding = 40;
  htf_vector_add(&vector, &adding);
  htf_assert(vector.size == 4);
  for (unsigned int i = 0; i < vector.size; i++) {
    int a = *(int*)htf_vector_get(&vector, i);
    htf_assert(a == ((int)i + 1) * 10);
    printf("%d -> %d\n", i, a);
  }
  printf("\n");
  htf_vector_remove_at(&vector, 0);
  htf_assert(vector.size == 3);
  for (unsigned int i = 0; i < vector.size; i++) {
    int a = *(int*)htf_vector_get(&vector, i);
    printf("%d -> %d\n", i, a);
    htf_assert(a == ((int)i + 2) * 10);
  }
  return EXIT_SUCCESS;
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
