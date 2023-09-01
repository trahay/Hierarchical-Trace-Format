/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include "htf_vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htf.h"
#include "htf_dbg.h"

static htf_subvector_t* htf_subvector_new(size_t element_size, uint size) {
  htf_subvector_t* new_subvector = malloc(sizeof(htf_subvector_t));
  new_subvector->allocated = size;
  new_subvector->size = 0;
  new_subvector->array = calloc(size, element_size);
  new_subvector->previous = NULL;
  new_subvector->next = NULL;
  new_subvector->starting_index = 0;
  return new_subvector;
}

static htf_subvector_t* htf_subvector_next(size_t element_size, htf_subvector_t* previous) {
  htf_subvector_t* new_subvector = htf_subvector_new(element_size, previous->allocated);
  new_subvector->starting_index = previous->starting_index + previous->allocated;
  new_subvector->previous = previous;
  previous->next = new_subvector;
  return new_subvector;
}

void htf_vector_new(htf_vector_t* vector, size_t element_size) {
  htf_vector_new_with_size(vector, element_size, DEFAULT_VECTOR_SIZE);
}

void htf_vector_new_with_size(htf_vector_t* vector, size_t element_size, uint size) {
  vector->element_size = element_size;
  vector->size = 0;
  vector->last_subvector = htf_subvector_new(element_size, size);
  vector->first_subvector = vector->last_subvector;
}

void* htf_vector_add(htf_vector_t* vector, void* element) {
  if (vector->last_subvector->size >= vector->last_subvector->allocated) {
    htf_log(htf_dbg_lvl_debug, "Adding a new tail to an array: %p\n", vector);
    vector->last_subvector = htf_subvector_next(vector->element_size, vector->last_subvector);
  }
  void* addr = (void*)((size_t)vector->last_subvector->array + vector->last_subvector->size * vector->element_size);
  memcpy(addr, element, vector->element_size);
  vector->size++;
  vector->last_subvector->size++;
  return addr;
}

void* htf_vector_get(htf_vector_t* vector, int index) {
  htf_assert(index >= 0);
  
  if (index >= vector->size)
    return NULL;
  htf_subvector_t* correct_sub = vector->last_subvector;
  while (index < correct_sub->starting_index || correct_sub->starting_index + correct_sub->size <= index) {
    correct_sub = (index < correct_sub->starting_index) ? correct_sub->previous : correct_sub->next;
  }
  return (void*)((size_t)correct_sub->array + ((index - correct_sub->starting_index) * vector->element_size));
}

void htf_vector_print_as_int(htf_vector_t* vector) {
  if (vector->element_size != sizeof(int)) {
    htf_warn("Warning: printing a array as vect of int where size is %lu\n", vector->element_size);
  }
  printf("[");
  DOFOR(i, vector->size - 1) {
    int value = *(int*)htf_vector_get(vector, i);
    printf("%d, ", value);
  }
  if (vector->size > 0) {
    int value = *(int*)htf_vector_get(vector, vector->size - 1);
    printf("%d", value);
  }
  printf("]");
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
