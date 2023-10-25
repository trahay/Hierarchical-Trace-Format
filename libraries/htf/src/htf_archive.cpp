/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include "htf/htf_archive.h"
#include "htf/htf.h"
#include "htf/htf_dbg.h"
#include "htf/htf_write.h"

namespace htf {

/**
 * Getter for a String from its id.
 * @returns First String matching the given htf::StringRef, nullptr if it doesn't have a match.
 */
const String* Definition::getString(StringRef string_ref) const {
  /* TODO: race condition here ? when adding a region, there may be a realloc so we may have to hold the mutex */
  for (auto& s : strings) {
    if (s.string_ref == string_ref) {
      return &s;
    }
  }
  return nullptr;
}

/**
 * Creates a new String and adds it to that definition. Error if the given htf::StringRef is already in use.
 */
void Definition::addString(StringRef string_ref, const char* string) {
  if (getString(string_ref)) {
    htf_error("Given string_ref was already in use.\n");
  }

  auto s = String();
  s.string_ref = string_ref;
  s.length = strlen(string) + 1;
  s.str = new char[s.length];
  strncpy(s.str, string, s.length);
  strings.push_back(s);

  htf_log(DebugLevel::Verbose, "Register string #%zu{.ref=%x, .length=%d, .str='%s'}\n", strings.size() - 1,
          s.string_ref, s.length, s.str);
}

/**
 * Getter for a Region from its id.
 * @returns First Region matching the given htf::RegionRef, nullptr if it doesn't have a match.
 */
const Region* Definition::getRegion(RegionRef region_ref) const {
  /* TODO: race condition here ? when adding a region, there may be a realloc so we may have to hold the mutex */
  for (auto& r : regions) {
    if (r.region_ref == region_ref) {
      return &r;
    }
  }
  return nullptr;
}

/**
 * Creates a new Region and adds it to that definition. Error if the given htf::RegionRef is already in use.
 */
void Definition::addRegion(RegionRef region_ref, StringRef string_ref) {
  if (getRegion(region_ref)) {
    htf_error("Given region_ref was already in use.\n");
  }

  auto r = Region();
  r.region_ref = region_ref;
  r.string_ref = string_ref;
  regions.push_back(r);

  htf_log(DebugLevel::Verbose, "Register region #%zu{.ref=%x, .str=%d}\n", regions.size() - 1, r.region_ref,
          r.string_ref);
}

/**
 * Getter for a Attribute from its id.
 * @returns First Attribute matching the given htf::AttributeRef, nullptr if it doesn't have a match.
 */
const Attribute* Definition::getAttribute(AttributeRef attribute_ref) const {
  /* TODO: race condition here ? when adding a region, there may be a realloc so we may have to hold the mutex */
  for (auto& a : attributes) {
    if (a.attribute_ref == attribute_ref) {
      return &a;
    }
  }
  return nullptr;
}

/**
 * Creates a new Attribute and adds it to that definition. Error if the given htf::AttributeRef is already in use.
 */
void Definition::addAttribute(AttributeRef attribute_ref,
                              StringRef name_ref,
                              StringRef description_ref,
                              htf_type_t type) {
  if (getAttribute(attribute_ref)) {
    htf_error("Given attribute_ref was already in use.\n");
  }
  auto a = Attribute();
  a.attribute_ref = attribute_ref;
  a.name = name_ref;
  a.description = description_ref;
  a.type = type;
  attributes.push_back(a);

  htf_log(DebugLevel::Verbose, "Register attribute #%zu{.ref=%x, .name=%d, .description=%d, .type=%d}\n",
          attributes.size() - 1, a.attribute_ref, a.name, a.description, a.type);
}

/**
 * Getter for a String from its id.
 * @returns First String matching the given htf::StringRef in this archive, or in the global_archive if it doesn't
 * have a match, or nullptr if it doesn't have a match in the global_archive.
 */
const String* Archive::getString(StringRef string_ref) const {
  auto res = definitions->getString(string_ref);
  return (res) ? res : (global_archive) ? global_archive->getString(string_ref) : nullptr;
}

/**
 * Getter for a Region from its id.
 * @returns First Region matching the given htf::RegionRef in this archive, or in the global_archive if it doesn't
 * have a match, or nullptr if it doesn't have a match in the global_archive.
 */
const Region* Archive::getRegion(RegionRef region_ref) const {
  auto res = definitions->getRegion(region_ref);
  return (res) ? res : (global_archive) ? global_archive->getRegion(region_ref) : nullptr;
}

/**
 * Getter for a Attribute from its id.
 * @returns First Attribute matching the given htf::AttributeRef in this archive, or in the global_archive if it
 * doesn't have a match, or nullptr if it doesn't have a match in the global_archive.
 */
const Attribute* Archive::getAttribute(AttributeRef attribute_ref) const {
  auto res = definitions->getAttribute(attribute_ref);
  return (res) ? res : (global_archive) ? global_archive->getAttribute(attribute_ref) : nullptr;
}

/**
 * Getter for a Thread from its id.
 * @returns First Thread matching the given htf::ThreadId, or nullptr if it doesn't have a match.
 */
Thread* Archive::getThread(ThreadId thread_id) const {
  for (int i = 0; i < nb_threads; i++) {
    if (threads[i]->id == thread_id)
      return threads[i];
  }
  return nullptr;
}

/**
 * Getter for a LocationGroup from its id.
 * @returns First LocationGroup matching the given htf::LocationGroupId in this archive, or in the global_archive if it
 * doesn't have a match, or nullptr if it doesn't have a match in the global_archive.
 */
const LocationGroup* Archive::getLocationGroup(LocationGroupId location_group_id) const {
  for (auto& lc : location_groups) {
    if (lc.id == location_group_id) {
      return &lc;
    }
  }
  return (global_archive) ? global_archive->getLocationGroup(location_group_id) : nullptr;
  // The global_archive is the only one for which the global_archive field is nullptr
}

/**
 * Getter for a Location from its id.
 * @returns First Location matching the given htf::ThreadId in this archive, or in the global_archive if it
 * doesn't have a match, or nullptr if it doesn't have a match in the global_archive.
 */
const Location* Archive::getLocation(ThreadId location_id) const {
  for (auto& l : locations) {
    if (l.id == location_id) {
      return &l;
    }
  }
  return (global_archive) ? global_archive->getLocation(location_id) : nullptr;
  // The global_archive is the only one for which the global_archive field is nullptr
}

/**
 * Creates a new String and adds it to that Archive.
 * Error if the given htf::StringRef is already in use.
 * Locks and unlocks the mutex for that operation.
 */
void Archive::addString(StringRef string_ref, const char* string) {
  pthread_mutex_lock(&lock);
  definitions->addString(string_ref, string);
  pthread_mutex_unlock(&lock);
}

/**
 * Creates a new Region and adds it to that Archive.
 * Error if the given htf::RegionRef is already in use.
 * Locks and unlocks the mutex for that operation.
 */
void Archive::addRegion(RegionRef region_ref, StringRef name_ref) {
  pthread_mutex_lock(&lock);
  definitions->addRegion(region_ref, name_ref);
  pthread_mutex_unlock(&lock);
}

/**
 * Creates a new Attribute and adds it to that Archive.
 * Error if the given htf::AttributeRef is already in use.
 * Locks and unlocks the mutex for that operation.
 */
void Archive::addAttribute(AttributeRef attribute_ref, StringRef name_ref, StringRef description_ref, htf_type_t type) {
  pthread_mutex_lock(&lock);
  definitions->addAttribute(attribute_ref, name_ref, description_ref, type);
  pthread_mutex_unlock(&lock);
}
} /* namespace htf*/

