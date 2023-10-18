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
  htf_log(htf::Error, "You can set VERBOSE to different values to control the verbosity of HTF:\n");
  htf_log(htf::Error, "Error:   only print errors\n");
  htf_log(htf::Error, "Quiet:   only print important messages\n");
  htf_log(htf::Error, "Normal:  default verbosity level\n");
  htf_log(htf::Error, "Verbose: print additional information\n");
  htf_log(htf::Error, "Debug:   print many information\n");
  htf_log(htf::Error, "Help:    print the diffent verbosity level and exit\n");
  htf_log(htf::Error, "Max:     flood stdout with debug messages\n");
  exit(EXIT_SUCCESS);
}

void htf_debug_level_set(enum htf::DebugLevel lvl) {
  htf::debugLevel = lvl;
  switch (htf::debugLevel) {
  case htf::Error:  // only print errors
    htf_log(htf::Normal, "Debug level: error\n");
    break;
  case htf::Quiet:
    htf_log(htf::Normal, "Debug level: quiet\n");
    break;
  case htf::Normal:
    htf_log(htf::Normal, "Debug level: normal\n");
    break;
  case htf::Verbose:
    htf_log(htf::Normal, "Debug level: verbose\n");
    break;
  case htf::Debug:
    htf_log(htf::Normal, "Debug level: debug\n");
    break;
  case htf::Help:
    htf_log(htf::Normal, "Debug level: help\n");
    print_help();
    break;
  case htf::Max:
    htf_log(htf::Normal, "Debug level: max\n");
    break;
  }
}

void htf_debug_level_init() {
  char* verbose_str = getenv("VERBOSE");
  if (verbose_str) {
    enum htf::DebugLevel lvl = htf::Verbose;
    if (strcmp(verbose_str, "error") == 0)
      lvl = htf::Error;
    else if (strcmp(verbose_str, "quiet") == 0)
      lvl = htf::Quiet;
    else if (strcmp(verbose_str, "normal") == 0)
      lvl = htf::Normal;
    else if (strcmp(verbose_str, "verbose") == 0)
      lvl = htf::Verbose;
    else if (strcmp(verbose_str, "debug") == 0)
      lvl = htf::Debug;
    else if (strcmp(verbose_str, "help") == 0)
      lvl = htf::Help;
    else if (strcmp(verbose_str, "max") == 0)
      lvl = htf::Max;
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
