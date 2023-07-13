/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#ifndef HTF_VECTOR_H
#define HTF_VECTOR_H

#include <sys/types.h>

typedef struct htf_vector {
  void* vector;
  size_t element_size;
  uint size;
  uint allocated;
} htf_vector_t;

/** Creates a new vector */
void htf_vector_new(htf_vector_t* vector, size_t element_size);

/** Creates a new vector with a default allocated size.*/
void htf_vector_new_with_size(htf_vector_t* vector, size_t element_size, uint size);

/** Adds an element to the vector. Copies the value of the element. */
void htf_vector_add(htf_vector_t* vector, void* element);

/** Gets te element at the given index. Returns a pointer to that element, NULL if the index is incorrect. */
void* htf_vector_get(htf_vector_t* vector, int index);

/** Removes the element at the given index. */
void htf_vector_remove_at(htf_vector_t* vector, int index);

/** Removes the first element whose memory is equal to the given element */
void htf_vector_remove_first(htf_vector_t* vector, void* element);

#define DEFAULT_VECTOR_SIZE 1000

#endif  // HTF_VECTOR_H


/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
