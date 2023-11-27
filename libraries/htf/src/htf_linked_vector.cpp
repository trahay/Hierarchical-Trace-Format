//
// Created by khatharsis on 04/10/23.
//

#include "htf/htf_linked_vector.h"
#include <cstring>
using namespace htf;

LinkedVector::LinkedVector() {
  first = new SubVector(default_size);
  last = first;
}

uint64_t& LinkedVector::add(uint64_t element) {
  if (this->last->size >= this->last->allocated) {
    htf_log(DebugLevel::Debug, "Adding a new tail to an array: %p\n", this);
    last = new SubVector(default_size, last);
  }
  size++;
  return last->add(element);
}
uint64_t& LinkedVector::at(size_t index) const {
  if (index >= size) {
    htf_error("Getting an element whose index (%lu) is bigger than vector size (%lu)\n", index, size);
  }
  struct SubVector* correct_sub = last;
  while (index < correct_sub->starting_index) {
    correct_sub = correct_sub->previous;
  }
  return correct_sub->at(index);
}
uint64_t& LinkedVector::front() const {
  return first->at(0);
}

uint64_t& LinkedVector::back() const {
  return last->at(size - 1);
}

void LinkedVector::print() const {
  std::cout << "[";
  for (auto& i : *this) {
    std::cout << i << ((&i != &this->back()) ? ", " : "]");
  }
}

/* C++ Callbacks for C Usage */
LinkedVector* linked_vector_new() {
  return new LinkedVector();
}
uint64_t* linked_vector_add(LinkedVector* linkedVector, uint64_t item) {
  return &linkedVector->add(item);
}
uint64_t* linked_vector_get(LinkedVector* linkedVector, size_t index) {
  return &linkedVector->at(index);
}
uint64_t* linked_vector_get_last(LinkedVector* linkedVector) {
  return &linkedVector->back();
}
void print(LinkedVector linkedVector) {
  return linkedVector.print();
}
