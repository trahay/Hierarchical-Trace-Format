#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "htf_dbg.h"

enum htf_debug_level htf_debug_level = dbg_lvl_normal;

static void print_help() {
  htf_log(dbg_lvl_error, "You can set VERBOSE to different values to control the verbosity of HTF:\n");
  htf_log(dbg_lvl_error, "dbg_lvl_error:   only print errors\n");
  htf_log(dbg_lvl_error, "dbg_lvl_quiet:   only print important messages\n");
  htf_log(dbg_lvl_error, "dbg_lvl_normal:  default verbosity level\n");
  htf_log(dbg_lvl_error, "dbg_lvl_verbose: print additional information\n");
  htf_log(dbg_lvl_error, "dbg_lvl_debug:   print many information\n");
  htf_log(dbg_lvl_error, "dbg_lvl_help:    print the diffent verbosity level and exit\n");
  htf_log(dbg_lvl_error, "dbg_lvl_max:     flood stdout with debug messages\n");
  exit(EXIT_SUCCESS);
}

void htf_debug_level_init() {
  char* verbose_str = getenv("VERBOSE");
  if(verbose_str) {
    if(strcmp(verbose_str, "error") == 0) htf_debug_level = dbg_lvl_error;
    else if(strcmp(verbose_str, "quiet") == 0) htf_debug_level = dbg_lvl_quiet;
    else if(strcmp(verbose_str, "normal") == 0) htf_debug_level = dbg_lvl_normal;
    else if(strcmp(verbose_str, "verbose") == 0) htf_debug_level = dbg_lvl_verbose;
    else if(strcmp(verbose_str, "debug") == 0) htf_debug_level = dbg_lvl_debug;
    else if(strcmp(verbose_str, "help") == 0) htf_debug_level = dbg_lvl_help;
    else if(strcmp(verbose_str, "max") == 0) htf_debug_level = dbg_lvl_max;
    else
      htf_debug_level = dbg_lvl_verbose;

    switch(htf_debug_level)  {
    case dbg_lvl_error:   // only print errors
      htf_log(dbg_lvl_normal, "Debug level: error\n");
      break;
    case dbg_lvl_quiet:
      htf_log(dbg_lvl_normal, "Debug level: quiet\n");
      break;
    case dbg_lvl_normal:
      htf_log(dbg_lvl_normal, "Debug level: normal\n");
      break;
    case dbg_lvl_verbose:
      htf_log(dbg_lvl_normal, "Debug level: verbose\n");
      break;
    case dbg_lvl_debug:
      htf_log(dbg_lvl_normal, "Debug level: debug\n");
      break;
    case dbg_lvl_help:
      htf_log(dbg_lvl_normal, "Debug level: help\n");
      print_help();
      break;
    case dbg_lvl_max:
      htf_log(dbg_lvl_normal, "Debug level: max\n");
      break;

    }
  }

}
