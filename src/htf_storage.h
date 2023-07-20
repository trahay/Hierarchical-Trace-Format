/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#ifndef HTF_STORAGE_H
#define HTF_STORAGE_H

#include "htf.h"

enum storage_compression_option { NO_COMPRESSION = 0, ZSTD = 1, MASKING = 2, MASKING_ZSTD = 3 };

void htf_storage_init(struct htf_archive* archive);
void htf_storage_finalize_thread(struct htf_thread* thread);
void htf_storage_finalize(struct htf_archive* archive);
void htf_storage_finalize_global(struct htf_archive* archive);

char* htf_archive_fullpath(char* dir_name, char* trace_name);
#define ZSTD_COMPRESSION_LEVEL 1
#endif /* HTF_STORAGE_H */

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
