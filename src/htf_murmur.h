//
// Created by khatharsis on 10/07/23.
//
#include "htf.h"
#ifndef HTF_SRC_HTF_MURMUR_H
#define HTF_SRC_HTF_MURMUR_H
void htf_murmur_hash3_x64_32(const void* key, const int len, const uint32_t seed, uint32_t* out);
void htf_murmur_hash3_x64_64(const void* key, const int len, const uint32_t seed, uint64_t* out);
#endif  // HTF_SRC_HTF_MURMUR_H
