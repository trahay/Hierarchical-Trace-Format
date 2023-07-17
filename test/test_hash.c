/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include <inttypes.h>
#include <malloc.h>
#include "htf_hash.h"
#include "stdlib.h"

#define SEED 0
#define MAX_EVENT (1000000)
#define MAX_SEQUENCE (1 << 14)
#define MAX_LOOP (MAX_SEQUENCE >> 1)
#define MAX_SEQUENCE_LENGTH (1 << 8)
#define NUM_TEST_SEQUENCE (1 << 14)
#define NUM_HASHED (MAX_EVENT + MAX_SEQUENCE + MAX_LOOP + (NUM_TEST_SEQUENCE * MAX_SEQUENCE_LENGTH))
#define SIZE_COLLISION_ARRAY UINT32_MAX

int main(int argc __attribute__((unused)), char** argv __attribute__((unused))) {
  short* collisions = calloc(SIZE_COLLISION_ARRAY, sizeof(int));
  htf_vector_t collision_indexes;
  htf_vector_new_with_size(&collision_indexes, sizeof(uint32_t), 4000);

  htf_assert(collisions);
  int count_collision = 0;
  uint32_t hash[4] = {0};  // This is so that we can quickly test 64-bit and 128-bit hashing
  // Start with testing the single events and sequences and loops
  printf("Testing all the events up to %d\n", MAX_EVENT);
  struct timespec start_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  for (uint32_t i = 0; i < MAX_EVENT; i++) {
    htf_token_t token = {.type = HTF_TYPE_EVENT, .id = i};
    htf_hash_32(&token, 1, SEED, hash);
    uint32_t new_key = hash[0] % SIZE_COLLISION_ARRAY;
    int buffer = collisions[new_key]++;
    if (buffer) {
      count_collision++;
      if (buffer == 1) {
        htf_vector_add(&collision_indexes, &new_key);
      }
    }
  }
  printf("Testing all the sequences up to %d\n", MAX_SEQUENCE);
  for (uint32_t i = 0; i < MAX_SEQUENCE; i++) {
    htf_token_t token = {.type = HTF_TYPE_SEQUENCE, .id = i};
    htf_hash_32(&token, 1, SEED, hash);
    uint32_t new_key = hash[0] % SIZE_COLLISION_ARRAY;
    int buffer = collisions[new_key]++;
    if (buffer) {
      count_collision++;
      if (buffer == 1) {
        htf_vector_add(&collision_indexes, &new_key);
      }
    }
  }
  printf("Testing all the loops up to %d\n", MAX_LOOP);
  for (uint32_t i = 0; i < MAX_LOOP; i++) {
    htf_token_t token = {.type = HTF_TYPE_LOOP, .id = i};
    htf_hash_32(&token, 1, SEED, hash);
    uint32_t new_key = hash[0] % SIZE_COLLISION_ARRAY;
    int buffer = collisions[new_key]++;
    if (buffer) {
      count_collision++;
      if (buffer == 1) {
        htf_vector_add(&collision_indexes, &new_key);
      }
    }
  }

  printf("Testing different sizes sequences, %d of them each\n", NUM_TEST_SEQUENCE);
  for (uint32_t sequence_size = 2; sequence_size < MAX_SEQUENCE_LENGTH; sequence_size++) {
    printf("\t Testing size %d\n", sequence_size);
    htf_token_t* token = malloc(sizeof(htf_token_t) * sequence_size);
    for (uint32_t i = 0; i < NUM_TEST_SEQUENCE; i++) {
      // Constructing the sequence
      for (uint32_t j = 0; j < sequence_size; j++) {
        enum htf_token_type type = 1 + (rand() % 3);
        if (type == HTF_TYPE_EVENT) {
          htf_token_t t = {.type = type, .id = rand() % MAX_EVENT};
          token[j] = t;
        }
        if (type == HTF_TYPE_SEQUENCE) {
          htf_token_t t = {.type = type, .id = rand() % MAX_SEQUENCE};
          token[j] = t;
        }
        if (type == HTF_TYPE_LOOP) {
          htf_token_t t = {.type = type, .id = rand() % MAX_LOOP};
          token[j] = t;
        }
      }

      htf_hash_32(token, sequence_size, SEED, hash);
      uint32_t new_key = hash[0] % SIZE_COLLISION_ARRAY;
      int buffer = collisions[new_key]++;
      if (buffer == 1)
        htf_vector_add(&collision_indexes, &new_key);
      if (buffer)
        count_collision++;
    }
    free(token);
  }

  struct timespec end_time;
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  uint64_t time_per_hash =
    ((end_time.tv_sec - start_time.tv_sec) * 1000000000 + (end_time.tv_nsec - start_time.tv_nsec)) / NUM_HASHED;
  printf("Counted a total of %d collisions out of %d hits = %f%%\n", count_collision, NUM_HASHED,
         (count_collision * 100.0) / NUM_HASHED);
  printf("%lu ns/hash\n", time_per_hash);

  uint32_t max_index = 0;
  for (uint32_t i = 0; i < collision_indexes.size; i++) {
    uint32_t* index = htf_vector_get(&collision_indexes, i);
    if (collisions[*index] > collisions[max_index]) {
      max_index = *index;
    }
  }
  printf("Max collision: %d\n", collisions[max_index]);
  printf("Found at:\n");
  for (uint32_t i = 0; i < collision_indexes.size; i++) {
    uint32_t* index = htf_vector_get(&collision_indexes, i);
    if (collisions[*index] == collisions[max_index]) {
      printf("\t%u\n", *index);
    }
  }
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
