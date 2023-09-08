/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include <stdlib.h>

#include "htf.h"
#include "htf_archive.h"
#include "htf_write.h"
#include "htf_dbg.h"

/** Returns the first htf_string corresponding to the given string_ref in the definition group.
 * Returns NULL if the ref does not have a match.
 */
static struct htf_string* _htf_archive_get_string_generic(struct htf_definition* d, htf_string_ref_t string_ref) {
  /* TODO: race condition here ? when adding a region, there may be a realloc so we may have to hold the mutex */
  for (int i = 0; i < d->nb_strings; i++) {
    struct htf_string* s = &d->strings[i];
    if (s->string_ref == string_ref) {
      return s;
    }
  }
  return NULL;
}

/** Returns the first htf_string corresponding to the given string_ref in the archive.
 * If the ref does not match, checks the global archive.
 * Returns NULL if the ref does not have a match.
 */
struct htf_string* htf_archive_get_string(struct htf_archive* archive, htf_string_ref_t string_ref) {
  struct htf_string* res = _htf_archive_get_string_generic(&archive->definitions, string_ref);
  if (!res && archive->global_archive)
    res = htf_archive_get_string(archive->global_archive, string_ref);
  return res;
}

const char* htf_archive_get_string_str(struct htf_archive* archive, htf_string_ref_t string_ref) {
  struct htf_string* res = htf_archive_get_string(archive, string_ref);
  if(res) return res->str;
  return "(null)";
}

/** Returns the first htf_region corresponding to the given region_ref in the definition group.
 * Returns NULL if the ref does not have a match.
 */
static struct htf_region* _htf_archive_get_region_generic(struct htf_definition* d, htf_region_ref_t region_ref) {
  /* TODO: race condition here ? when adding a region, there may be a realloc so we may have to hold the mutex */
  for (int i = 0; i < d->nb_regions; i++) {
    struct htf_region* s = &d->regions[i];
    if (s->region_ref == region_ref) {
      return s;
    }
  }
  return NULL;
}

/** Returns the first htf_region corresponding to the given region_ref in the archive.
 * If the ref does not match, checks the global archive.
 * Returns NULL if the ref does not have a match.
 */
struct htf_region* htf_archive_get_region(struct htf_archive* archive, htf_region_ref_t region_ref) {
  struct htf_region* res = _htf_archive_get_region_generic(&archive->definitions, region_ref);
  if (!res && archive->global_archive)
    res = htf_archive_get_region(archive->global_archive, region_ref);
  return res;
}

/** Adds the given string to the given definition and sets its ref.
 * Error is raised if htf_string_ref is already in the definition or malloc fails.
 */
void htf_archive_register_string_generic(struct htf_definition* d, htf_string_ref_t string_ref, const char* string) {
  if (_htf_archive_get_string_generic(d, string_ref) != NULL) {
    htf_error("Given string_ref was already in use.\n");
  }
  // Check if the definition set has enough allocated strings
  if (d->nb_strings >= d->nb_allocated_strings) {
    DOUBLE_MEMORY_SPACE(d->strings, d->nb_allocated_strings, struct htf_string);
  }

  int index = d->nb_strings++;
  struct htf_string* s = &d->strings[index];
  s->string_ref = string_ref;
  s->length = strlen(string) + 1;
  s->str = malloc(sizeof(char) * s->length);
  strncpy(s->str, string, s->length);

  htf_log(htf_dbg_lvl_verbose, "Register string #%d{.ref=%x, .length=%d, .str='%s'}\n", index, s->string_ref, s->length,
          s->str);
}

/** Adds the given string to the given archive and sets its ref. */
void htf_archive_register_string(struct htf_archive* archive, htf_string_ref_t string_ref, const char* string) {
  pthread_mutex_lock(&archive->lock);
  htf_archive_register_string_generic(&archive->definitions, string_ref, string);
  pthread_mutex_unlock(&archive->lock);
}

/** Adds the given region to the given definition and sets its ref.
 * Error is raised if htf_region_ref is already in the definition or malloc fails.
 */
