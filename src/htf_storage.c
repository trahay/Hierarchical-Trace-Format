#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>
#include <string.h>
#include <pthread.h>

#include "htf.h"
#include "htf_dbg.h"
#include "htf_timestamp.h"
#include "htf_event.h"

static void _htf_store_event(const char* base_dirname,
			     struct htf_thread* th,
			     struct htf_event_summary *e,
			     htf_event_id_t event_id);
static void _htf_store_sequence(const char* base_dirname,
				struct htf_thread* th,
				struct htf_sequence *s,
				htf_sequence_id_t sequence_id);
static void _htf_store_loop(const char* base_dirname,
			    struct htf_thread* th,
			    struct htf_loop *l, htf_loop_id_t loop_id);

static void _htf_store_string(struct htf_archive *c,
			      struct htf_string *l,
			      int string_index);
static void _htf_store_regions(struct htf_archive *c);

static void _htf_store_location_groups(struct htf_archive *a);
static void _htf_store_locations(struct htf_archive *a);

static void _htf_read_event(const char* base_dirname,
			    struct htf_thread *th,
			    struct htf_event_summary *e,
			    htf_event_id_t event_id);
static void _htf_read_sequence(const char* base_dirname,
			       struct htf_thread *th,
			       struct htf_sequence *s,
			       htf_sequence_id_t sequence_id);
static void _htf_read_loop(const char* base_dirname,
			   struct htf_thread *th,
			    struct htf_loop *l,
			   htf_loop_id_t loop_id);

static void _htf_read_string(struct htf_archive *c,
			     struct htf_string *l,
			     int string_index);
static void _htf_read_regions(struct htf_archive* c);
static void _htf_read_location_groups(struct htf_archive *a);
static void _htf_read_locations(struct htf_archive *a);

static void _htf_read_thread(struct htf_global_archive* global_archive,
			     struct htf_thread *th,
			     htf_thread_id_t thread_id);
static void _htf_read_archive(struct htf_global_archive* global_archive,
			      struct htf_archive* archive,
			      char* dir_name,
			      char* trace_name);

static struct htf_archive * _htf_get_archive(struct htf_global_archive* global_archive,
					     htf_archive_id_t archive_id);

static void _htf_mkdir(char* dirname, mode_t mode) {
  if(mkdir(dirname, mode) != 0) {
    if(errno != EEXIST)
      htf_error("mkdir(%s) failed: %s\n", dirname, strerror(errno));
  }
}

static FILE* _htf_file_open(char* filename, char* mode) {
  htf_log(htf_dbg_lvl_debug, "Open %s with mode %s\n", filename, mode);
  char* filename_copy = strdup(filename);
  _htf_mkdir(dirname(filename_copy), 0777);
  free(filename_copy);

  FILE* file = fopen(filename, mode);
  if(file == NULL) {
    htf_error("Cannot open %s: %s\n", filename, strerror(errno));
  }
  return file;
}

#define _htf_fread(ptr, size, nmemb, stream) do {	\
    size_t  ret = fread(ptr, size, nmemb, stream);	\
    if(ret != nmemb) htf_error("fread failed\n");		\
  } while(0)


#define _htf_fwrite(ptr, size, nmemb, stream) do {	\
    size_t  ret = fwrite(ptr, size, nmemb, stream);	\
    if(ret != nmemb) htf_error("fwrite failed\n");		\
  } while(0)

void htf_storage_init(struct htf_archive* archive) {
  _htf_mkdir(archive->dir_name, 0777);
}

static const char* base_dirname(struct htf_archive *a) { return a->dir_name;}
static const char* base_dirname_t(struct htf_thread *th) { return base_dirname(th->archive); }

static FILE* _htf_get_event_file(const char* base_dirname,
				 struct htf_thread *th,
				 htf_event_id_t event_id,
				 char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u/event_%d", base_dirname, th->id, HTF_ID(event_id));
  return _htf_file_open(filename, mode);
}

