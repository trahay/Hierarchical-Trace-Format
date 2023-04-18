#include <stdlib.h>

#include "htf.h"
#include "htf_dbg.h"
#include "htf_archive.h"

/* Returns the first htf_string corresponding to the given string_ref in the definition group.
 * Returns NULL if the ref does not have a match.
 */
static struct htf_string* _htf_archive_get_string_generic(struct htf_definition *d,
							  htf_string_ref_t string_ref) {
  /* TODO: race condition here ? when adding a region, there may be a realloc so we may have to hold the mutex */
  for(int i = 0; i< d->nb_strings; i++) {
    struct htf_string* s = &d->strings[i];
    if(s->string_ref == string_ref) {
      return s;
    }
  }
  return NULL;
}

/* Returns the first htf_string corresponding to the given string_ref in the archive.
 * If the ref does not match, checks the global archive.
 * Returns NULL if the ref does not have a match.
 */
struct htf_string* htf_archive_get_string(struct htf_archive *archive,
					  htf_string_ref_t string_ref) {
  struct htf_string* res = _htf_archive_get_string_generic(&archive->definitions, string_ref);
  if(!res && archive->global_archive)
    res = htf_archive_get_string(archive->global_archive, string_ref);
  return res;
}

/* Returns the first htf_region corresponding to the given region_ref in the definition group.
 * Returns NULL if the ref does not have a match.
 */
static struct htf_region* _htf_archive_get_region_generic(struct htf_definition *d,
							  htf_region_ref_t region_ref) {
	/* TODO: race condition here ? when adding a region, there may be a realloc so we may have to hold the mutex */
  for(int i = 0; i< d->nb_regions; i++) {
    struct htf_region* s = &d->regions[i];
    if(s->region_ref == region_ref) {
      return s;
    }
  }
  return NULL;
}

/* Returns the first htf_region corresponding to the given region_ref in the archive.
 * If the ref does not match, checks the global archive.
 * Returns NULL if the ref does not have a match.
 */
struct htf_region* htf_archive_get_region(struct htf_archive *archive,
					  htf_region_ref_t region_ref) {
  struct htf_region* res = _htf_archive_get_region_generic(&archive->definitions, region_ref);
  if(!res && archive->global_archive)
    res = htf_archive_get_region(archive->global_archive, region_ref);
  return res;
}

/* Adds the given string to the given definition and sets its ref.
 * Error is raised if htf_string_ref is already in the definition or malloc fails.
 */
void htf_archive_register_string_generic(struct htf_definition *d,
				 htf_string_ref_t string_ref,
				 const char* string) {
	if (_htf_archive_get_string_generic(d, string_ref) != NULL) {
		htf_error("Given string_ref was already in use.\n");
	}
	// Check if the definition set has enough allocated strings
  if(d->nb_strings + 1 >= d->nb_allocated_strings) {
    d->nb_allocated_strings *= 2;
    d->strings = realloc(d->strings, d->nb_allocated_strings * sizeof(struct htf_string));
    if(d->strings == NULL) {
      htf_error("Failed to allocate memory\n");
    }
  }

	int index = d->nb_strings++;
	struct htf_string *s = &d->strings[index];
  s->string_ref = string_ref;
  s->length = strlen(string)+1;
  s->str = malloc(sizeof(char) * s->length);
  strncpy(s->str, string, s->length);

	htf_log(htf_dbg_lvl_verbose,
	        "Register string #%d{.ref=%x, .length=%d, .str='%s'}\n",
	        index,
	        s->string_ref,
	        s->length,
	        s->str);
}

/* Adds the given string to the given archive and sets its ref. */
void htf_archive_register_string(struct htf_archive *archive,
				 htf_string_ref_t string_ref,
				 const char* string) {
  pthread_mutex_lock(&archive->lock);
  htf_archive_register_string_generic(&archive->definitions, string_ref, string);
  pthread_mutex_unlock(&archive->lock);
}

/* Adds the given region to the given definition and sets its ref.
 * Error is raised if htf_region_ref is already in the definition or malloc fails.
 */
void htf_archive_register_region_generic(struct htf_definition *d,
					 htf_region_ref_t region_ref,
					 htf_string_ref_t string_ref) {

	if (_htf_archive_get_region_generic(d, region_ref) != NULL) {
		htf_error("Given region_ref was already in use.\n");
	}

  if(d->nb_regions + 1 >= d->nb_allocated_regions) {
    d->nb_allocated_regions *= 2;
    d->regions = realloc(d->regions, d->nb_allocated_regions * sizeof(struct htf_region));
    if(d->regions == NULL) {
      htf_error("Failed to allocate memory\n");
    }
  }

  int index = d->nb_regions++;
  struct htf_region *r = &d->regions[index];
  r->region_ref = region_ref;
  r->string_ref = string_ref;

  htf_log(htf_dbg_lvl_verbose, "Register region #%d{.ref=%x, .str=%d}\n",
	  index, r->region_ref, r->string_ref);
}

/* Adds the given region to the given archive and sets its ref. */
void htf_archive_register_region(struct htf_archive *archive,
				 htf_region_ref_t region_ref,
				 htf_string_ref_t string_ref) {
  pthread_mutex_lock(&archive->lock);
  htf_archive_register_region_generic(&archive->definitions, region_ref, string_ref);
  pthread_mutex_unlock(&archive->lock);
}