void htf_archive_register_region_generic(struct htf_definition* d,
                                         htf_region_ref_t region_ref,
                                         htf_string_ref_t string_ref) {
  if (_htf_archive_get_region_generic(d, region_ref) != NULL) {
    htf_error("Given region_ref was already in use.\n");
  }

  if (d->nb_regions >= d->nb_allocated_regions) {
    DOUBLE_MEMORY_SPACE(d->regions, d->nb_allocated_regions, struct htf_region);
  }

  int index = d->nb_regions++;
  struct htf_region* r = &d->regions[index];
  r->region_ref = region_ref;
  r->string_ref = string_ref;

  htf_log(htf_dbg_lvl_verbose, "Register region #%d{.ref=%x, .str=%d}\n", index, r->region_ref, r->string_ref);
}

/** Adds the given region to the given archive and sets its ref. */
void htf_archive_register_region(struct htf_archive* archive,
                                 htf_region_ref_t region_ref,
                                 htf_string_ref_t string_ref) {
  pthread_mutex_lock(&archive->lock);
  htf_archive_register_region_generic(&archive->definitions, region_ref, string_ref);
  pthread_mutex_unlock(&archive->lock);
}

/** Returns the first htf_attribute corresponding to the given attribute_ref in the definition group.
 * Returns NULL if the ref does not have a match.
 */
static struct htf_attribute* _htf_archive_get_attribute_generic(struct htf_definition* d, htf_attribute_ref_t attribute_ref) {
  /* TODO: race condition here ? when adding a attribute, there may be a realloc so we may have to hold the mutex */
  for (int i = 0; i < d->nb_attributes; i++) {
    struct htf_attribute* s = &d->attributes[i];
    if (s->attribute_ref == attribute_ref) {
      return s;
    }
  }
  return NULL;
}

/** Returns the first htf_attribute corresponding to the given attribute_ref in the archive.
 * If the ref does not match, checks the global archive.
 * Returns NULL if the ref does not have a match.
 */
struct htf_attribute* htf_archive_get_attribute(struct htf_archive* archive, htf_attribute_ref_t attribute_ref) {
  struct htf_attribute* res = _htf_archive_get_attribute_generic(&archive->definitions, attribute_ref);
  if (!res && archive->global_archive)
    res = htf_archive_get_attribute(archive->global_archive, attribute_ref);
  return res;
}

/** Adds the given attribute to the given definition and sets its ref.
 * Error is raised if htf_attribute_ref is already in the definition or malloc fails.
 */
void htf_archive_register_attribute_generic(struct htf_definition* d,
					    htf_attribute_ref_t attribute_ref,
					    htf_string_ref_t name_ref,
					    htf_string_ref_t description_ref,
					    htf_type_t type) {
  if (_htf_archive_get_attribute_generic(d, attribute_ref) != NULL) {
    htf_error("Given attribute_ref was already in use.\n");
  }

  if (d->nb_attributes >= d->nb_allocated_attributes) {
    if(d->nb_allocated_attributes == 0) {
      d->attributes = malloc(sizeof(struct htf_attribute) * NB_ATTRIBUTE_DEFAULT);
      if (d->attributes == NULL) {
	htf_error("Failed to allocate memory\n");
      }
      d->nb_allocated_attributes = NB_ATTRIBUTE_DEFAULT;
    } else {
      DOUBLE_MEMORY_SPACE(d->attributes, d->nb_allocated_attributes, struct htf_attribute);
    }
  }

  int index = d->nb_attributes++;
  struct htf_attribute* a = &d->attributes[index];
  a->attribute_ref = attribute_ref;
  a->name = name_ref;
  a->description = description_ref;
  a->type = type;

  htf_log(htf_dbg_lvl_verbose, "Register attribute #%d{.ref=%x, .name=%d, .description=%d, .type=%d}\n", index, a->attribute_ref, a->name, a->description, a->type);
}


/** Adds the given attribute to the given archive and sets its ref. */
void htf_archive_register_attribute(struct htf_archive* archive,
				    htf_attribute_ref_t attribute_ref,
				    htf_string_ref_t name_ref,
				    htf_string_ref_t description_ref,
				    htf_type_t type) {
  pthread_mutex_lock(&archive->lock);
  htf_archive_register_attribute_generic(&archive->definitions, attribute_ref, name_ref, description_ref, type);
  pthread_mutex_unlock(&archive->lock);
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
