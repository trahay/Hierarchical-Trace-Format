/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zstd.h>

#include "htf.h"
#include "htf_dbg.h"
#include "htf_hash.h"
#include "htf_read.h"
#include "htf_storage.h"

static enum storage_compression_option COMPRESSION_OPTIONS = ZSTD;
short STORE_TIMESTAMPS = 1;
static short STORE_HASHING = 0;
void htf_storage_option_init() {
  // Compression options
  char* compression_str = getenv("COMPRESSION");
  if (compression_str) {
    if (strcmp(compression_str, "ZSTD") == 0)
      COMPRESSION_OPTIONS = ZSTD;
    else if (strcmp(compression_str, "MASKING") == 0)
      COMPRESSION_OPTIONS = MASKING;
    else if (strcmp(compression_str, "MASKING_ZSTD") == 0)
      COMPRESSION_OPTIONS = MASKING_ZSTD;
    else if (strcmp(compression_str, "NONE") == 0)
      COMPRESSION_OPTIONS = NO_COMPRESSION;
  }

  // Timestamp storage
  char* store_timestamps_str = getenv("STORE_TIMESTAMPS");
  if (store_timestamps_str && strcmp(store_timestamps_str, "TRUE") != 0)
    STORE_TIMESTAMPS = 0;

  // Store hash for sequences
  char* store_hashing_str = getenv("STORE_HASHING");
  if (store_hashing_str && strcmp(store_hashing_str, "FALSE") != 0)
    STORE_HASHING = 1;
}
static void _htf_store_event(const char* base_dirname,
                             struct htf_thread* th,
                             struct htf_event_summary* e,
                             htf_event_id_t event_id);
static void _htf_store_sequence(const char* base_dirname,
                                struct htf_thread* th,
                                struct htf_sequence* s,
                                htf_sequence_id_t sequence_id);
static void _htf_store_loop(const char* base_dirname, struct htf_thread* th, struct htf_loop* l, htf_loop_id_t loop_id);

static void _htf_store_string(struct htf_archive* c, struct htf_string* l, int string_index);
static void _htf_store_regions(struct htf_archive* c);
static void _htf_store_attributes(struct htf_archive* c);

static void _htf_store_location_groups(struct htf_archive* a);
static void _htf_store_locations(struct htf_archive* a);

static void _htf_read_event(const char* base_dirname,
                            struct htf_thread* th,
                            struct htf_event_summary* e,
                            htf_event_id_t event_id);
static void _htf_read_sequence(const char* base_dirname,
                               struct htf_thread* th,
                               struct htf_sequence* s,
                               htf_sequence_id_t sequence_id);
static void _htf_read_loop(const char* base_dirname, struct htf_thread* th, struct htf_loop* l, htf_loop_id_t loop_id);

static void _htf_read_string(struct htf_archive* c, struct htf_string* l, int string_index);
static void _htf_read_regions(struct htf_archive* c);
static void _htf_read_attributes(struct htf_archive* c);
static void _htf_read_location_groups(struct htf_archive* a);
static void _htf_read_locations(struct htf_archive* a);

void htf_read_thread(struct htf_archive* archive, htf_thread_id_t thread_id);

static struct htf_archive* _htf_get_archive(struct htf_archive* global_archive, htf_location_group_id_t archive_id);

static void _htf_mkdir(char* dirname, mode_t mode) {
  if (mkdir(dirname, mode) != 0) {
    if (errno != EEXIST)
      htf_error("mkdir(%s) failed: %s\n", dirname, strerror(errno));
  }
}

static FILE* _htf_file_open(char* filename, char* mode) {
  htf_log(htf_dbg_lvl_debug, "Open %s with mode %s\n", filename, mode);
  char* filename_copy = strdup(filename);
  _htf_mkdir(dirname(filename_copy), 0777);
  free(filename_copy);

  FILE* file = fopen(filename, mode);
  if (file == NULL) {
    htf_error("Cannot open %s: %s\n", filename, strerror(errno));
  }
  return file;
}

#define _htf_fread(ptr, size, nmemb, stream)      \
  do {                                            \
    size_t ret = fread(ptr, size, nmemb, stream); \
    if (ret != nmemb)                             \
      htf_error("fread failed: %s\n", strerror(errno)); \
  } while (0)

#define _htf_fwrite(ptr, size, nmemb, stream)      \
  do {                                             \
    size_t ret = fwrite(ptr, size, nmemb, stream); \
    if (ret != nmemb)                              \
      htf_error("fwrite failed\n"); \
  } while (0)

/******************* Read/Write/Compression function for vectors and arrays *******************/

/** Compresses the content in src using ZSTD and writes it to dest. Returns the size of the compressed array.
 *  - void* src: what's going to be compressed.
 *  - void* dest: a free array in which the compressed array will be written.
 *  - size_t size: size of src (and of the allocated memory in dest).
 */
