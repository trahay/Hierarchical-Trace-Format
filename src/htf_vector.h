/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#ifndef HTF_VECTOR_H
#define HTF_VECTOR_H

#include <sys/types.h>
/**
 * Dynamic arrays, basically a hybrid between a linked list and an array.
 * This struct is kind of a wrapper for easier usage.
 *  - size_t element_size: sizeof the elements stored.
 *  - uint size: Size of this array (not of the array).
 *  - uint _local_size: Number of items stored in the array.
 *  - uint allocated: Number of items allocated in the array.
 *  - htf_vector_t * previous: Next vector in the Linked List structure.
 */
typedef struct htf_vector {
  size_t element_size;
  uint size;
  struct htf_subvector* last_subvector;
  struct htf_subvector* first_subvector;
} htf_vector_t;

/**
 * Dynamic arrays, basically a hybrid between a linked list and an array.
 *  - void* array: The array containing the data.
 *  - uint size: size of that particular array.
 *  - uint starting_index: the starting index of that array.
 *  - uint allocated: Number of items allocated in the array.
 *  - htf_subvector_t previous: The previous subvector in the Linked List structure.
 */

typedef struct htf_subvector {
  void* array;
  uint size;
  uint starting_index;
  uint allocated;
  struct htf_subvector* previous;
  struct htf_subvector* next;
} htf_subvector_t;

/** Creates a new vector */
void htf_vector_new(htf_vector_t* vector, size_t element_size);

/** Creates a new vector with a default allocated size.*/
void htf_vector_new_with_size(htf_vector_t* vector, size_t element_size, uint size);

/** Adds an element to the vector. Copies the value of the element. */
void* htf_vector_add(htf_vector_t* vector, void* element);

/** Gets the element at the given index. Returns a pointer to that element, NULL if the index is incorrect. */
void* htf_vector_get(htf_vector_t* vector, int index);

/* Prints the vector as a vector of ints.*/
void htf_vector_print_as_int(htf_vector_t* vector);

#define DEFAULT_VECTOR_SIZE 1000
// That's a very bad name
#define VECTOR_EMPTINESS_SWITCH 2
#endif  // HTF_VECTOR_H

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
