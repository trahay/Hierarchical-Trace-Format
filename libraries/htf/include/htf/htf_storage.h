/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
/** @file
 * Functions related to reading/writing a trace file.
 */
#pragma once

#include "htf.h"
#include "htf_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Creates the directories for the trace to be written.
 * @param archive Archive to be written to a folder.
 */
void htf_storage_init(HTF(Archive) * archive);
/**
 * @brief Finalize the writing process by writing the thread.
 * @param thread Thread to be written to folder.
 */
void htf_storage_finalize_thread(HTF(Thread) * thread);
/**
 * @brief Finalize the writing process by writing the whole archive.
 * @param archive Archive to be written to a folder.
 */
void htf_storage_finalize(HTF(Archive) * archive);
/**
 * @brief Returns the path of the archive's folder.
 * @param dir_name Directory for the archive's storage.
 * @param trace_name Name of the trace.
 * @return Path to the archive's folder.
 */
char* htf_archive_fullpath(char* dir_name, char* trace_name);
/**
 * @brief Read an archive from a `main.htf` file.
 * @param archive Pointer to an allocated archive.
 * @param main_filename Path to a `main.htf` file.
 */
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
