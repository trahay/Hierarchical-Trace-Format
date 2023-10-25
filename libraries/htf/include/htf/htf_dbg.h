/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
/** @file
 * Debug macros, debug levels, all that stuff.
 */
#pragma once
#ifndef NDEBUG
#define DEBUG
#endif

#ifdef __cplusplus
/* Only exists in C++. */
#define CXX(cxx_name) cxx_name
/* Only exists in C. */
#define C(c_name)
#else
/* Only exists in C++. */
#define CXX(cxx_name)
/* Only exists in C. */
#define C(c_name) c_name
#endif

/** A macro to help naming conventions in C/C++. First argument is only kept in C, second is only kept in C++. */
#define C_CXX(c_name, cxx_name) C(c_name) CXX(cxx_name)
/* Adds htf:: in front of the variables in C++. */
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

enum CXX(class) DebugLevel {
  Error,   /**< only print errors */
  Quiet,   /**< only print important messages */
  Normal,  /**< default verbosity level */
  Verbose, /**< print additional information */
  Debug,   /**< print much information */
  Help,    /**< print the different verbosity level and exit */
  Max,     /**< flood stdout with debug messages */
};
#ifdef __cplusplus
extern enum DebugLevel debugLevel;
}; /* namespace htf */
extern "C" {
#endif
extern void htf_debug_level_init();
extern void htf_debug_level_set(enum HTF(DebugLevel) lvl);
extern enum HTF(DebugLevel) htf_debug_level_get();
CXX(
};)

#define htf_abort() abort()
#define _htf_log(fd, _debug_level_, format, ...)                        \
  do {                                                                  \
    if (C_CXX(htf_debug_level_get(), htf::debugLevel) >= _debug_level_) \
      printf("[HTF - %lx] " format, pthread_self(), ##__VA_ARGS__);     \
  } while (0)

#define htf_log(_debug_level_, format, ...) _htf_log(stdout, _debug_level_, format, ##__VA_ARGS__)

#define htf_warn(format, ...)                                                                                          \
  do {                                                                                                                 \
    _htf_log(stderr, C_CXX(Normal, htf::DebugLevel::Normal), "HTF warning in %s (%s:%d): " format, __func__, __FILE__, \
             __LINE__, ##__VA_ARGS__);                                                                                 \
  } while (0)

#define htf_error(format, ...)                                                                                     \
  do {                                                                                                             \
    _htf_log(stderr, C_CXX(Error, htf::DebugLevel::Error), "HTF error in %s (%s:%d): " format, __func__, __FILE__, \
             __LINE__, ##__VA_ARGS__);                                                                             \
    htf_abort();                                                                                                   \
  } while (0)

#ifdef NDEBUG
#define htf_assert(cond)
#else
#define htf_assert(cond)             \
  do {                               \
    if (!(cond))                     \
      htf_error("Assertion failed"); \
  } while (0)
#endif

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