static void _htf_store_event(const char* base_dirname,
			     struct htf_thread *th,
			     struct htf_event_summary *e,
			     htf_event_id_t event_id) {
  FILE* file = _htf_get_event_file(base_dirname, th, event_id, "w");

  htf_log(htf_dbg_lvl_debug, "\tStore event %x {.nb_timestamps=%d}\n", HTF_ID(event_id), e->nb_timestamps);

  _htf_fwrite(&e->event, sizeof(struct htf_event), 1, file);
  _htf_fwrite(&e->nb_timestamps, sizeof(e->nb_timestamps), 1, file);
  _htf_fwrite(e->timestamps, sizeof(e->timestamps[0]), e->nb_timestamps, file);
  
  fclose(file);
}

static void _htf_read_event(const char* base_dirname,
			    struct htf_thread *th,
			    struct htf_event_summary *e,
			    htf_event_id_t event_id) {
  FILE* file = _htf_get_event_file(base_dirname, th, event_id, "r");

  _htf_fread(&e->event, sizeof(struct htf_event), 1, file);
  _htf_fread(&e->nb_timestamps, sizeof(e->nb_timestamps), 1, file);
  e->timestamps = malloc(e->nb_timestamps * sizeof(htf_timestamp_t));
  _htf_fread(e->timestamps, sizeof(e->timestamps[0]), e->nb_timestamps, file);

  htf_log(htf_dbg_lvl_debug, "\tLoad event %x {.nb_timestamps=%d}\n", HTF_ID(event_id), e->nb_timestamps);

  fclose(file);
}


static FILE* _htf_get_sequence_file(const char* base_dirname,
				    struct htf_thread *th,
				    htf_sequence_id_t sequence_id,
				    char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u/sequence_%d", base_dirname, th->id, HTF_ID(sequence_id));
  return _htf_file_open(filename, mode);
}

