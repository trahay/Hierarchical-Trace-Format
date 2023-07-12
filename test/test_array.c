#include <stdlib.h>
#include "htf_dbg.h"
#include "htf_dynamic_array.h"
//
// Created by khatharsis on 15/05/23.
//
int main(int argc    __attribute__((unused)),
	 char** argv __attribute__((unused))) {
	htf_array_t array;
	int adding;
	array_new_with_size(&array, sizeof(int), 1);
	adding = 10;
	array_add(&array, &adding);
	adding = 20;
	array_add(&array, &adding);
	adding = 30;
	array_add(&array, &adding);
	adding = 40;
	array_add(&array, &adding);
	htf_assert(array.size == 4);
	for (unsigned int i = 0; i < array.size; i++) {
		int a = *(int*)array_get(&array, i);
		htf_assert(a == ((int)i + 1) * 10);
		printf("%d -> %d\n", i, a);
	}
	printf("\n");
	array_remove_at(&array, 0);
	htf_assert(array.size == 3);
	for (unsigned int i = 0; i < array.size; i++) {
		int a = *(int*)array_get(&array, i);
		printf("%d -> %d\n", i, a);
		htf_assert(a == ((int)i + 2) * 10);
	}
	return EXIT_SUCCESS;
}
