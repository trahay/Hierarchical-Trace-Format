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
/** Default size for creating Vectors and SubVectors.*/
#define DEFAULT_VECTOR_SIZE 1000
namespace htf {
#endif
/**
 * An hybrid between a LinkedList and a Vector.
 *
 * Contains many sub-arrays organized in a linked list fashion.
 * Does not implement any methods to remove items from itself.
 */
typedef struct LinkedVector {
  /** Number of element stored in the vector */
  size_t size CXX({0});

#ifdef __cplusplus
 private:
  /**
   * A fixed-sized array functionning as a node in a LinkedList.
   *
   * We call it a SubVector since it's the sub-structure of our LinkedVector struct.
   */
  struct SubVector {
   public:
    size_t size{0};               /**< Number of elements stored in the vector. */
    size_t allocated;             /**< Number of elements this vector has allocated. */
    uint64_t* array;              /**< Array of elements. Currently only used on uint64_t */
    SubVector* next{nullptr};     /**< Next SubVector in the LinkedVector. nullptr if last. */
    SubVector* previous{nullptr}; /**< Previous SubVector in the LinkedVector. nullptr if first. */
    size_t starting_index;        /**< Starting index of this SubVector. */

    /**
     * Adds a new element at the end of the vector, after its current last element.
     * The content of `val` is copied to the new element.
     *
     * @param val Value to be copied to the new element.
     * @return Reference to the new element.
     */
    uint64_t* add(uint64_t val) {
      array[size] = val;
      return &array[size++];
    }

    /**
     * Returns a reference to the element at specified location `pos`, with bounds checking.
     * @param pos Position of the element in the LinkedVector.
     * @return Reference to the requested element.
     */
    [[nodiscard]] uint64_t& at(size_t pos) const {
      if (pos >= starting_index && pos < size + starting_index) {
        return array[pos - starting_index];
      }
      htf_error("Wrong index (%lu) compared to starting index (%lu) and size (%lu)\n", pos, starting_index, size);
    }

    /**
     * Returns a reference to the element at specified location `pos`, without bounds checking.
     * @param pos Position of the element in the LinkedVector.
     * @return Reference to the requested element.
     */
    uint64_t& operator[](size_t pos) const { return array[pos - starting_index]; }

    /**
     * Construct a SubVector of a given size.
     * @param new_array_size Size of the SubVector.
     * @param previous_subvector Previous SubVector in the LinkedVector.
     */
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

    /**
     * Construct a SubVector from a given already allocated array, and its size.
     * @param size Size of `array`.
     * @param array Allocated array of values.
     */
    SubVector(size_t size, uint64_t* array) {
      previous = nullptr;
      starting_index = 0;
      allocated = size;
      this->array = array;
    }

    /**
     * Copies the values in array to given_array.
     * @param given_array An allocated array of correct size.
     */
    void copyToArray(uint64_t* given_array) const { memcpy(given_array, array, size * sizeof(uint64_t)); }
  };
#endif
  size_t defaultSize CXX({DEFAULT_VECTOR_SIZE}); /**< Default size of the newly created SubVectors.*/
  C_CXX(void, SubVector) * first;                /**< First SubVector in the LinkedList structure.*/
  C_CXX(void, SubVector) * last;                 /**< Last SubVector in the LinkedList structure.*/
#ifdef __cplusplus
 public:
  /**
   * Creates a new LinkedVector, with a SubVector of size `defaultSize`.
   */
  LinkedVector();
  /** Loads a LinkedVector from a file without reading the size. */
  LinkedVector(FILE* file, size_t size);
  /** Loads a LinkedVector from a file. */
  LinkedVector(FILE* file);
  /**
   * Adds a new element at the end of the vector, after its current last element.
   * The content of `val` is copied to the new element.
   *
   * @param val Value to be copied to the new element.
   * @return Reference to the new element.
   */
  uint64_t* add(uint64_t val);
  /**
   * Returns a reference to the element at specified location `pos`, with bounds checking.
   *
   * To do so, parses the LinkedList from the last SubVector to the first one, stopping once the condition
   * `starting_index` <= `pos` < `starting_index` + `size`
   * @param pos Position of the element in the LinkedVector.
   * @return Reference to the requested element.
   */
  [[nodiscard]] uint64_t& at(size_t pos) const;
  /**
   * Returns a reference to the element at specified location `pos`, without bounds checking.
   *
   * To do so, parses the LinkedList from the last SubVector to the first one, stopping once the condition
   * `starting_index` <= `pos` < `starting_index` + `size`
   * @param pos Position of the element in the LinkedVector.
   * @return Reference to the requested element.
   */
  [[nodiscard]] uint64_t& operator[](size_t pos) const;
  /**
   * Returns a reference to the first element in the LinkedVector.
   * @return Reference to the first element.
   */
  [[nodiscard]] uint64_t& front() const;
  /**
   * Returns a reference to the last element in the LinkedVector.
   * @return Reference to the last element.
   */
  [[nodiscard]] uint64_t& back() const;

  /**
   * Prints the content of the LinkedVector to stdout.
   */
  void print() const;
  /**
   * Writes the vector to the given file as an array.
   * @param file File descriptor.
   * @param writeSize Boolean indicating wether you should write the size of the LinkedVector as a header.
   */
  void writeToFile(FILE* file, bool writeSize) const;

  /**
   * Classic ForwardIterator for LinkedVector.
   */
  struct Iterator {
    /// @cond NONE
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
    /// @endcond
  };
  /** Returns an iterator pointing to the first element in the LinkedVector. */
  [[nodiscard]] Iterator begin() const { return {first}; };
  /** Returns an iterator pointing to the **past-the*end** element in the LinkedVector.
   * Here, it means nullptr.*/
  [[nodiscard]] Iterator end() const { return {nullptr}; };
#endif
} LinkedVector;

CXX(
})  // namespace htf


CXX(extern "C" {)
/** Allocates and returns a new LinkedVector. */
  extern HTF(LinkedVector)* linked_vector_new(void);
  /**
   * Adds a new element at the end of the vector, after its current last element.
   * The content of `val` is copied to the new element.
   *
   * @param linkedVector Pointer to the vector.
   * @param val Value to be copied to the new element.
   * @return Pointer to the new element.
   */
  extern uint64_t* linked_vector_add(HTF(LinkedVector) * linkedVector, uint64_t val);
  /**
   * Returns a pointer to the element at specified location `pos`, with bounds checking.
   *
   * To do so, parses the LinkedList from the last SubVector to the first one, stopping once the condition
   * `starting_index` <= `pos` < `starting_index` + `size`
   * @param linkedVector Pointer to the vector.
   * @param pos Position of the element in the LinkedVector.
   * @return Pointer to the requested element.
   */
  extern uint64_t* linked_vector_get(HTF(LinkedVector) * linkedVector, size_t pos);
  /**
   * Returns a pointer to the last element in the LinkedVector.
   * @param linkedVector Pointer to the vector.
   * @return Pointer to the last element.
   */
  extern uint64_t* linked_vector_get_last(HTF(LinkedVector) * linkedVector);
  /**
   * Prints the content of the LinkedVector to stdout.
   */
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
