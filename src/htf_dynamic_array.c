//
// Created by khatharsis on 15/05/23.
//

#include "htf_dynamic_array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htf_dbg.h"

void array_new(htf_array_t* array, size_t element_size) {
	array_new_with_size(array, element_size, DEFAULT_ARRAY_SIZE);
};

/** Creates a new array with a default allocated size.*/
void array_new_with_size(htf_array_t* array, size_t element_size, uint size) {
	array->element_size = element_size;
	array->size = 0;
	array->allocated = size;
	array->array = malloc(element_size * size);
};

/** Adds an element to the array. Copies the value of the element. */
void array_add(htf_array_t* array, void* element) {
	if (array->size >= array->allocated) {
		htf_warn("Reallocating memory \n");
		void* new_buffer = realloc(array->array, array->size * 2 * array->element_size);
		if (new_buffer == NULL) {
			new_buffer = malloc(array->size * 2 * array->element_size);
			if (new_buffer == NULL) {
				htf_error("Could not allocate memory\n");
			}
			memmove(new_buffer, array->array, array->size * array->element_size);
			free(array->array);
		}
		array->allocated *= 2;
		array->array = new_buffer;
	}
	memcpy(array->array + array->size * array->element_size, element, array->element_size);
	array->size++;
}

/** Gets te element at the given index. Returns a pointer to that element, NULL if the index is incorrect. */
void* array_get(htf_array_t* array, int index) {
	if (index < array->size)
		return array->array + (index * array->element_size);
	else
		return NULL;
};

/** Removes the element at the given index. */
void array_remove_at(htf_array_t* array, int index) {
	memmove(array->array + (index)*array->element_size, array->array + (index + 1) * array->element_size,
					(array->size - 1 - index) * array->element_size);
	array->size--;
};

/** Removes the first element whose memory is equal to the given element */
void array_remove_first(htf_array_t* array, void* element) {
	for (int i = 0; i < array->size; i++) {
		if (memcmp(element, array->array + i * array->element_size, array->element_size) == 0) {
			array_remove_at(array, i);
			return;
		}
	}
};