/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
/** @file
 * Hashing functions, used for hashing htf::Sequence.
 */
#pragma once

#include "htf.h"

#ifdef __cplusplus
/** Seed used for the hasing algorithm. */
#define SEED 17
namespace htf {
/** Writes a 32bits hash value to out.*/
void hash32(const void* key, size_t len, uint32_t seed, uint32_t* out);
/** Writes a 64bits hash value to out.*/
void hash64(const void* key, size_t len, uint32_t seed, uint64_t* out);
}  // namespace htf
#endif

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
