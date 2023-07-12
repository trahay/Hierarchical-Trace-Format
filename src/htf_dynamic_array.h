//
// Created by khatharsis on 15/05/23.
//

#ifndef HTF_SRC_HTF_DYNAMIC_ARRAY_H
#define HTF_SRC_HTF_DYNAMIC_ARRAY_H

#include <sys/types.h>
typedef struct htf_array {
  void* array;
  size_t element_size;
  uint size;
  uint allocated;
} htf_array_t;

/** Creates a new array */
void array_new(htf_array_t* array, size_t element_size);

/** Creates a new array with a default allocated size.*/
void array_new_with_size(htf_array_t* array, size_t element_size, uint size);

/** Adds an element to the array. Copies the value of the element. */
void array_add(htf_array_t* array, void* element);

/** Gets te element at the given index. Returns a pointer to that element, NULL if the index is incorrect. */
void* array_get(htf_array_t* array, int index);

/** Removes the element at the given index. */
void array_remove_at(htf_array_t* array, int index);

/** Removes the first element whose memory is equal to the given element */
void array_remove_first(htf_array_t* array, void* element);

#define DEFAULT_ARRAY_SIZE 1000

#endif  // HTF_SRC_HTF_DYNAMIC_ARRAY_H
