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

/**Tries to fill this vector with the next one's data.*/
void _htf_vector_reorganize(htf_vector_t* vector) {
  htf_warn("This array has too much unused memory, reorganizing\n");
  // This means that the current array isn't using enough capacity, so we'll move the memory
  uint mem_to_move = vector->allocated - vector->_local_size;
  if (mem_to_move > vector->next->_local_size) {
    mem_to_move = vector->next->_local_size;
  }
  memmove(vector->array + vector->_local_size * vector->element_size, vector->next->array,
          mem_to_move * vector->element_size);
  memmove(vector->next->array, vector->next->array + mem_to_move * vector->element_size,
          (vector->next->allocated - mem_to_move) * vector->element_size);
  vector->next->_local_size -= mem_to_move;
  vector->_local_size += mem_to_move;
  // TODO If the next one is empty, we should put it at the end of the linked structure.
}

void htf_vector_new(htf_vector_t* vector, size_t element_size) {
  htf_vector_new_with_size(vector, element_size, DEFAULT_VECTOR_SIZE);
};

void htf_vector_new_with_size(htf_vector_t* vector, size_t element_size, uint size) {
  vector->element_size = element_size;
  vector->size = 0;
  vector->allocated = size;
  vector->array = malloc(element_size * size);
  vector->next = NULL;
}

void htf_vector_add(htf_vector_t* vector, void* element) {
  if (vector->_local_size >= vector->allocated) {
    if (vector->next == NULL) {
      htf_warn("Extending the array size\n");
      vector->next = malloc(sizeof(htf_vector_t));
      htf_vector_new_with_size(vector->next, vector->element_size, vector->allocated);
    }
    htf_vector_add(vector->next, element);
    vector->size++;
    return;
  }
  if (vector->next != NULL) {
    // If this linked list has a next one, we prioritize putting it in the other one
    // Except in the case where this one is becoming too small
    // Or if the next one is empty
    if (vector->_local_size <= vector->allocated / VECTOR_EMPTINESS_SWITCH && vector->next->_local_size) {
      _htf_vector_reorganize(vector);
    }
    if (vector->_local_size >= vector->allocated || (vector->next->_local_size != 0 || vector->next->next != NULL)) {
      htf_vector_add(vector->next, element);
      vector->size++;
      return;
    }
  }
  memcpy(vector->array + vector->_local_size * vector->element_size, element, vector->element_size);
  vector->size++;
  vector->_local_size++;
}

void* htf_vector_get(htf_vector_t* vector, int index) {
  if (index < vector->size) {
    if (index < vector->_local_size) {
      return vector->array + (index * vector->element_size);
    } else {
      return htf_vector_get(vector->next, index - vector->_local_size);
    }
  } else
    return NULL;
};

void htf_vector_remove_at(htf_vector_t* vector, int index) {
  // ... Is there any way to remove a single element from a hybrid linked list
  // Without doing some pretty nasty stuff ???
  if (index < vector->size) {
    if (index < vector->_local_size) {
      memmove(vector->array + index * vector->element_size, vector->array + (index + 1) * vector->element_size,
              (vector->_local_size - 1 - index) * vector->element_size);
      vector->_local_size--;
    } else
      htf_vector_remove_at(vector->next, index - vector->_local_size);
  }
  vector->size--;
};

void htf_vector_remove_first(htf_vector_t* vector, void* element) {
  for (int i = 0; i < vector->_local_size; i++) {
    if (memcmp(element, vector->array + i * vector->element_size, vector->element_size) == 0) {
      htf_vector_remove_at(vector, i);
      return;
    }
  }
  htf_vector_remove_first(vector->next, element);
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
