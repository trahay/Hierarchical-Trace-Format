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

void htf_vector_new(htf_vector_t* vector, size_t element_size) {
  htf_vector_new_with_size(vector, element_size, DEFAULT_VECTOR_SIZE);
};

/** Creates a new vector with a default allocated size.*/
void htf_vector_new_with_size(htf_vector_t* vector, size_t element_size, uint size) {
  vector->element_size = element_size;
  vector->size = 0;
  vector->allocated = size;
  vector->vector = malloc(element_size * size);
};

/** Adds an element to the vector. Copies the value of the element. */
void htf_vector_add(htf_vector_t* vector, void* element) {
  if (vector->size >= vector->allocated) {
    htf_warn("Reallocating memory from %d to %d\n", vector->allocated, vector->allocated * 2);
    void* new_buffer = realloc(vector->vector, vector->size * 2 * vector->element_size);
    if (new_buffer == NULL) {
      new_buffer = malloc(vector->size * 2 * vector->element_size);
      if (new_buffer == NULL) {
        htf_error("Could not allocate memory\n");
      }
      memmove(new_buffer, vector->vector, vector->size * vector->element_size);
      free(vector->vector);
    }
    vector->allocated *= 2;
    vector->vector = new_buffer;
  }
  memcpy(vector->vector + vector->size * vector->element_size, element, vector->element_size);
  vector->size++;
}

/** Gets te element at the given index. Returns a pointer to that element, NULL if the index is incorrect. */
void* htf_vector_get(htf_vector_t* vector, int index) {
  if (index < vector->size)
    return vector->vector + (index * vector->element_size);
  else
    return NULL;
};

/** Removes the element at the given index. */
void htf_vector_remove_at(htf_vector_t* vector, int index) {
  memmove(vector->vector + (index)*vector->element_size, vector->vector + (index + 1) * vector->element_size,
          (vector->size - 1 - index) * vector->element_size);
  vector->size--;
};

/** Removes the first element whose memory is equal to the given element */
void htf_vector_remove_first(htf_vector_t* vector, void* element) {
  for (int i = 0; i < vector->size; i++) {
    if (memcmp(element, vector->vector + i * vector->element_size, vector->element_size) == 0) {
      htf_vector_remove_at(vector, i);
      return;
    }
  }
}
/* Prints the vector as a vector of ints.*/
void htf_vector_print_as_int(htf_vector_t* vector) {
  if (vector->element_size != sizeof(int)) {
    htf_warn("Warning: printing a vector as vect of int where size is %lu\n", vector->element_size);
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