/********************** C Bindings **********************/
htf::Archive* htf_archive_new() {
  return new htf::Archive();
};

htf::Thread* htf_archive_get_thread(htf::Archive* archive, htf::ThreadId thread_id) {
  return archive->getThread(thread_id);
};

const htf::LocationGroup* htf_archive_get_location_group(htf::Archive* archive, htf::LocationGroupId location_group) {
  return archive->getLocationGroup(location_group);
};

const htf::Location* htf_archive_get_location(htf::Archive* archive, htf::ThreadId threadId) {
  return archive->getLocation(threadId);
}
void htf_archive_register_string(htf::Archive* archive, htf::StringRef string_ref, const char* string) {
  archive->addString(string_ref, string);
}
void htf_archive_register_region(htf::Archive* archive, htf::RegionRef region_ref, htf::StringRef string_ref) {
  archive->addRegion(region_ref, string_ref);
}
void htf_archive_register_attribute(htf::Archive* archive,
                                    htf::AttributeRef attribute_ref,
                                    htf::StringRef name_ref,
                                    htf::StringRef description_ref,
                                    htf::htf_type_t type) {
  archive->addAttribute(attribute_ref, name_ref, description_ref, type);
}
const htf::String* htf_archive_get_string(htf::Archive* archive, htf::StringRef string_ref) {
  return archive->getString(string_ref);
}
const htf::Region* htf_archive_get_region(htf::Archive* archive, htf::RegionRef region_ref) {
  return archive->getRegion(region_ref);
}
const htf::Attribute* htf_archive_get_attribute(htf::Archive* archive, htf::AttributeRef attribute_ref) {
  return archive->getAttribute(attribute_ref);
}

/* -*-
  mode: c++;
  c-file-style: "k&r";
  c-basic-offset 2;
  tab-width 2 ;
  indent-tabs-mode nil
  -*- */