static void _htf_store_sequence(const char* base_dirname,
				struct htf_thread *th,
				struct htf_sequence *s,
				htf_sequence_id_t sequence_id) {
  FILE* file = _htf_get_sequence_file(base_dirname, th, sequence_id, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore sequence %x {.size=%d}\n", HTF_ID(sequence_id), s->size);
  
  _htf_fwrite(&s->size, sizeof(s->size), 1, file);
  _htf_fwrite(s->token, sizeof(s->token[0]), s->size, file);  
  fclose(file);
}

static void _htf_read_sequence(const char* base_dirname,
			       struct htf_thread *th,
			       struct htf_sequence *s,
			       htf_sequence_id_t sequence_id) {
  FILE* file = _htf_get_sequence_file(base_dirname, th, sequence_id, "r");
  _htf_fread(&s->size, sizeof(s->size), 1, file);
  s->token = malloc(sizeof(htf_token_t) * s->size);
  s->allocated = s->size;
  _htf_fread(s->token, sizeof(htf_token_t), s->size, file);  
  fclose(file);

  htf_log(htf_dbg_lvl_debug, "\tLoad sequence %x {.size=%d}\n", HTF_ID(sequence_id), s->size);

  if(htf_debug_level >= htf_dbg_lvl_debug) {
    for(int i = 0; i<s->size && i< 15; i++) {
      printf("{%x.%x} ", HTF_TOKEN_TYPE(s->token[i]), HTF_TOKEN_ID(s->token[i]));
    }
    printf("\n");
  }
}



static FILE* _htf_get_loop_file(const char* base_dirname,
				struct htf_thread *th,
				htf_loop_id_t loop_id,
				char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u/loop_%d", base_dirname, th->id, HTF_ID(loop_id));
  return _htf_file_open(filename, mode);
}

static void _htf_store_loop(const char* base_dirname,
			    struct htf_thread* th,
			    struct htf_loop *l,
			    htf_loop_id_t loop_id) {
  FILE* file = _htf_get_loop_file(base_dirname, th, loop_id, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore loop %x {.nb_iterations=%d, .token=%x.%x}\n",
	  HTF_ID(loop_id), l->nb_iterations, l->token.type, l->token.id );

  _htf_fwrite(&l->nb_iterations, sizeof(l->nb_iterations), 1, file);
  _htf_fwrite(&l->token, sizeof(l->token), 1, file);
  fclose(file);
}

static void _htf_read_loop(const char* base_dirname,
			   struct htf_thread *th,
			   struct htf_loop *l,
			   htf_loop_id_t loop_id) {
  FILE* file = _htf_get_loop_file(base_dirname, th, loop_id, "r");
  _htf_fread(&l->nb_iterations, sizeof(l->nb_iterations), 1, file);
  _htf_fread(&l->token, sizeof(l->token), 1, file);
  fclose(file);
  htf_log(htf_dbg_lvl_debug, "\tLoad loop %x {.nb_iterations=%d, .token=%x.%x}\n",
	  HTF_ID(loop_id), l->nb_iterations, l->token.type, l->token.id );
}


static FILE* _htf_get_string_file(struct htf_archive *a,
				  int string_index, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/string_%d", base_dirname(a), a->id, string_index);
  return _htf_file_open(filename, mode);
}

static FILE* _htf_get_global_string_file(struct htf_global_archive *a,
					 int string_index, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive/string_%d", a->dir_name, string_index);
  return _htf_file_open(filename, mode);
}

static void _htf_store_string_generic(FILE* file,
				      struct htf_string *s,
				      int string_index) {
  htf_log(htf_dbg_lvl_debug, "\tStore String %d {.ref=%d, .length=%d, .str='%s'}\n",
	  string_index, s->string_ref, s->length, s->str);

  _htf_fwrite(&s->string_ref, sizeof(s->string_ref), 1, file);
  _htf_fwrite(&s->length, sizeof(s->length), 1, file);
  _htf_fwrite(s->str, sizeof(char), s->length, file);
}


static void _htf_store_string(struct htf_archive *a,
			      struct htf_string *s,
			      int string_index) {
  FILE* file = _htf_get_string_file(a, string_index, "w");
  _htf_store_string_generic(file, s, string_index);
  fclose(file);
}

static void _htf_store_global_string(struct htf_global_archive *a,
				     struct htf_string *s,
				     int string_index) {
  FILE* file = _htf_get_global_string_file(a, string_index, "w");
  _htf_store_string_generic(file, s, string_index);
  fclose(file);
}

static void _htf_read_string_generic(FILE* file,
				     struct htf_string *s,
				     int string_index) {
  _htf_fread(&s->string_ref, sizeof(s->string_ref), 1, file);
  _htf_fread(&s->length, sizeof(s->length), 1, file);
  s->str = malloc(sizeof(char) * s->length);
  htf_assert(s->str);
  _htf_fread(s->str, sizeof(char), s->length, file);
  htf_log(htf_dbg_lvl_debug, "\tLoad String %d {.ref=%d, .length=%d, .str='%s'}\n",
	  string_index, s->string_ref, s->length, s->str);
}

static void _htf_read_string(struct htf_archive *a,
			     struct htf_string *s,
			     int string_index) {
  FILE* file = _htf_get_string_file(a, string_index, "r");
  _htf_read_string_generic(file, s, string_index);
  fclose(file);
}

static void _htf_read_global_string(struct htf_global_archive *a,
				    struct htf_string *s,
				    int string_index) {
  FILE* file = _htf_get_global_string_file(a, string_index, "r");
  _htf_read_string_generic(file, s, string_index);
  fclose(file);
}

static FILE* _htf_get_regions_file(struct htf_archive *a,
				   char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/regions.dat", base_dirname(a), a->id);
  return _htf_file_open(filename, mode);
}


static FILE* _htf_get_global_regions_file(struct htf_global_archive *a,
					  char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive/regions.dat", a->dir_name);
  return _htf_file_open(filename, mode);
}

static void _htf_store_regions_generic(FILE* file,
				       struct htf_definition *d) {
  if(d->nb_regions == 0)
    return;

  htf_log(htf_dbg_lvl_debug, "\tStore %d Regions\n", d->nb_regions);

  _htf_fwrite(d->regions, sizeof(struct htf_region), d->nb_regions, file);
}

static void _htf_store_regions(struct htf_archive *a) {
  if(a->definitions.nb_regions == 0)
    return;

  FILE* file = _htf_get_regions_file(a, "w");
  _htf_store_regions_generic(file, &a->definitions);
  fclose(file);
}

static void _htf_store_global_regions(struct htf_global_archive *a) {
  if(a->definitions.nb_regions == 0)
    return;

  FILE* file = _htf_get_global_regions_file(a, "w");
  _htf_store_regions_generic(file, &a->definitions);
  fclose(file);
}

static void _htf_read_regions_generic(FILE* file, struct htf_definition *d) {
  if(d->nb_regions == 0)
    return;

  printf( "\tLoad %d regions\n", d->nb_regions);

  d->nb_allocated_regions = d->nb_regions;
  d->regions = malloc(sizeof(struct htf_region)* d->nb_allocated_regions);
  _htf_fread(d->regions, sizeof(struct htf_region), d->nb_allocated_regions, file);

  htf_log(htf_dbg_lvl_debug, "\tLoad %d regions\n", d->nb_regions);
}

static void _htf_read_regions(struct htf_archive *a) {
  if(a->definitions.nb_regions == 0)
    return;

  FILE* file = _htf_get_regions_file(a, "r");
  _htf_read_regions_generic(file, &a->definitions);
  fclose(file);
}

static void _htf_read_global_regions(struct htf_global_archive *a) {
  if(a->definitions.nb_regions == 0)
    return;

  FILE* file = _htf_get_global_regions_file(a, "r");
  _htf_read_regions_generic(file, &a->definitions);
  fclose(file);
}

static FILE* _htf_get_location_groups_file(struct htf_archive *a,
				      char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/location_groups.dat", base_dirname(a), a->id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_location_groups(struct htf_archive *a) { 
  if(a->nb_location_groups == 0)
    return;

  FILE* file = _htf_get_location_groups_file(a, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore %d location groupds\n", a->nb_location_groups);

  _htf_fwrite(a->location_groups, sizeof(struct htf_location_group), a->nb_location_groups, file);
  fclose(file);
}

static void _htf_read_location_groups(struct htf_archive *a) {
  if(a->nb_location_groups == 0)
    return;

  FILE* file = _htf_get_location_groups_file(a, "r");
  
  printf( "\tLoad %d location_groups\n", a->nb_location_groups);

  a->location_groups = malloc(sizeof(struct htf_location_group)* a->nb_location_groups);
  _htf_fread(a->location_groups, sizeof(struct htf_location_group), a->nb_location_groups, file);
  fclose(file);

  htf_log(htf_dbg_lvl_debug, "\tLoad %d location_groups\n", a->nb_location_groups);
}


static FILE* _htf_get_global_location_groups_file(struct htf_global_archive *a,
					     char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive/location_groups.dat", a->dir_name);
  return _htf_file_open(filename, mode);
}

static void _htf_store_global_location_groups(struct htf_global_archive *a) { 
  if(a->nb_location_groups == 0)
    return;

  FILE* file = _htf_get_global_location_groups_file(a, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore %d Location_Groups\n", a->nb_location_groups);

  _htf_fwrite(a->location_groups, sizeof(struct htf_location_group), a->nb_location_groups, file);
  fclose(file);
}

static void _htf_read_global_location_groups(struct htf_global_archive *a) {
  if(a->nb_location_groups == 0)
    return;

  FILE* file = _htf_get_global_location_groups_file(a, "r");
  
  printf( "\tLoad %d location_groups\n", a->nb_location_groups);

  a->location_groups = malloc(sizeof(struct htf_location_group)* a->nb_location_groups);
  _htf_fread(a->location_groups, sizeof(struct htf_location_group), a->nb_location_groups, file);
  fclose(file);
//
//  a->nb_threads = 0;
//  for(int i=0; i<a->nb_location_groups; i++) {
//    if(a->location_groups[i].thread_id != HTF_THREAD_ID_INVALID)
//      a->nb_threads++;
//  }
//
//  a->threads = malloc(sizeof(struct htf_thread) * a->nb_threads);
//  int thread_rank = 0;
//  for(int i=0; i < a->nb_containers; i++) {
//    if(a->containers[i].thread_id != HTF_THREAD_ID_INVALID) {
//      _htf_read_thread(a, &a->threads[thread_rank++], a->containers[i].thread_id);
//    }
//  }
  htf_log(htf_dbg_lvl_debug, "\tLoad %d location groups\n", a->nb_location_groups);
}

static FILE* _htf_get_locations_file(struct htf_archive *a,
					   char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/locations.dat", base_dirname(a), a->id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_locations(struct htf_archive *a) { 
  if(a->nb_locations == 0)
    return;

  FILE* file = _htf_get_locations_file(a, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore %d locations\n", a->nb_locations);

  _htf_fwrite(a->locations, sizeof(struct htf_location), a->nb_locations, file);
  fclose(file);
}

static void _htf_read_locations(struct htf_archive *a) {
  if(a->nb_locations == 0)
    return;

  FILE* file = _htf_get_locations_file(a, "r");
  
  printf( "\tLoad %d locations\n", a->nb_locations);

  a->locations = malloc(sizeof(struct htf_location)* a->nb_locations);
  _htf_fread(a->locations, sizeof(struct htf_location), a->nb_locations, file);
  fclose(file);

  htf_log(htf_dbg_lvl_debug, "\tLoad %d locations\n", a->nb_locations);
}


static FILE* _htf_get_global_locations_file(struct htf_global_archive *a,
					     char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive/locations.dat", a->dir_name);
  return _htf_file_open(filename, mode);
}

static void _htf_store_global_locations(struct htf_global_archive *a) { 
  if(a->nb_locations == 0)
    return;

  FILE* file = _htf_get_global_locations_file(a, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore %d Locations\n", a->nb_locations);

  _htf_fwrite(a->locations, sizeof(struct htf_location), a->nb_locations, file);
  fclose(file);
}

static void _htf_read_global_locations(struct htf_global_archive *a) {
  if(a->nb_locations == 0)
    return;

  FILE* file = _htf_get_global_locations_file(a, "r");
  
  printf( "\tLoad %d locations\n", a->nb_locations);

  a->locations = malloc(sizeof(struct htf_location)* a->nb_locations);
  _htf_fread(a->locations, sizeof(struct htf_location), a->nb_locations, file);
  fclose(file);
//
//  a->nb_threads = 0;
//  for(int i=0; i<a->nb_location_groups; i++) {
//    if(a->location_groups[i].thread_id != HTF_THREAD_ID_INVALID)
//      a->nb_threads++;
//  }
//
//  a->threads = malloc(sizeof(struct htf_thread) * a->nb_threads);
//  int thread_rank = 0;
//  for(int i=0; i < a->nb_containers; i++) {
//    if(a->containers[i].thread_id != HTF_THREAD_ID_INVALID) {
//      _htf_read_thread(a, &a->threads[thread_rank++], a->containers[i].thread_id);
//    }
//  }
  htf_log(htf_dbg_lvl_debug, "\tLoad %d location groups\n", a->nb_locations);
}

static FILE* _htf_get_thread(const char* dir_name, 
			     htf_thread_id_t thread_id,
			     char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u.dat", dir_name, thread_id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_thread(const char* dir_name, struct htf_thread *th) {
  FILE* token_file = _htf_get_thread(dir_name, th->id, "w");
 
  htf_log(htf_dbg_lvl_verbose, "\tThread %u {.nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n",
	  th->id, th->nb_events,  th->nb_sequences, th->nb_loops);

  _htf_fwrite(&th->id, sizeof(th->id), 1, token_file);
  _htf_fwrite(&th->archive->id, sizeof(th->archive->id), 1, token_file);

  _htf_fwrite(&th->nb_events, sizeof(th->nb_events), 1, token_file);
  _htf_fwrite(&th->nb_sequences, sizeof(th->nb_sequences), 1, token_file);
  _htf_fwrite(&th->nb_loops, sizeof(th->nb_loops), 1, token_file);

  fclose(token_file);

  for(int i=0; i<th->nb_events; i++)
    _htf_store_event(dir_name, th, &th->events[i], HTF_EVENT_ID(i));
  
  for(int i=0; i<th->nb_sequences; i++)
    _htf_store_sequence(dir_name, th, &th->sequences[i], HTF_SEQUENCE_ID(i));

  for(int i=0; i<th->nb_loops; i++)
    _htf_store_loop(dir_name, th, &th->loops[i], HTF_LOOP_ID(i));
}

static void _htf_read_thread(struct htf_global_archive* global_archive,
			     struct htf_thread *th,
			     htf_thread_id_t thread_id) {
   
  FILE* token_file = _htf_get_thread(global_archive->dir_name, thread_id, "r");
  _htf_fread(&th->id, sizeof(th->id), 1, token_file);
  htf_archive_id_t archive_id;
  _htf_fread(&archive_id, sizeof(archive_id), 1, token_file);
  //  th->archive = _htf_get_archive(global_archive, archive_id);

  _htf_fread(&th->nb_events, sizeof(th->nb_events), 1, token_file);
  th->nb_allocated_events = th->nb_events;
  th->events = malloc(sizeof(struct htf_event_summary) * th->nb_allocated_events);

  _htf_fread(&th->nb_sequences, sizeof(th->nb_sequences), 1, token_file);
  th->nb_allocated_sequences = th->nb_sequences;
  th->sequences = malloc(sizeof(struct htf_sequence) * th->nb_allocated_sequences);

  _htf_fread(&th->nb_loops, sizeof(th->nb_loops), 1, token_file);
  th->nb_allocated_loops = th->nb_loops;
  th->loops = malloc(sizeof(struct htf_loop) * th->nb_allocated_loops);

  htf_log(htf_dbg_lvl_verbose, "Reading %d events\n", th->nb_events);
  for(int i=0; i<th->nb_events; i++)
    _htf_read_event(global_archive->dir_name, th, &th->events[i], HTF_EVENT_ID(i));

  htf_log(htf_dbg_lvl_verbose, "Reading %d sequences\n", th->nb_sequences);
  for(int i=0; i<th->nb_sequences; i++)
    _htf_read_sequence(global_archive->dir_name, th, &th->sequences[i], HTF_SEQUENCE_ID(i));

  htf_log(htf_dbg_lvl_verbose, "Reading %d loops\n", th->nb_loops);
  for(int i=0; i<th->nb_loops; i++)
    _htf_read_loop(global_archive->dir_name, th, &th->loops[i], HTF_LOOP_ID(i));

  fclose(token_file);

  htf_log(htf_dbg_lvl_verbose, "\tThread %u: {.nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n",
	  th->id, th->nb_events,  th->nb_sequences, th->nb_loops);
}

void htf_storage_finalize_thread(struct htf_thread* thread) {
  if(!thread)
    return;
  _htf_store_thread(thread->archive->dir_name, thread);

}

void htf_storage_finalize(struct htf_archive *archive) {
  if(! archive)
    return;

  int fullpath_len = strlen(archive->dir_name)+strlen(archive->trace_name)+20;
  char* fullpath = malloc(fullpath_len * sizeof(char));
  snprintf(fullpath, fullpath_len, "%s/%s_%u.htf", archive->dir_name, archive->trace_name, archive->id); 

  FILE* f = _htf_file_open(fullpath, "w");
  free(fullpath);
  _htf_fwrite(&archive->id, sizeof(htf_archive_id_t), 1, f);
  _htf_fwrite(&archive->definitions.nb_strings, sizeof(int), 1, f);
  _htf_fwrite(&archive->definitions.nb_regions, sizeof(int), 1, f);
  _htf_fwrite(&archive->nb_location_groups, sizeof(int), 1, f);
  _htf_fwrite(&archive->nb_locations, sizeof(int), 1, f);
  _htf_fwrite(&archive->nb_threads, sizeof(int), 1, f);

  for(int i =0; i<archive->definitions.nb_strings; i++) {
    _htf_store_string(archive, &archive->definitions.strings[i], i);
  }

  _htf_store_regions(archive);

  _htf_store_location_groups(archive);
  _htf_store_locations(archive);

  for(int i =0; i<archive->nb_threads; i++) {
    _htf_store_thread(archive->dir_name, archive->threads[i]);
  }

  fclose(f);
}


void htf_storage_finalize_global(struct htf_global_archive *archive) {
  if(! archive)
    return;

  int fullpath_len = strlen(archive->dir_name)+strlen(archive->trace_name)+20;
  char* fullpath = malloc(fullpath_len * sizeof(char));
  snprintf(fullpath, fullpath_len, "%s/%s.htf", archive->dir_name, archive->trace_name);

  FILE* f = _htf_file_open(fullpath, "w");
  free(fullpath);
  _htf_fwrite(&archive->definitions.nb_strings, sizeof(int), 1, f);
  _htf_fwrite(&archive->definitions.nb_regions, sizeof(int), 1, f);
  _htf_fwrite(&archive->nb_location_groups, sizeof(int), 1, f);
  _htf_fwrite(&archive->nb_locations, sizeof(int), 1, f);
  _htf_fwrite(&archive->nb_archives, sizeof(int), 1, f);
  struct htf_archive *arch = archive->archive_list;
  for(int i = 0; i<archive->nb_archives; i++, arch=arch->next) {
    _htf_fwrite(&arch->id, sizeof(arch->id), 1, f);
  }

  for(int i =0; i<archive->definitions.nb_strings; i++) {
    _htf_store_global_string(archive, &archive->definitions.strings[i], i);
  }

  _htf_store_global_regions(archive);

  _htf_store_global_location_groups(archive);
  _htf_store_global_locations(archive);

  fclose(f);
}


static char* _archive_filename(struct htf_global_archive* global_archive,
			       htf_archive_id_t    id) {
  
  int tracename_len = strlen(global_archive->trace_name)+1;
  int extension_index = tracename_len - 5;
  htf_assert(strcmp(&global_archive->trace_name[extension_index], ".htf")==0);

  char trace_basename[tracename_len];
  strncpy(trace_basename, global_archive->trace_name, extension_index);
  trace_basename[extension_index] = '\0';

  int len = strlen(trace_basename)+ 20;
  char* result = malloc(len * sizeof(char));
  snprintf(result, len, "%s_%d.htf", trace_basename, id);
  return result;
}

char* htf_archive_fullpath(char* dir_name, char* trace_name) {
  int len = strlen(dir_name) + strlen(trace_name) + 2;
  char* fullpath = malloc(len * sizeof(char));
  snprintf(fullpath, len, "%s/%s",dir_name, trace_name);
  return fullpath;
}

static struct htf_archive * _htf_get_archive(struct htf_global_archive* global_archive,
					     htf_archive_id_t archive_id) {
  /* check if archive_id is already known */
  for(struct htf_archive*arch = global_archive->archive_list;
      arch;
      arch = arch->next) {
    if(arch->id == archive_id) {
      return arch;
    }
  }

  /* not found. we need to read the archive */
  struct htf_archive* arch = malloc(sizeof(struct htf_archive));
  char* filename = _archive_filename(global_archive, archive_id);
  _htf_read_archive(global_archive, arch, global_archive->dir_name, filename);
  arch->next = global_archive->archive_list;
  global_archive->archive_list = arch;

  return arch;
}

static void _htf_read_archive(struct htf_global_archive* global_archive,
			      struct htf_archive* archive,
			      char* dir_name,
			      char* trace_name) {

  archive->fullpath = htf_archive_fullpath(dir_name, trace_name);
  archive->dir_name = strdup(dir_name);
  archive->trace_name = strdup(trace_name);
  archive->global_archive = global_archive;
  archive->next = NULL;

  htf_log(htf_dbg_lvl_debug, "Reading archive {.dir_name='%s', .trace='%s'}\n", archive->dir_name, archive->trace_name);

  FILE* f = _htf_file_open(archive->fullpath, "r");

  _htf_fread(&archive->id, sizeof(htf_archive_id_t), 1, f);
  _htf_fread(&archive->definitions.nb_strings, sizeof(int), 1, f);
  _htf_fread(&archive->definitions.nb_regions, sizeof(int), 1, f);
  _htf_fread(&archive->nb_location_groups, sizeof(int), 1, f);
  _htf_fread(&archive->nb_locations, sizeof(int), 1, f);
  _htf_fread(&archive->nb_threads, sizeof(int), 1, f);
  
  archive->definitions.nb_allocated_strings = archive->definitions.nb_strings;
  archive->definitions.strings = malloc(sizeof(struct htf_string) * archive->definitions.nb_allocated_strings);
  for(int i =0; i<archive->definitions.nb_strings; i++) {
    htf_assert(strcmp( archive->dir_name, dir_name) == 0);

    _htf_read_string(archive, &archive->definitions.strings[i], i);
  }

  _htf_read_regions(archive);

  archive->nb_allocated_location_groups = archive->nb_location_groups;
  if(archive->nb_allocated_location_groups) {
    archive->location_groups = malloc(sizeof(struct htf_location_group*)*archive->nb_allocated_location_groups);
    _htf_read_location_groups(archive);
  }

  archive->nb_allocated_locations = archive->nb_locations;
  if(archive->nb_allocated_locations) {
    archive->locations = malloc(sizeof(struct htf_location*)*archive->nb_allocated_locations);
    _htf_read_locations(archive);
  }

  archive->nb_allocated_threads = archive->nb_threads;
  if(archive->nb_allocated_threads) {
    archive->threads = malloc(sizeof(struct htf_thread*)*archive->nb_allocated_threads);

    htf_assert(archive->nb_threads == archive->nb_locations);// or should we get it from the global archive ?
    for(int i =0; i<archive->nb_threads; i++) {
      archive->threads[i] = malloc(sizeof(struct htf_thread));
      _htf_read_thread(global_archive, archive->threads[i], archive->locations[i].id);
    }
  }
  fclose(f);
}

static void _htf_read_global_archive(struct htf_global_archive* archive,
				     char* dir_name,
				     char* trace_name) {

  archive->fullpath = htf_archive_fullpath(dir_name, trace_name);
  archive->dir_name = strdup(dir_name);
  archive->trace_name = strdup(trace_name);

  FILE* f = _htf_file_open(archive->fullpath, "r");

  _htf_fread(&archive->definitions.nb_strings, sizeof(int), 1, f);
  _htf_fread(&archive->definitions.nb_regions, sizeof(int), 1, f);
  _htf_fread(&archive->nb_location_groups, sizeof(int), 1, f);
  _htf_fread(&archive->nb_locations, sizeof(int), 1, f);
  _htf_fread(&archive->nb_archives, sizeof(int), 1, f);
  for(int i = 0; i< archive->nb_archives; i++) {
    htf_archive_id_t arch_id;
    _htf_fread(&arch_id, sizeof(arch_id), 1, f);
    _htf_get_archive(archive, arch_id);  
  }

  fclose(f);
  
  archive->definitions.nb_allocated_strings = archive->definitions.nb_strings;
  archive->definitions.strings = malloc(sizeof(struct htf_string) * archive->definitions.nb_allocated_strings);
  for(int i =0; i<archive->definitions.nb_strings; i++) {
    _htf_read_global_string(archive, &archive->definitions.strings[i], i);
  }

  _htf_read_global_regions(archive);

  archive->nb_allocated_location_groups = archive->nb_location_groups;
  if(archive->nb_allocated_location_groups) {
    archive->location_groups = malloc(sizeof(struct htf_location_groups*)*archive->nb_allocated_location_groups);
    _htf_read_global_location_groups(archive);
  }

  archive->nb_allocated_locations = archive->nb_locations;
  if(archive->nb_allocated_locations) {
    archive->locations = malloc(sizeof(struct htf_locations*)*archive->nb_allocated_locations);
    _htf_read_global_locations(archive);

    archive->nb_threads = archive->nb_locations;
    archive->threads = malloc(sizeof(struct htf_thread)*archive->nb_allocated_locations);
    for(int i =0; i<archive->nb_threads; i++) {
      _htf_read_thread(archive, &archive->threads[i], archive->locations[i].id);
      /* TODO: probleme here: thread->archive is null */
    }
  }
}

void htf_read_archive(struct htf_archive* archive, char* main_filename) {
  char* dir_name = dirname(strdup(main_filename));
  char* trace_name = basename(strdup(main_filename));

  memset(archive, 0, sizeof(struct htf_archive));
  _htf_read_archive(NULL, archive, dir_name, trace_name);

}

void htf_read_global_archive(struct htf_global_archive* archive, char* main_filename) {
  char* dir_name = dirname(strdup(main_filename));
  char* trace_name = basename(strdup(main_filename));

  memset(archive, 0, sizeof(struct htf_global_archive));
  _htf_read_global_archive(archive, dir_name, trace_name);
}
