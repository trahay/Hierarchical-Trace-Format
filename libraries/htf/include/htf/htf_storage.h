/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
#pragma once

#include "htf.h"
#include "htf_archive.h"
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

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
