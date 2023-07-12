/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#ifndef HTF_ARCHIVE_H
#define HTF_ARCHIVE_H

#include "htf.h"

struct htf_thread* htf_archive_get_thread(struct htf_archive* archive, htf_thread_id_t thread_id);

struct htf_location_group* htf_archive_get_location_group(struct htf_archive* archive,
                                                          htf_location_group_id_t location_group);
struct htf_location* htf_archive_get_location(struct htf_archive* archive, htf_thread_id_t thread_id);

void htf_archive_register_string(struct htf_archive* archive, htf_string_ref_t string_ref, const char* string);

void htf_archive_register_region(struct htf_archive* archive, htf_region_ref_t region_ref, htf_string_ref_t string_ref);

struct htf_string* htf_archive_get_string(struct htf_archive* archive, htf_string_ref_t string_ref);

struct htf_region* htf_archive_get_region(struct htf_archive* archive, htf_region_ref_t region_ref);

#endif

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
