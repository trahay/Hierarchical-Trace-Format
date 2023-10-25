/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
#include "htf/htf_dbg.h"
#include "htf/htf.h"

namespace htf {
enum DebugLevel debugLevel = DebugLevel::Normal;

}

static void print_help() {
  htf_log(htf::DebugLevel::Error, "You can set VERBOSE to different values to control the verbosity of HTF:\n");
  htf_log(htf::DebugLevel::Error, "Error:   only print errors\n");
  htf_log(htf::DebugLevel::Error, "Quiet:   only print important messages\n");
  htf_log(htf::DebugLevel::Error, "Normal:  default verbosity level\n");
  htf_log(htf::DebugLevel::Error, "Verbose: print additional information\n");
  htf_log(htf::DebugLevel::Error, "Debug:   print many information\n");
  htf_log(htf::DebugLevel::Error, "Help:    print the diffent verbosity level and exit\n");
  htf_log(htf::DebugLevel::Error, "Max:     flood stdout with debug messages\n");
  exit(EXIT_SUCCESS);
}

void htf_debug_level_set(enum htf::DebugLevel lvl) {
  htf::debugLevel = lvl;
  switch (htf::debugLevel) {
  case htf::DebugLevel::Error:  // only print errors
    htf_log(htf::DebugLevel::Normal, "Debug level: error\n");
    break;
  case htf::DebugLevel::Quiet:
    htf_log(htf::DebugLevel::Normal, "Debug level: quiet\n");
    break;
  case htf::DebugLevel::Normal:
    htf_log(htf::DebugLevel::Normal, "Debug level: normal\n");
    break;
  case htf::DebugLevel::Verbose:
    htf_log(htf::DebugLevel::Normal, "Debug level: verbose\n");
    break;
  case htf::DebugLevel::Debug:
    htf_log(htf::DebugLevel::Normal, "Debug level: debug\n");
    break;
  case htf::DebugLevel::Help:
    htf_log(htf::DebugLevel::Normal, "Debug level: help\n");
    print_help();
    break;
  case htf::DebugLevel::Max:
    htf_log(htf::DebugLevel::Normal, "Debug level: max\n");
    break;
  }
}

void htf_debug_level_init() {
  char* verbose_str = getenv("VERBOSE");
  if (verbose_str) {
    enum htf::DebugLevel lvl = htf::DebugLevel::Verbose;
    if (strcmp(verbose_str, "error") == 0)
      lvl = htf::DebugLevel::Error;
    else if (strcmp(verbose_str, "quiet") == 0)
      lvl = htf::DebugLevel::Quiet;
    else if (strcmp(verbose_str, "normal") == 0)
      lvl = htf::DebugLevel::Normal;
    else if (strcmp(verbose_str, "verbose") == 0)
      lvl = htf::DebugLevel::Verbose;
    else if (strcmp(verbose_str, "debug") == 0)
      lvl = htf::DebugLevel::Debug;
    else if (strcmp(verbose_str, "help") == 0)
      lvl = htf::DebugLevel::Help;
    else if (strcmp(verbose_str, "max") == 0)
      lvl = htf::DebugLevel::Max;
    htf_debug_level_set(lvl);
  }
}

enum htf::DebugLevel htf_debug_level_get() {
  return htf::debugLevel;
};

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