inline static size_t _htf_zstd_compress(void* src, size_t size, void* dest, size_t dest_size) {
  return ZSTD_compress(dest, dest_size, src, size, ZSTD_COMPRESSION_LEVEL);
}
/** Compresses the content in src using a Masking technique and writes it to dest.
 * Returns the size of the compressed array.
 *  - void* src: what's going to be compressed.
 *  - void* dest: a free array in which the compressed array will be written.
 *  - size_t size: size of src (and of the allocated memory in dest).
 */
inline static size_t _htf_masking_compress(void* src, void* dest, size_t size) {
  // We must be careful of size isn't a multiple of 8
  if (size % sizeof(htf_timestamp_t) != 0) {
    memcpy(dest, src, size);
    return size;
  }

  size_t n = size / sizeof(htf_timestamp_t);
  uint64_t* new_src = src;
  uint64_t mask = 0;
  for (int i = 0; i < n; i++) {
    mask |= new_src[i];
  }
  short mask_size = 0;
  while (mask != 0) {
    mask >>= 8;
    mask_size += 1;
  }
  if (mask_size && mask_size != sizeof(htf_timestamp_t)) {
    for (int i = 0; i < n; i++) {
      memcpy(&((uint8_t*)dest)[sizeof(htf_timestamp_t) * i], &new_src[i], mask_size);
    }
    return mask_size * n;
  } else {
    memcpy(dest, src, size);
    return size;
  }
}

/** Writes the array to the given file, but compresses it before, according to the value of COMPRESSION_OPTIONS.
 * - void* array: the array to be written.
 * - size_t size: size of said array.
 * - FILE*  file: file to write in.
 */
inline static void _htf_compress_write(void* array, size_t size, FILE* file) {
  size_t compSize;
  void* compArray = malloc(size);
  int mustFree = 1;

  switch (COMPRESSION_OPTIONS) {
  case NO_COMPRESSION:
    compArray = array;
    compSize = size;
    mustFree = 0;
    memcpy(compArray, array, size);
    break;
  case ZSTD: {
    compSize = ZSTD_compressBound(size);
    compArray = malloc(compSize);
    compSize = _htf_zstd_compress(array, size, compArray, compSize);
  } break;
  case MASKING:
    compArray = malloc(size);
    compSize = _htf_masking_compress(array, compArray, size);
    break;
  case MASKING_ZSTD: {
    void* buffer = malloc(size);
    size_t maskedSize = _htf_masking_compress(array, buffer, size);
    compSize = ZSTD_compressBound(maskedSize);
    compArray = malloc(compSize);
    compSize = _htf_zstd_compress(buffer, maskedSize, compArray, compSize);
    free(buffer);
    break;
  }
  }
  htf_log(htf_dbg_lvl_normal, "Writing %lu bytes as %lu bytes\n", size, compSize);
  _htf_fwrite(&compSize, sizeof(compSize), 1, file);
  _htf_fwrite(compArray, compSize, 1, file);

  if (mustFree)
    free(compArray);
}
/** Decompresses an array that has been compressed by ZSTD. Returns the size of the uncompressed data.
 * - void * array : the array in which the uncompressed data will be written.
 * - void * compArray : the compressed array.
 * - size_t compSize : size of the compressed array
 */
inline static size_t _htf_zstd_read(void* array, void* compArray, size_t compSize) {
  size_t realSize = ZSTD_getFrameContentSize(compArray, compSize);
  ZSTD_decompress(array, realSize, compArray, compSize);
  return realSize;
}

/** Decompresses an array that has been compressed by the Masking technique. Returns the size of the uncompressed data.
 * - void * array : the array in which the uncompressed data will be written.
 * - size_t size: size of that array.
 * - void * compArray : the compressed array.
 * - size_t compSize : size of the compressed array
 */
inline static size_t _htf_masking_read(void* array, size_t size, void* compArray, size_t compSize) {
  if (compSize == size) {
    memcpy(array, compArray, size);
    return compSize;
  }
  size_t width = 1 << (size / compSize);
  memset(array, 0, size);
  for (int i = 0; i < size / sizeof(uint64_t); i++) {
    memcpy(&((uint8_t*)array)[i], &((uint8_t*)compArray)[size * i], width);
  }
  return width * (size / sizeof(uint64_t));
}

/** Reads the array from the given file, but decompresses it before, according to the value of COMPRESSION_OPTIONS.
 * - void* array: the array to be read.
 * - size_t size: size of said array.
 * - FILE*  file: file to read from.
 */
inline static void _htf_compress_read(void* array, size_t size, FILE* file) {
  size_t compSize;
  _htf_fread(&compSize, sizeof(compSize), 1, file);
  // Then we used ZSTD
  void* compArray = malloc(compSize);
  _htf_fread(compArray, compSize, 1, file);
  switch (COMPRESSION_OPTIONS) {
  case NO_COMPRESSION:
    memcpy(array, compArray, size);
    break;
  case ZSTD: {
    size_t realSize = _htf_zstd_read(array, compArray, compSize);
    htf_assert(realSize == size);
    break;
  }
  case MASKING:
    _htf_masking_read(array, size, compArray, compSize);
    break;
  case MASKING_ZSTD: {
    void* buffer = compArray;
    size_t maskedSize = _htf_zstd_read(array, buffer, compSize);
    compArray = malloc(maskedSize);
    _htf_masking_read(buffer, size, compArray, maskedSize);
    free(buffer);
    break;
  }
  }
  free(compArray);
}

