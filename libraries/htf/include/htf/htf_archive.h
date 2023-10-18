/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
#pragma once

#include "htf.h"

#ifdef __cplusplus
namespace htf {
#endif

#define LocationGroupName C_CXX(htf_location_group, LocationGroup)
/**
 * A location_group can be a process, a machine, etc.
 */
struct LocationGroupName {
  LocationGroupIdName id;
  StringRefName name;
  LocationGroupIdName parent;
};

#define LocationName C_CXX(htf_location, Location)
/**
 * A location is basically a thread (or GPU stream)
 */
struct LocationName {
  ThreadIdName id;
  StringRefName name;
  LocationGroupIdName parent;
};

#define DefinitionName C_CXX(htf_definition, Definition)
/**
 * A definition describes a function
 */
typedef struct DefinitionName {
#ifdef __cplusplus
 public:
  std::vector<String> strings;
  std::vector<Region> regions;
  std::vector<Attribute> attributes;
  [[nodiscard]] const String* getString(StringRef) const;
  void addString(StringRefName, const char*);
  [[nodiscard]] const Region* getRegion(RegionRef) const;
  void addRegion(RegionRefName, StringRefName);
  [[nodiscard]] const Attribute* getAttribute(AttributeRef) const;
  void addAttribute(AttributeRefName, StringRefName, StringRefName, htf_type_t);
#endif
} DefinitionName;

#define ArchiveName C_CXX(htf_archive, Archive)
/**
 * An archive represents a program
 */
typedef struct ArchiveName {
  char* dir_name;
  char* trace_name;
  char* fullpath;
  pthread_mutex_t lock;

  LocationGroupIdName id;
  struct ArchiveName* global_archive;
  struct DefinitionName* definitions CXX({new Definition()});
  /* a list of threads */
  struct ThreadName** threads;
  int nb_threads;
  int nb_allocated_threads;

  /* a list of archive */
  struct ArchiveName** archive_list;
  int nb_archives;
  int nb_allocated_archives;
  /* Indicates whether there are timestamps in there.*/
  short store_timestamps;
#ifdef __cplusplus
  std::vector<Location> locations{std::vector<Location>()};
  std::vector<LocationGroup> location_groups{std::vector<LocationGroup>()};

  [[nodiscard]] Thread* getThread(ThreadIdName) const;
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

  [[nodiscard]] const LocationGroup* getLocationGroup(LocationGroupIdName) const;
  [[nodiscard]] const Location* getLocation(ThreadIdName) const;
#endif
} ArchiveName;

#ifdef __cplusplus
} /* namespace htf */
extern "C" {
#endif

extern HTF(ArchiveName) * htf_archive_new(void);

extern struct HTF(ThreadName) * htf_archive_get_thread(HTF(ArchiveName) * archive, HTF(ThreadIdName) thread_id);

extern const struct HTF(LocationGroupName) *
  htf_archive_get_location_group(HTF(ArchiveName) * archive, HTF(LocationGroupIdName) location_group);
extern const struct HTF(LocationName) *
  htf_archive_get_location(HTF(ArchiveName) * archive, HTF(ThreadIdName) thread_id);

extern void htf_archive_register_string(HTF(ArchiveName) * archive, HTF(StringRefName) string_ref, const char* string);

extern void htf_archive_register_region(HTF(ArchiveName) * archive,
                                        HTF(RegionRefName) region_ref,
                                        HTF(StringRefName) string_ref);

extern void htf_archive_register_attribute(HTF(ArchiveName) * archive,
                                           HTF(AttributeRefName) attribute_ref,
                                           HTF(StringRefName) name_ref,
                                           HTF(StringRefName) description_ref,
                                           HTF(htf_type_t) type);

extern const struct HTF(StringName) * htf_archive_get_string(HTF(ArchiveName) * archive, HTF(StringRefName) string_ref);

extern const struct HTF(RegionName) * htf_archive_get_region(HTF(ArchiveName) * archive, HTF(RegionRefName) region_ref);

extern const struct HTF(AttributeName) *
  htf_archive_get_attribute(HTF(ArchiveName) * archive, HTF(AttributeRefName) attribute_ref);
#ifdef __cplusplus
};
#endif /* __cplusplus */

/* -*-
  mode: c;
  c-file-style: "k&r";
  c-basic-offset 2;
  tab-width 2 ;
  indent-tabs-mode nil
  -*- */
