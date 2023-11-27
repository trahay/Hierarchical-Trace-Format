/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
/** @file
 * A custom type of Linked-List. This takes into account the fact that we never go remove anything from timestamps
 * vector.
 */
#pragma once

#include "htf_dbg.h"
#ifndef __cplusplus
#include <stdint-gcc.h>
#endif
#ifdef __cplusplus
#include <cstring>
#include <iostream>
#include <memory>
#include <vector>
#define DEFAULT_VECTOR_SIZE 1000
namespace htf {
#endif
typedef struct LinkedVector {
  size_t size CXX({0});

#ifdef __cplusplus
 private:
  struct SubVector {
   public:
    size_t size{0};
    size_t allocated;
    uint64_t* array;
    SubVector* next{nullptr};
    SubVector* previous{nullptr};
    size_t starting_index;

    uint64_t& add(uint64_t element) {
      array[size] = element;
      return array[size++];
    }

    [[nodiscard]] uint64_t& at(size_t index) const {
      if (index >= starting_index && index < size + starting_index) {
        return array[index - starting_index];
      }
      htf_error("Wrong index (%lu) compared to starting index (%lu) and size (%lu)\n", index, starting_index, size);
    }
    uint64_t& operator[](size_t index) const { return array[index - starting_index]; }

    SubVector(size_t new_array_size, SubVector* previous_subvector = nullptr) {
      previous = previous_subvector;
      starting_index = 0;
      if (previous) {
        previous->next = this;
        starting_index = previous->starting_index + previous->size;
      }
      allocated = new_array_size;
      array = new uint64_t[new_array_size];
    }

    SubVector(size_t size, uint64_t* array) {
      previous = nullptr;
      starting_index = 0;
      allocated = size;
      this->array = array;
    }

    void copyToArray(uint64_t* given_array) const { memcpy(given_array, array, size * sizeof(uint64_t)); }
  };
#endif
  size_t default_size CXX({DEFAULT_VECTOR_SIZE});
  C_CXX(void, SubVector) * first;
  C_CXX(void, SubVector) * last;
#ifdef __cplusplus
 public:
  LinkedVector();
  /* Loads a LinkedVector from a file without reading the size. */
  LinkedVector(FILE* file, size_t size);
  /* Loads a LinkedVector from a file. */
  LinkedVector(FILE* file);
  uint64_t& add(uint64_t element);
  [[nodiscard]] uint64_t& at(size_t index) const;
  [[nodiscard]] uint64_t& operator[](size_t index) const { return at(index); };
  [[nodiscard]] uint64_t& front() const;
  [[nodiscard]] uint64_t& back() const;

  void print() const;
  /* Writes the vector to the given file as an array. */
  void writeToFile(FILE* file, bool writeSize) const;

  struct Iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = uint64_t;
    using pointer = uint64_t*;
    using reference = uint64_t&;
    Iterator(SubVector* s) {
      if (s) {
        cur_sub = s;
        ptr = &s->array[0];
      } else {
        cur_sub = nullptr;
        ptr = nullptr;
      }
    }
    reference operator*() const { return *ptr; }
    pointer operator->() { return ptr; }

    // Prefix increment
    Iterator& operator++() {
      i++;
      if (i < cur_sub->starting_index + cur_sub->size) {
        ptr++;
      } else {
        cur_sub = cur_sub->next;
        ptr = (cur_sub) ? &(*cur_sub)[i] : nullptr;
      }
      return *this;
    }

    // Postfix increment
    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const Iterator& a, const Iterator& b) { return a.ptr == b.ptr; };
    friend bool operator!=(const Iterator& a, const Iterator& b) { return a.ptr != b.ptr; };

   private:
    SubVector* cur_sub;
    pointer ptr;
    size_t i{0};
  };
  Iterator begin() const { return {first}; };
  Iterator end() const { return {nullptr}; };
#endif
} LinkedVector;

CXX(
})  // namespace htf


CXX(extern "C" {)
  extern HTF(LinkedVector)* linked_vector_new(void);
  extern uint64_t* linked_vector_add(HTF(LinkedVector)*, uint64_t);
  extern uint64_t* linked_vector_get(HTF(LinkedVector)*, size_t);
  extern uint64_t* linked_vector_get_last(HTF(LinkedVector)*);
  extern void print(HTF(LinkedVector));
CXX(
};)

/* -*-
   mode: c++;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