/** Writes a vector to the given file.*/
inline static void _htf_vector_fwrite(htf_vector_t* vector, FILE* file) {
  _htf_fwrite(&vector->size, sizeof(vector->size), 1, file);
  if (vector->size == 0) {
    return;
  }
  void* buffer = malloc(vector->element_size * vector->size);
  uint cur_index = 0;
  htf_subvector_t* sub_vec = vector->first_subvector;
  while (sub_vec != NULL) {
    memcpy((void*)((size_t)buffer + cur_index * vector->element_size), sub_vec->array,
           sub_vec->size * vector->element_size);
    cur_index += sub_vec->size;
    sub_vec = sub_vec->next;
  }
  htf_assert(cur_index == vector->size);
  _htf_compress_write(buffer, vector->size * vector->element_size, file);
  free(buffer);
}

/** Reads a vector from the given file.*/
inline static void _htf_vector_fread(htf_vector_t* vector, size_t element_size, FILE* file) {
  _htf_fread(&vector->size, sizeof(vector->size), 1, file);
  vector->element_size = element_size;
  if (vector->size != 0) {
    vector->last_subvector = malloc(sizeof(htf_subvector_t));
    vector->first_subvector = vector->last_subvector;
    vector->first_subvector->array = malloc(vector->element_size * vector->size);
    vector->first_subvector->size = vector->size;
    vector->first_subvector->allocated = vector->size;
    vector->first_subvector->starting_index = 0;
    vector->first_subvector->next = NULL;
    vector->first_subvector->previous = NULL;
    _htf_compress_read(vector->first_subvector->array, vector->element_size * vector->size, file);
  } else {
    vector->first_subvector = NULL;
    vector->last_subvector = NULL;
  }
}

/**************** Storage Functions ****************/

void htf_storage_init(struct htf_archive* archive) {
  _htf_mkdir(archive->dir_name, 0777);
  htf_storage_option_init();
}

static const char* base_dirname(struct htf_archive* a) {
  return a->dir_name;
}

static FILE* _htf_get_event_file(const char* base_dirname, struct htf_thread* th, htf_event_id_t event_id, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u/event_%d", base_dirname, th->id, HTF_ID(event_id));
  return _htf_file_open(filename, mode);
}


static void _htf_store_attribute_values(struct htf_event_summary* e,
					FILE* file) {  
  _htf_fwrite(&e->attribute_pos, sizeof(e->attribute_pos), 1, file);
  if(e->attribute_pos > 0) {
    htf_log(htf_dbg_lvl_debug, "\t\tStore %lu attributes\n", e->attribute_pos);
    _htf_fwrite(e->attribute_buffer, e->attribute_pos, 1, file);
  }
}

