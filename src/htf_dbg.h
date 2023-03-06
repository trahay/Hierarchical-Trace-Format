#ifndef HTF_DBG_H
#define HTF_DBG_H

enum htf_debug_level {
  dbg_lvl_error,   // only print errors
  dbg_lvl_quiet,   // only print important messages
  dbg_lvl_normal,  // default verbosity level
  dbg_lvl_verbose, // print additional information
  dbg_lvl_debug,   // print many information
  dbg_lvl_help,    // print the diffent verbosity level and exit
  dbg_lvl_max,     // flood stdout with debug messages
};

extern enum htf_debug_level htf_debug_level;

void htf_debug_level_init();
void htf_debug_level_set(enum htf_debug_level  lvl);

#define htf_abort() abort()

#define _htf_log(fd, _debug_level_, format, ...)	\
  do {						\
    if (htf_debug_level >= _debug_level_)	\
      printf("[HTF] "format, ##__VA_ARGS__);	\
  } while(0)

#define htf_log(_debug_level_, format, ...) _htf_log(stdout, _debug_level_, format, ##__VA_ARGS__)

#define htf_warn(format, ...)						\
  do {									\
    _htf_log(stderr, dbg_lvl_normal, "HTF warning in %s (%s:%d): "format, \
	     __func__, __FILE__, __LINE__,				\
	     ##__VA_ARGS__);						\
  } while(0)

#define htf_error(format, ...)					\
  do {								\
    _htf_log(stderr, dbg_lvl_error, "HTF error in %s (%s:%d): "format,	\
	     __func__, __FILE__, __LINE__,				\
	     ##__VA_ARGS__);						\
    htf_abort();							\
  } while(0)

#ifdef NDEBUG
#define htf_assert(cond) do { (void)sizeof(cond);} while (0)
#else
#define htf_assert(cond)			\
  do {						\
    if(!(cond))					\
      htf_error("Assertion failed");		\
  } while(0)
#endif


#endif
