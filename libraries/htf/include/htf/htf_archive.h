/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
#pragma once

#include "htf.h"

#ifdef __cplusplus
namespace htf {
#endif

/**
 * A location_group can be a process, a machine, etc.
 */
struct LocationGroup {
  LocationGroupId id;
  StringRef name;
  LocationGroupId parent;
};

/**
 * A location is basically a thread (or GPU stream)
 */
struct Location {
  ThreadId id;
  StringRef name;
  LocationGroupId parent;
};

/**
 * A definition describes a function
 */
typedef struct Definition {
#ifdef __cplusplus
 public:
  std::vector<String> strings;
  std::vector<Region> regions;
  std::vector<Attribute> attributes;
  [[nodiscard]] const String* getString(StringRef) const;
  void addString(StringRef, const char*);
  [[nodiscard]] const Region* getRegion(RegionRef) const;
  void addRegion(RegionRef, StringRef);
  [[nodiscard]] const Attribute* getAttribute(AttributeRef) const;
  void addAttribute(AttributeRef, StringRef, StringRef, htf_type_t);
#endif
} Definition;

/**
 * An archive represents a program
 */
typedef struct Archive {
  char* dir_name;
  char* trace_name;
  char* fullpath;
  pthread_mutex_t lock;

  LocationGroupId id;
  struct Archive* global_archive;
  struct Definition* definitions CXX({new Definition()});
  /* a list of threads */
  struct Thread** threads;
  int nb_threads;
  int nb_allocated_threads;

  /* a list of archive */
  struct Archive** archive_list;
  int nb_archives;
  int nb_allocated_archives;
  /* Indicates whether there are timestamps in there.*/
  short store_timestamps;
#ifdef __cplusplus
  std::vector<Location> locations{std::vector<Location>()};
  std::vector<LocationGroup> location_groups{std::vector<LocationGroup>()};

  [[nodiscard]] Thread* getThread(ThreadId) const;
  [[nodiscard]] const struct String* getString(StringRef) const;
  [[nodiscard]] const struct Region* getRegion(RegionRef) const;
  [[nodiscard]] const struct Attribute* getAttribute(AttributeRef) const;
  void addString(StringRef, const char*);
  void addRegion(RegionRef, StringRef);
  void addAttribute(AttributeRef, StringRef, StringRef, htf_type_t);
  void open(const char* dirname, const char* given_trace_name, LocationGroupId archive_id);
  void globalOpen(const char* dirname, const char* given_trace_name) {
    open(dirname, given_trace_name, HTF_MAIN_LOCATION_GROUP_ID);
  };
  void defineLocationGroup(LocationGroupId id, StringRef name, LocationGroupId parent);
  void defineLocation(ThreadId id, StringRef name, LocationGroupId parent);
  void finalize() { htf_error("Not implemented yet !\n"); };
  void close();

  [[nodiscard]] const LocationGroup* getLocationGroup(LocationGroupId) const;
  [[nodiscard]] const Location* getLocation(ThreadId) const;
#endif
} Archive;

#ifdef __cplusplus
} /* namespace htf */
extern "C" {
#endif

extern HTF(Archive) * htf_archive_new(void);

extern struct HTF(Thread) * htf_archive_get_thread(HTF(Archive) * archive, HTF(ThreadId) thread_id);

extern const struct HTF(LocationGroup) *
  htf_archive_get_location_group(HTF(Archive) * archive, HTF(LocationGroupId) location_group);
extern const struct HTF(Location) * htf_archive_get_location(HTF(Archive) * archive, HTF(ThreadId) thread_id);

extern void htf_archive_register_string(HTF(Archive) * archive, HTF(StringRef) string_ref, const char* string);

extern void htf_archive_register_region(HTF(Archive) * archive, HTF(RegionRef) region_ref, HTF(StringRef) string_ref);

extern void htf_archive_register_attribute(HTF(Archive) * archive,
                                           HTF(AttributeRef) attribute_ref,
                                           HTF(StringRef) name_ref,
                                           HTF(StringRef) description_ref,
                                           HTF(htf_type_t) type);

extern const struct HTF(String) * htf_archive_get_string(HTF(Archive) * archive, HTF(StringRef) string_ref);

extern const struct HTF(Region) * htf_archive_get_region(HTF(Archive) * archive, HTF(RegionRef) region_ref);

extern const struct HTF(Attribute) * htf_archive_get_attribute(HTF(Archive) * archive, HTF(AttributeRef) attribute_ref);
#ifdef __cplusplus
};
#endif /* __cplusplus */

/* -*-
  mode: c++;
  c-file-style: "k&r";
  c-basic-offset 2;
  tab-width 2 ;
  indent-tabs-mode nil
  -*- */
