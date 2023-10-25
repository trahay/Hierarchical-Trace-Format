/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
/** /file
 * This file contains the definitions for Locations, Definitions and Archives.
 * An Archive is the closest thing to an actual trace file. There's one per process (approx).
 * Locations are threads, which are grouped by LocationGroups (processes, machines, etc.).
 * Definitions are where we store local data that we need to be able to parse later (String, regions, attributes).
 */
#pragma once

#include "htf.h"

#ifdef __cplusplus
namespace htf {
#endif

/**
 * A LocationGroup can be a process, a machine, etc.
 */
struct LocationGroup {
  /** Unique id for that group. */
  LocationGroupId id;
  /** Group name */
  StringRef name;
  /** Parent of that group. */
  LocationGroupId parent;
};

/**
 * A location is basically a thread (or GPU stream)
 */
struct Location {
  /** Unique id for that location. */
  ThreadId id;
  /** Location name. */
  StringRef name;
  /** Group containing that location. */
  LocationGroupId parent;
};

/**
 * @brief A Definition stores Strings, Regions and Attributes.
 */
typedef struct Definition {
  /** List of String stored in that Definition. */
  DEFINE_Vector(String, strings);
  /** List of Region stored in that Definition. */
  DEFINE_Vector(Region, regions);
  /** List of Attribute stored in that Definition. */
  DEFINE_Vector(Attribute, attributes);
#ifdef __cplusplus
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
  char* dir_name;       /**< Name of the directory in which the archive is recorded. */
  char* trace_name;     /**< Name of the trace. */
  char* fullpath;       /**< TODO Complete this. */
  pthread_mutex_t lock; /**< Archive-wise lock, used for synchronising some threads. */

  LocationGroupId id;             /**< ID for the htf::LocationGroup of that Archive. */
  struct Archive* global_archive; /**< The Global Archive is the archive encompassing the whole execution. NULL if
                                   * this is the global archive. */

  struct Definition* definitions CXX({new Definition()}); /**< Array of Definition.*/
  struct Thread** threads;                                /**< Array of Thread. */
  int nb_threads;                                         /**< Number of Thread in #threads. */
  int nb_allocated_threads;                               /**< Size of #threads. */

  struct Archive** archive_list; /**< Array of Archive *. */
  int nb_archives;               /**< Number of Archive in #archive_list. */
  int nb_allocated_archives;     /**< Size of #archive_list. */

  DEFINE_Vector(Location, locations);            /**< Vector of Location. */
  DEFINE_Vector(LocationGroup, location_groups); /**< Vector of LocationGroup. */

  short store_timestamps; /**< Indicates whether there are timestamps in there.*/
#ifdef __cplusplus
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

/** Constructor for an Archive. In C, always use this to create a new Archive. */
extern HTF(Archive) * htf_archive_new(void);

/**
 * Getter for a Thread from its id.
 * @returns First Thread matching the given htf::ThreadId, or nullptr if it doesn't have a match.
 */
extern struct HTF(Thread) * htf_archive_get_thread(HTF(Archive) * archive, HTF(ThreadId) thread_id);

/**
 * Getter for a LocationGroup from its id.
 * @returns First LocationGroup matching the given htf::LocationGroupId in this archive, or in the global_archive if it
 * doesn't have a match, or nullptr if it doesn't have a match in the global_archive.
 */
extern const struct HTF(LocationGroup) *
  htf_archive_get_location_group(HTF(Archive) * archive, HTF(LocationGroupId) location_group);

/**
 * Getter for a Location from its id.
 * @returns First Location matching the given htf::ThreadId in this archive, or in the global_archive if it
 * doesn't have a match, or nullptr if it doesn't have a match in the global_archive.
 */
extern const struct HTF(Location) * htf_archive_get_location(HTF(Archive) * archive, HTF(ThreadId) thread_id);

/**
 * Creates a new String and adds it to that Archive.
 * Error if the given htf::StringRef is already in use.
 * Locks and unlocks the mutex for that operation.
 */
extern void htf_archive_register_string(HTF(Archive) * archive, HTF(StringRef) string_ref, const char* string);

/**
 * Creates a new Region and adds it to that Archive.
 * Error if the given htf::RegionRef is already in use.
 * Locks and unlocks the mutex for that operation.
 */
extern void htf_archive_register_region(HTF(Archive) * archive, HTF(RegionRef) region_ref, HTF(StringRef) string_ref);

/**
 * Creates a new Attribute and adds it to that Archive.
 * Error if the given htf::AttributeRef is already in use.
 * Locks and unlocks the mutex for that operation.
 */
extern void htf_archive_register_attribute(HTF(Archive) * archive,
                                           HTF(AttributeRef) attribute_ref,
                                           HTF(StringRef) name_ref,
                                           HTF(StringRef) description_ref,
                                           HTF(htf_type_t) type);

/**
 * Getter for a String from its id.
 * @returns First String matching the given htf::StringRef in this archive, or in the global_archive if it doesn't
 * have a match, or nullptr if it doesn't have a match in the global_archive.
 */
extern const struct HTF(String) * htf_archive_get_string(HTF(Archive) * archive, HTF(StringRef) string_ref);

/**
 * Getter for a Region from its id.
 * @returns First Region matching the given htf::RegionRef in this archive, or in the global_archive if it doesn't
 * have a match, or nullptr if it doesn't have a match in the global_archive.
 */
extern const struct HTF(Region) * htf_archive_get_region(HTF(Archive) * archive, HTF(RegionRef) region_ref);

/**
 * Getter for a Attribute from its id.
 * @returns First Attribute matching the given htf::AttributeRef in this archive, or in the global_archive if it
 * doesn't have a match, or nullptr if it doesn't have a match in the global_archive.
 */
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
