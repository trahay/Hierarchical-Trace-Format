/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
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
  struct htf_archive *archive;

  struct htf_thread_writer* thread_writer;
};

struct OTF2_EvtWriter_struct {
  OTF2_LocationRef locationRef;
  struct htf_archive *archive;
  struct htf_thread_writer* thread_writer;
};

struct OTF2_Archive_struct {
  struct htf_archive archive;

  OTF2_GlobalDefWriter* globalDefWriter;

  OTF2_DefWriter **def_writers;
  OTF2_EvtWriter **evt_writers;
  int nb_locations;

  struct OTF2_LockingCallbacks lockingCallbacks;
  void*                  lockingData;

  struct OTF2_FlushCallbacks flushCallbacks;
  void*                      flushData;

	pthread_mutex_t lock;
};

#endif /* !OTF2_H */
