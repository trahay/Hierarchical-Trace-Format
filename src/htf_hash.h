/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include "htf.h"
#ifndef HTF_HASH_H
#define HTF_HASH_H
void htf_hash_32(const void* key, const int len, const uint32_t seed, uint32_t* out);
void htf_hash_64(const void* key, const int len, const uint32_t seed, uint64_t* out);
#endif  // HTF_HASH_H


/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
