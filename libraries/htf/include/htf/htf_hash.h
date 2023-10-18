/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
#pragma once

#include "htf.h"

#ifdef __cplusplus
#define SEED 17
namespace htf {
void hash32(const void* key, const size_t len, const uint32_t seed, uint32_t* out);
void hash64(const void* key, const size_t len, const uint32_t seed, uint64_t* out);
}  // namespace htf
#endif

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
