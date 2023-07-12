/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#ifndef OTF2_H
#define OTF2_H

/**
 *  @file
 *
 *  @brief      Main include file for applications using OTF2.
 */

#include <otf2/OTF2_Reader.h>
//#include <otf2/otf2_compiler.h>

#ifndef __otf2_deprecated__
#define __otf2_deprecated__
#endif

#include "htf.h"
#include "htf_archive.h"
#include "htf_write.h"

struct OTF2_GlobalDefWriter_struct {
  struct htf_archive archive;
};

struct OTF2_DefWriter_struct {
  OTF2_LocationRef locationRef;
  struct htf_archive* archive;

  struct htf_thread_writer* thread_writer;
};

struct OTF2_EvtWriter_struct {
  OTF2_LocationRef locationRef;
  struct htf_archive* archive;
  struct htf_thread_writer* thread_writer;
};

struct OTF2_Archive_struct {
  struct htf_archive archive;

  OTF2_GlobalDefWriter* globalDefWriter;

  OTF2_DefWriter** def_writers;
  OTF2_EvtWriter** evt_writers;
  int nb_locations;

  struct OTF2_LockingCallbacks lockingCallbacks;
  void* lockingData;

  struct OTF2_FlushCallbacks flushCallbacks;
  void* flushData;

  pthread_mutex_t lock;
};

#endif /* !OTF2_H */

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
