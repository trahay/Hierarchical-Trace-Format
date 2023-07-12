//
// Created by khatharsis on 10/07/23.
//

#include <inttypes.h>
#include <malloc.h>
#include "htf_murmur.h"
#include "stdlib.h"
#define SEED 0
#define MAX_EVENT (1000000)
#define MAX_SEQUENCE (1 << 14)
#define MAX_LOOP (MAX_SEQUENCE >> 1)
#define MAX_SEQUENCE_LENGTH (1 << 8)
#define NUM_TEST_SEQUENCE (1 << 14)
#define NUM_HASHED (MAX_EVENT + MAX_SEQUENCE + MAX_LOOP + (NUM_TEST_SEQUENCE * MAX_SEQUENCE_LENGTH))
#define SIZE_COLLISION_ARRAY UINT32_MAX
int main(int argc, char** argv) {
  short* collisions = calloc(SIZE_COLLISION_ARRAY, sizeof(int));
  htf_assert(collisions);
  int count_collision = 0;
  uint32_t hash;
  // Start with testing the single events and sequences and loops
  printf("Testing all the events up to %d\n", MAX_EVENT);
  for (uint32_t i = 0; i < MAX_EVENT; i++) {
    htf_token_t token = {.type = HTF_TYPE_EVENT, .id = i};
    htf_murmur_hash3_x64_32(&token, 1, SEED, &hash);
    uint32_t new_key = hash % SIZE_COLLISION_ARRAY;
    int buffer;
    buffer = collisions[new_key]++;
    if (buffer) {
      count_collision++;
    }
  }
  printf("Testing all the sequences up to %d\n", MAX_SEQUENCE);
  for (uint32_t i = 0; i < MAX_SEQUENCE; i++) {
    htf_token_t token = {.type = HTF_TYPE_SEQUENCE, .id = i};
    htf_murmur_hash3_x64_32(&token, 1, SEED, &hash);
    uint32_t new_key = hash % SIZE_COLLISION_ARRAY;
    if (collisions[new_key]++) {
      count_collision++;
    }
  }
  printf("Testing all the loops up to %d\n", MAX_LOOP);
  for (uint32_t i = 0; i < MAX_LOOP; i++) {
    htf_token_t token = {.type = HTF_TYPE_LOOP, .id = i};
    htf_murmur_hash3_x64_32(&token, 1, SEED, &hash);
    uint32_t new_key = hash % SIZE_COLLISION_ARRAY;
    if (collisions[new_key]++) {
      count_collision++;
    }
  }

  printf("Testing different sizes sequences, %d of them each\n", NUM_TEST_SEQUENCE);
  for (uint32_t sequence_size = 2; sequence_size < MAX_SEQUENCE_LENGTH; sequence_size++) {
    printf("\t Testing size %d\n", sequence_size);
    for (uint32_t i = 0; i < NUM_TEST_SEQUENCE; i++) {
      // Constructing the sequence
      htf_token_t* token = malloc(sizeof(htf_token_t) * sequence_size);
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

      htf_murmur_hash3_x64_32(&token, sequence_size, SEED, &hash);
      uint32_t new_key = hash % SIZE_COLLISION_ARRAY;
      int buffer;
      if (collisions[new_key]) {
        count_collision++;
      }
      collisions[new_key]++;
    }
  }

  printf("Counted a total of %d collisions out of %d hits = %u%%\n", count_collision, NUM_HASHED,
         (count_collision * 100) / NUM_HASHED);

  uint32_t max_i = 0;
  for (uint32_t i = 0; i < SIZE_COLLISION_ARRAY; i++) {
    if (collisions[i] > collisions[max_i]) {
      max_i = i;
    }
  }
  printf("Max collision at %d: %d\n", max_i, collisions[max_i]);
}
