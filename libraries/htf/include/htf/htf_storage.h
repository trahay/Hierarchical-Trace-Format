/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
#pragma once

#include "htf.h"
#include "htf_archive.h"
enum storage_compression_option { NO_COMPRESSION = 0, ZSTD = 1, MASKING = 2, MASKING_ZSTD = 3 };
#ifdef __cplusplus
extern "C" {
#endif
void htf_storage_init(HTF(Archive) * archive);
void htf_storage_finalize_thread(HTF(Thread) * thread);
void htf_storage_finalize(HTF(Archive) * archive);
void htf_storage_finalize_global(HTF(Archive) * archive);
char* htf_archive_fullpath(char* dir_name, char* trace_name);
void htf_read_archive(HTF(Archive) * archive, char* main_filename);
#ifdef __cplusplus
};
#endif

#define ZSTD_COMPRESSION_LEVEL 1

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
