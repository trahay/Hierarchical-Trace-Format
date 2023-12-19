/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
/** @file
 * Debug macros, debug levels, all that stuff.
 */
#pragma once
#ifndef NDEBUG
/** This macro only exists in Debug mode.*/
#define DEBUG
#endif

#ifdef __cplusplus
/** Only exists in C++. */
#define CXX(cxx_name) cxx_name
/** Only exists in C. */
#define C(c_name)
#else
/** Only exists in C++. */
#define CXX(cxx_name)
/** Only exists in C. */
#define C(c_name) c_name
#endif

/** A macro to help naming conventions in C/C++. First argument is only kept in C, second is only kept in C++. */
#define C_CXX(c_name, cxx_name) C(c_name) CXX(cxx_name)
/** Adds htf:: in front of the variables in C++. */
#define HTF(something) CXX(htf::) something

#ifdef __cplusplus
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#else
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#endif
#ifdef __cplusplus
namespace htf {
#endif

/**
 * Enum to handle the different level of logging.
 */
enum CXX(class) DebugLevel {
  Error,   /**< Only print errors. */
  Quiet,   /**< Only print important messages. */
  Normal,  /**< Default verbosity level. */
  Verbose, /**< Print additional information. */
  Debug,   /**< Print much information. */
  Help,    /**< Print the different verbosity level and exit. */
  Max,     /**< Flood stdout with debug messages. */
};
#ifdef __cplusplus
extern enum DebugLevel debugLevel;
}; /* namespace htf */
extern "C" {
#endif
/** Initializes the DebugLevel using Env Variables. */
extern void htf_debug_level_init();
/** Sets the DebugLevel to the given level. */
extern void htf_debug_level_set(enum HTF(DebugLevel) lvl);
/** Returns the DebugLevel. */
extern enum HTF(DebugLevel) htf_debug_level_get();
CXX(
};)
/** Stops the execution. */
#define htf_abort() abort()
/** Logs a formated message to the given filedescriptor if the given debugLevel is high enough. */
#define _htf_log(fd, _debug_level_, format, ...)                        \
  do {                                                                  \
    if (C_CXX(htf_debug_level_get(), htf::debugLevel) >= _debug_level_) \
      printf("[HTF - %lx] " format, pthread_self(), ##__VA_ARGS__);     \
  } while (0)
/** Logs a formated message to stdout if the given debugLevel is high enough. */
#define htf_log(_debug_level_, format, ...) _htf_log(stdout, _debug_level_, format, ##__VA_ARGS__)
/** Logs a formated message to stderr if the debugLevel is under Normal. */
#define htf_warn(format, ...)                                                                                          \
  do {                                                                                                                 \
    _htf_log(stderr, C_CXX(Normal, htf::DebugLevel::Normal), "HTF warning in %s (%s:%d): " format, __func__, __FILE__, \
             __LINE__, ##__VA_ARGS__);                                                                                 \
  } while (0)
/** Logs a formated message to stderr if the given debugLevel is under Error. */
#define htf_error(format, ...)                                                                                     \
  do {                                                                                                             \
    _htf_log(stderr, C_CXX(Error, htf::DebugLevel::Error), "HTF error in %s (%s:%d): " format, __func__, __FILE__, \
             __LINE__, ##__VA_ARGS__);                                                                             \
    htf_abort();                                                                                                   \
  } while (0)

/** Asserts a condition whatever the build mode (ie. Debug or Release). */
#define htf_assert_always(cond)	     \
  do {                               \
    if (!(cond))                     \
      htf_error("Assertion failed"); \
  } while (0)


#ifdef NDEBUG
/** Asserts a condition only if in Debug mode (if DEBUG is defined). */
#define htf_assert(cond)
#else
/** Asserts a condition only if in Debug mode (if DEBUG is defined). */
#define htf_assert(cond) htf_assert_always(cond)
#endif

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