static void _htf_store_event(const char* base_dirname,
                             struct htf_thread* th,
                             struct htf_event_summary* e,
                             htf_event_id_t event_id) {
  FILE* file = _htf_get_event_file(base_dirname, th, event_id, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore event %x {.nb_events=%d}\n", HTF_ID(event_id), e->nb_occurrences);

  _htf_fwrite(&e->event, sizeof(struct htf_event), 1, file);
  _htf_fwrite(&e->nb_occurrences, sizeof(e->nb_occurrences), 1, file);
  _htf_store_attribute_values(e, file);
  if (STORE_TIMESTAMPS) {
    _htf_vector_fwrite(&e->durations, file);
  }
  fclose(file);
}

static void _htf_read_attribute_values(struct htf_event_summary* e,
				 FILE* file) {

  _htf_fread(&e->attribute_pos, sizeof(e->attribute_pos), 1, file);
  e->attribute_buffer_size = e->attribute_pos;
  e->attribute_pos = 0;
  e->attribute_buffer = NULL;

  if(e->attribute_buffer_size > 0) {
    e->attribute_buffer = malloc(e->attribute_buffer_size);
    if(e->attribute_buffer == 0) {
      htf_error("Cannot allocate memory\n");
    }
    _htf_fread(e->attribute_buffer, e->attribute_buffer_size, 1, file);
  }
}

static void _htf_read_event(const char* base_dirname,
                            struct htf_thread* th,
                            struct htf_event_summary* e,
                            htf_event_id_t event_id) {
  FILE* file = _htf_get_event_file(base_dirname, th, event_id, "r");

  _htf_fread(&e->event, sizeof(struct htf_event), 1, file);
  _htf_fread(&e->nb_occurrences, sizeof(e->nb_occurrences), 1, file);
  htf_log(htf_dbg_lvl_debug, "\tLoad event %x {.nb_events=%d}\n", HTF_ID(event_id), e->nb_occurrences);
  _htf_read_attribute_values(e, file);
  if (STORE_TIMESTAMPS) {
    _htf_vector_fread(&e->durations, sizeof(htf_timestamp_t), file);
  } else {
    e->durations.size = 0;
  }
  fclose(file);
}

static FILE* _htf_get_sequence_file(const char* base_dirname,
                                    struct htf_thread* th,
                                    htf_sequence_id_t sequence_id,
                                    char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u/sequence_%d", base_dirname, th->id, HTF_ID(sequence_id));
  return _htf_file_open(filename, mode);
}

static void _htf_store_sequence(const char* base_dirname,
                                struct htf_thread* th,
                                struct htf_sequence* s,
                                htf_sequence_id_t sequence_id) {
  FILE* file = _htf_get_sequence_file(base_dirname, th, sequence_id, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore sequence %x {.size=%d, .nb_ts=%u}\n", HTF_ID(sequence_id), s->size,
          s->durations.size);
  if (htf_debug_level >= htf_dbg_lvl_debug) {
    htf_print_sequence(th, sequence_id);
  }

  _htf_fwrite(&s->size, sizeof(s->size), 1, file);
  _htf_fwrite(s->token, sizeof(s->token[0]), s->size, file);
  if (STORE_HASHING) {
    if (!s->hash) {
      htf_hash_32(s->token, s->size, SEED, &s->hash);
    }
    _htf_fwrite(&s->hash, sizeof(s->hash), 1, file);
  }
  if (STORE_TIMESTAMPS) {
    _htf_vector_fwrite(&s->durations, file);
  }
  fclose(file);
}

static void _htf_read_sequence(const char* base_dirname,
                               struct htf_thread* th,
                               struct htf_sequence* s,
                               htf_sequence_id_t sequence_id) {
  FILE* file = _htf_get_sequence_file(base_dirname, th, sequence_id, "r");
  _htf_fread(&s->size, sizeof(s->size), 1, file);
  s->token = malloc(sizeof(htf_token_t) * s->size);
  s->allocated = s->size;
  _htf_fread(s->token, sizeof(htf_token_t), s->size, file);
  if (STORE_HASHING) {
    uint32_t stored_hash;
    _htf_fread(&stored_hash, sizeof(stored_hash), 1, file);
    htf_hash_32(s->token, s->size, SEED, &s->hash);
    htf_assert(stored_hash == s->hash);
  }
  if (STORE_TIMESTAMPS) {
    _htf_vector_fread(&s->durations, sizeof(htf_timestamp_t), file);
  }
  fclose(file);

  htf_log(htf_dbg_lvl_debug, "\tLoad sequence %x {.size=%u, .nb_ts=%u}\n", HTF_ID(sequence_id), s->size,
          s->durations.size);

  if (htf_debug_level >= htf_dbg_lvl_debug) {
    htf_print_sequence(th, sequence_id);
  }
}

static FILE* _htf_get_loop_file(const char* base_dirname, struct htf_thread* th, htf_loop_id_t loop_id, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u/loop_%d", base_dirname, th->id, HTF_ID(loop_id));
  return _htf_file_open(filename, mode);
}

static void _htf_store_loop(const char* base_dirname,
                            struct htf_thread* th,
                            struct htf_loop* l,
                            htf_loop_id_t loop_id) {
  FILE* file = _htf_get_loop_file(base_dirname, th, loop_id, "w");
  if (htf_debug_level >= htf_dbg_lvl_debug) {
    htf_log(htf_dbg_lvl_debug, "\tStore loops %x {.nb_loops=%d, .token=%x.%x, .nb_iterations:", HTF_ID(loop_id),
            l->nb_iterations.size, l->token.type, l->token.id);
    htf_vector_print_as_int(&l->nb_iterations);
    printf("}\n");
  }
  _htf_fwrite(&l->token, sizeof(l->token), 1, file);
  _htf_vector_fwrite(&l->nb_iterations, file);
  fclose(file);
}

static void _htf_read_loop(const char* base_dirname, struct htf_thread* th, struct htf_loop* l, htf_loop_id_t loop_id) {
  FILE* file = _htf_get_loop_file(base_dirname, th, loop_id, "r");
  l->id = HTF_TOKENIZE(HTF_TYPE_LOOP, loop_id.id);
  _htf_fread(&l->token, sizeof(l->token), 1, file);
  _htf_vector_fread(&l->nb_iterations, sizeof(int), file);
  fclose(file);
  if (htf_debug_level >= htf_dbg_lvl_debug) {
    htf_log(htf_dbg_lvl_debug, "\tLoad loops %x {.nb_loops=%d, .token=%x.%x, .nb_iterations: ", HTF_ID(loop_id),
            l->nb_iterations.size, l->token.type, l->token.id);
    htf_vector_print_as_int(&l->nb_iterations);
    printf("}\n");
  }
}

static FILE* _htf_get_string_file(struct htf_archive* a, int string_index, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/string_%d", base_dirname(a), a->id, string_index);
  return _htf_file_open(filename, mode);
}

static void _htf_store_string_generic(FILE* file, struct htf_string* s, int string_index) {
  htf_log(htf_dbg_lvl_debug, "\tStore String %d {.ref=%d, .length=%d, .str='%s'}\n", string_index, s->string_ref,
          s->length, s->str);

  _htf_fwrite(&s->string_ref, sizeof(s->string_ref), 1, file);
  _htf_fwrite(&s->length, sizeof(s->length), 1, file);
  _htf_fwrite(s->str, sizeof(char), s->length, file);
}

static void _htf_store_string(struct htf_archive* a, struct htf_string* s, int string_index) {
  FILE* file = _htf_get_string_file(a, string_index, "w");
  _htf_store_string_generic(file, s, string_index);
  fclose(file);
}

static void _htf_read_string_generic(FILE* file, struct htf_string* s, int string_index) {
  _htf_fread(&s->string_ref, sizeof(s->string_ref), 1, file);
  _htf_fread(&s->length, sizeof(s->length), 1, file);
  s->str = malloc(sizeof(char) * s->length);
  htf_assert(s->str);
  _htf_fread(s->str, sizeof(char), s->length, file);
  htf_log(htf_dbg_lvl_debug, "\tLoad String %d {.ref=%d, .length=%d, .str='%s'}\n", string_index, s->string_ref,
          s->length, s->str);
}

static void _htf_read_string(struct htf_archive* a, struct htf_string* s, int string_index) {
  FILE* file = _htf_get_string_file(a, string_index, "r");
  _htf_read_string_generic(file, s, string_index);
  fclose(file);
}

static FILE* _htf_get_regions_file(struct htf_archive* a, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/regions.dat", base_dirname(a), a->id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_regions_generic(FILE* file, struct htf_definition* d) {
  if (d->nb_regions == 0)
    return;

  htf_log(htf_dbg_lvl_debug, "\tStore %d Regions\n", d->nb_regions);

  _htf_fwrite(d->regions, sizeof(struct htf_region), d->nb_regions, file);
}

static void _htf_store_regions(struct htf_archive* a) {
  if (a->definitions.nb_regions == 0)
    return;

  FILE* file = _htf_get_regions_file(a, "w");
  _htf_store_regions_generic(file, &a->definitions);
  fclose(file);
}


static void _htf_read_regions_generic(FILE* file, struct htf_definition* d) {
  if (d->nb_regions == 0)
    return;

  d->nb_allocated_regions = d->nb_regions;
  d->regions = malloc(sizeof(struct htf_region) * d->nb_allocated_regions);
  _htf_fread(d->regions, sizeof(struct htf_region), d->nb_allocated_regions, file);

  htf_log(htf_dbg_lvl_debug, "\tLoad %d regions\n", d->nb_regions);
}

static void _htf_read_regions(struct htf_archive* a) {
  if (a->definitions.nb_regions == 0)
    return;

  FILE* file = _htf_get_regions_file(a, "r");
  _htf_read_regions_generic(file, &a->definitions);
  fclose(file);
}


static FILE* _htf_get_attributes_file(struct htf_archive* a, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/attributes.dat", base_dirname(a), a->id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_attributes_generic(FILE* file, struct htf_definition* d) {
  if (d->nb_attributes == 0)
    return;

  htf_log(htf_dbg_lvl_debug, "\tStore %d Attributes\n", d->nb_attributes);
  for(int i=0; i< d->nb_attributes; i++) {
    htf_log(htf_dbg_lvl_debug, "\t\t[%d] {ref=%x, name=%x, type=%x}\n", i,
	    d->attributes[i].attribute_ref, d->attributes[i].name, d->attributes[i].type);
  }

  _htf_fwrite(d->attributes, sizeof(struct htf_attribute), d->nb_attributes, file);
}

static void _htf_store_attributes(struct htf_archive* a) {
  if (a->definitions.nb_attributes == 0)
    return;

  FILE* file = _htf_get_attributes_file(a, "w");
  _htf_store_attributes_generic(file, &a->definitions);
  fclose(file);
}

static void _htf_read_attributes_generic(FILE* file, struct htf_definition* d) {
  if (d->nb_attributes == 0)
    return;

  d->nb_allocated_attributes = d->nb_attributes;
  d->attributes = malloc(sizeof(struct htf_attribute) * d->nb_allocated_attributes);
  _htf_fread(d->attributes, sizeof(struct htf_attribute), d->nb_allocated_attributes, file);

  htf_log(htf_dbg_lvl_debug, "\tLoad %d attributes\n", d->nb_attributes);
}

static void _htf_read_attributes(struct htf_archive* a) {
  if (a->definitions.nb_attributes == 0)
    return;

  FILE* file = _htf_get_attributes_file(a, "r");
  _htf_read_attributes_generic(file, &a->definitions);
  fclose(file);
}

static FILE* _htf_get_location_groups_file(struct htf_archive* a, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/location_groups.dat", base_dirname(a), a->id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_location_groups(struct htf_archive* a) {
  if (a->nb_location_groups == 0)
    return;

  FILE* file = _htf_get_location_groups_file(a, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore %d location groupds\n", a->nb_location_groups);

  _htf_fwrite(a->location_groups, sizeof(struct htf_location_group), a->nb_location_groups, file);
  fclose(file);
}

static void _htf_read_location_groups(struct htf_archive* a) {
  if (a->nb_location_groups == 0)
    return;

  FILE* file = _htf_get_location_groups_file(a, "r");

  a->location_groups = malloc(sizeof(struct htf_location_group) * a->nb_location_groups);
  _htf_fread(a->location_groups, sizeof(struct htf_location_group), a->nb_location_groups, file);
  fclose(file);

  htf_log(htf_dbg_lvl_debug, "\tLoad %d location_groups\n", a->nb_location_groups);
}

static FILE* _htf_get_locations_file(struct htf_archive* a, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/locations.dat", base_dirname(a), a->id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_locations(struct htf_archive* a) {
  if (a->nb_locations == 0)
    return;

  FILE* file = _htf_get_locations_file(a, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore %d locations\n", a->nb_locations);

  for (int i = 0; i < a->nb_locations; i++) {
    htf_assert(a->locations[i].id != HTF_THREAD_ID_INVALID);
  }

  _htf_fwrite(a->locations, sizeof(struct htf_location), a->nb_locations, file);
  fclose(file);
}

static void _htf_read_locations(struct htf_archive* a) {
  if (a->nb_locations == 0)
    return;

  FILE* file = _htf_get_locations_file(a, "r");

  a->locations = malloc(sizeof(struct htf_location) * a->nb_locations);
  _htf_fread(a->locations, sizeof(struct htf_location), a->nb_locations, file);
  fclose(file);

  htf_log(htf_dbg_lvl_debug, "\tLoad %d locations\n", a->nb_locations);
}

static FILE* _htf_get_thread(const char* dir_name, htf_thread_id_t thread_id, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u.dat", dir_name, thread_id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_thread(const char* dir_name, struct htf_thread* th) {
  if (th->nb_events == 0) {
    htf_log(htf_dbg_lvl_verbose, "\tSkipping Thread %u {.nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n", th->id,
            th->nb_events, th->nb_sequences, th->nb_loops);
    abort();
  }

  FILE* token_file = _htf_get_thread(dir_name, th->id, "w");

  htf_log(htf_dbg_lvl_verbose, "\tThread %u {.nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n", th->id, th->nb_events,
          th->nb_sequences, th->nb_loops);

  _htf_fwrite(&th->id, sizeof(th->id), 1, token_file);
  _htf_fwrite(&th->archive->id, sizeof(th->archive->id), 1, token_file);

  _htf_fwrite(&th->nb_events, sizeof(th->nb_events), 1, token_file);
  _htf_fwrite(&th->nb_sequences, sizeof(th->nb_sequences), 1, token_file);
  _htf_fwrite(&th->nb_loops, sizeof(th->nb_loops), 1, token_file);

  fclose(token_file);
  htf_finish_timestamp();

  for (int i = 0; i < th->nb_events; i++)
    _htf_store_event(dir_name, th, &th->events[i], HTF_EVENT_ID(i));

  for (int i = 0; i < th->nb_sequences; i++)
    _htf_store_sequence(dir_name, th, th->sequences[i], HTF_SEQUENCE_ID(i));

  for (int i = 0; i < th->nb_loops; i++)
    _htf_store_loop(dir_name, th, &th->loops[i], HTF_LOOP_ID(i));
}

static void _htf_read_thread(struct htf_archive* global_archive, struct htf_thread* th, htf_thread_id_t thread_id) {
  FILE* token_file = _htf_get_thread(global_archive->dir_name, thread_id, "r");
  _htf_fread(&th->id, sizeof(th->id), 1, token_file);
  htf_location_group_id_t archive_id;
  _htf_fread(&archive_id, sizeof(archive_id), 1, token_file);
  th->archive = _htf_get_archive(global_archive, archive_id);

  _htf_fread(&th->nb_events, sizeof(th->nb_events), 1, token_file);
  th->nb_allocated_events = th->nb_events;
  th->events = malloc(sizeof(struct htf_event_summary) * th->nb_allocated_events);

  _htf_fread(&th->nb_sequences, sizeof(th->nb_sequences), 1, token_file);
  th->nb_allocated_sequences = th->nb_sequences;
  th->sequences = malloc(sizeof(struct htf_sequence*) * th->nb_allocated_sequences);
  for (int i = 0; i < th->nb_sequences; i++) {
    th->sequences[i] = malloc(sizeof(struct htf_sequence));
  }

  _htf_fread(&th->nb_loops, sizeof(th->nb_loops), 1, token_file);
  th->nb_allocated_loops = th->nb_loops;
  th->loops = malloc(sizeof(struct htf_loop) * th->nb_allocated_loops);

  htf_log(htf_dbg_lvl_verbose, "Reading %d events\n", th->nb_events);
  for (int i = 0; i < th->nb_events; i++)
    _htf_read_event(global_archive->dir_name, th, &th->events[i], HTF_EVENT_ID(i));

  htf_log(htf_dbg_lvl_verbose, "Reading %d sequences\n", th->nb_sequences);
  for (int i = 0; i < th->nb_sequences; i++)
    _htf_read_sequence(global_archive->dir_name, th, th->sequences[i], HTF_SEQUENCE_ID(i));

  htf_log(htf_dbg_lvl_verbose, "Reading %d loops\n", th->nb_loops);
  for (int i = 0; i < th->nb_loops; i++)
    _htf_read_loop(global_archive->dir_name, th, &th->loops[i], HTF_LOOP_ID(i));

  fclose(token_file);

  htf_log(htf_dbg_lvl_verbose, "\tThread %u: {.nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n", th->id, th->nb_events,
          th->nb_sequences, th->nb_loops);
}

void htf_storage_finalize_thread(struct htf_thread* thread) {
  if (!thread)
    return;
  _htf_store_thread(thread->archive->dir_name, thread);
}

void htf_storage_finalize(struct htf_archive* archive) {
  if (!archive)
    return;

  int fullpath_len = strlen(archive->dir_name) + strlen(archive->trace_name) + 20;
  char* fullpath = malloc(fullpath_len * sizeof(char));
  if (archive->id == HTF_MAIN_LOCATION_GROUP_ID)
    snprintf(fullpath, fullpath_len, "%s/%s.htf", archive->dir_name, archive->trace_name);
  else
    snprintf(fullpath, fullpath_len, "%s/%s_%u.htf", archive->dir_name, archive->trace_name, archive->id);

  FILE* f = _htf_file_open(fullpath, "w");
  free(fullpath);
  _htf_fwrite(&archive->id, sizeof(htf_location_group_id_t), 1, f);
  _htf_fwrite(&archive->definitions.nb_strings, sizeof(int), 1, f);
  _htf_fwrite(&archive->definitions.nb_regions, sizeof(int), 1, f);
  _htf_fwrite(&archive->definitions.nb_attributes, sizeof(int), 1, f);
  _htf_fwrite(&archive->nb_location_groups, sizeof(int), 1, f);
  _htf_fwrite(&archive->nb_locations, sizeof(int), 1, f);
  _htf_fwrite(&archive->nb_threads, sizeof(int), 1, f);
  _htf_fwrite(&COMPRESSION_OPTIONS, sizeof(COMPRESSION_OPTIONS), 1, f);
  _htf_fwrite(&STORE_HASHING, sizeof(STORE_HASHING), 1, f);
  _htf_fwrite(&STORE_TIMESTAMPS, sizeof(STORE_TIMESTAMPS), 1, f);

  for (int i = 0; i < archive->definitions.nb_strings; i++) {
    _htf_store_string(archive, &archive->definitions.strings[i], i);
  }

  _htf_store_regions(archive);
  _htf_store_attributes(archive);

  _htf_store_location_groups(archive);
  _htf_store_locations(archive);

  fclose(f);
}

static char* _archive_filename(struct htf_archive* global_archive, htf_location_group_id_t id) {
  if (id == HTF_MAIN_LOCATION_GROUP_ID)
    return strdup(global_archive->trace_name);

  int tracename_len = strlen(global_archive->trace_name) + 1;
  int extension_index = tracename_len - 5;
  htf_assert(strcmp(&global_archive->trace_name[extension_index], ".htf") == 0);

  char trace_basename[tracename_len];
  strncpy(trace_basename, global_archive->trace_name, extension_index);
  trace_basename[extension_index] = '\0';

  int len = strlen(trace_basename) + 20;
  char* result = malloc(len * sizeof(char));
  snprintf(result, len, "%s_%d.htf", trace_basename, id);
  return result;
}

char* htf_archive_fullpath(char* dir_name, char* trace_name) {
  int len = strlen(dir_name) + strlen(trace_name) + 2;
  char* fullpath = malloc(len * sizeof(char));
  snprintf(fullpath, len, "%s/%s", dir_name, trace_name);
  return fullpath;
}

static void _htf_read_archive(struct htf_archive* global_archive,
                              struct htf_archive* archive,
                              char* dir_name,
                              char* trace_name) {
  archive->fullpath = htf_archive_fullpath(dir_name, trace_name);
  archive->dir_name = strdup(dir_name);
  archive->trace_name = strdup(trace_name);
  archive->global_archive = global_archive;
  archive->nb_archives = 0;
  archive->nb_allocated_archives = 1;
  archive->archive_list = malloc(sizeof(struct htf_archive*));
  if (archive->archive_list == NULL) {
    htf_error("Failed to allocate memory\n");
  }

  htf_log(htf_dbg_lvl_debug, "Reading archive {.dir_name='%s', .trace='%s'}\n", archive->dir_name, archive->trace_name);

  FILE* f = _htf_file_open(archive->fullpath, "r");

  _htf_fread(&archive->id, sizeof(htf_location_group_id_t), 1, f);
  _htf_fread(&archive->definitions.nb_strings, sizeof(int), 1, f);
  _htf_fread(&archive->definitions.nb_regions, sizeof(int), 1, f);
  _htf_fread(&archive->definitions.nb_attributes, sizeof(int), 1, f);
  _htf_fread(&archive->nb_location_groups, sizeof(int), 1, f);
  _htf_fread(&archive->nb_locations, sizeof(int), 1, f);
  _htf_fread(&archive->nb_threads, sizeof(int), 1, f);

  archive->threads = calloc(archive->nb_threads, sizeof(struct htf_thread*));
  archive->nb_allocated_threads = archive->nb_threads;

  _htf_fread(&COMPRESSION_OPTIONS, sizeof(COMPRESSION_OPTIONS), 1, f);
  _htf_fread(&STORE_HASHING, sizeof(STORE_HASHING), 1, f);
  _htf_fread(&STORE_TIMESTAMPS, sizeof(STORE_TIMESTAMPS), 1, f);

  char* store_timestamps_str = getenv("STORE_TIMESTAMPS");
  if (store_timestamps_str && strcmp(store_timestamps_str, "FALSE") == 0) {
    STORE_TIMESTAMPS = 0;
  }
  archive->store_timestamps = STORE_TIMESTAMPS;

  archive->definitions.nb_allocated_strings = archive->definitions.nb_strings;
  archive->definitions.strings = malloc(sizeof(struct htf_string) * archive->definitions.nb_allocated_strings);
  for (int i = 0; i < archive->definitions.nb_strings; i++) {
    htf_assert(strcmp(archive->dir_name, dir_name) == 0);

    _htf_read_string(archive, &archive->definitions.strings[i], i);
  }

  _htf_read_regions(archive);
  _htf_read_attributes(archive);

  archive->nb_allocated_location_groups = archive->nb_location_groups;
  if (archive->nb_allocated_location_groups) {
    archive->location_groups = malloc(sizeof(struct htf_location_group*) * archive->nb_allocated_location_groups);
    _htf_read_location_groups(archive);
  }

  archive->nb_allocated_locations = archive->nb_locations;
  if (archive->nb_allocated_locations) {
    archive->locations = malloc(sizeof(struct htf_location*) * archive->nb_allocated_locations);
    _htf_read_locations(archive);
  }

  if (archive->id == HTF_MAIN_LOCATION_GROUP_ID) {
    global_archive = archive;
  }

  for (int i = 0; i < archive->nb_locations; i++) {
    htf_assert(archive->locations[i].id != HTF_THREAD_ID_INVALID);
    htf_read_thread(global_archive, archive->locations[i].id);
  }
  fclose(f);
}

static struct htf_archive* _htf_get_archive(struct htf_archive* global_archive, htf_location_group_id_t archive_id) {
  /* check if archive_id is already known */
  for (int i = 0; i < global_archive->nb_archives; i++) {
    if (global_archive->archive_list[i]->id == archive_id) {
      return global_archive->archive_list[i];
    }
  }

  /* not found. we need to read the archive */
  struct htf_archive* arch = calloc(1, sizeof(struct htf_archive));
  char* filename = _archive_filename(global_archive, archive_id);
  char* fullpath = htf_archive_fullpath(global_archive->dir_name, filename);
  if (access(fullpath, R_OK) < 0) {
    printf("I can't read %s: %s\n", fullpath, strerror(errno));
    free(fullpath);
    return NULL;
  }
  printf("Reading archive %s\n", fullpath);
  free(fullpath);

  while (global_archive->nb_archives >= global_archive->nb_allocated_archives) {
    INCREMENT_MEMORY_SPACE(global_archive->archive_list, global_archive->nb_allocated_archives, struct htf_archive*);
  }

  _htf_read_archive(global_archive, arch, global_archive->dir_name, filename);

  int index = global_archive->nb_archives++;
  global_archive->archive_list[index] = arch;

  return arch;
}

void htf_read_thread(struct htf_archive* archive, htf_thread_id_t thread_id) {
  for (int i = 0; i < archive->nb_threads; i++) {
    if (archive->threads[i]->id == thread_id) {
      /* thread_id is already loaded */
      return;
    }
  }

  while (archive->nb_threads >= archive->nb_allocated_threads) {
    INCREMENT_MEMORY_SPACE(archive->threads, archive->nb_allocated_threads, struct htf_thread*);
  }

  int index = archive->nb_threads++;
  archive->threads[index] = malloc(sizeof(struct htf_thread));
  _htf_read_thread(archive, archive->threads[index], thread_id);
  htf_assert(archive->threads[index]->nb_events > 0);
}

void htf_read_archive(struct htf_archive* archive, char* main_filename) {
  char* dir_name = dirname(strdup(main_filename));
  char* trace_name = basename(strdup(main_filename));

  memset(archive, 0, sizeof(struct htf_archive));
  _htf_read_archive(NULL, archive, dir_name, trace_name);

  struct htf_archive* global_archive = archive->global_archive;

  if (archive->id == HTF_MAIN_LOCATION_GROUP_ID) {
    global_archive = archive;
  }

  for (int i = 0; i < archive->nb_locations; i++) {
    htf_read_thread(global_archive, archive->locations[i].id);
  }

  for (int i = 0; i < archive->nb_location_groups; i++) {
    _htf_get_archive(global_archive, archive->location_groups[i].id);
  }
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
