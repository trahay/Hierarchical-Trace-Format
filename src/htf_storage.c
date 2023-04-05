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

static void _htf_store_event(struct htf_thread* th,
			     struct htf_event_summary *e,
			     htf_event_id_t event_id);
static void _htf_store_sequence(struct htf_thread* th,
				struct htf_sequence *s,
				htf_sequence_id_t sequence_id);
static void _htf_store_loop(struct htf_thread* th,
			    struct htf_loop *l, htf_loop_id_t loop_id);
static void _htf_store_string(struct htf_archive *c,
			      struct htf_string *l,
			      int string_index);
static void _htf_store_regions(struct htf_archive *c);
static void _htf_store_containers(struct htf_archive *c);

static void _htf_read_event(struct htf_thread *th,
			    struct htf_event_summary *e,
			    htf_event_id_t event_id);
static void _htf_read_sequence(struct htf_thread *th,
			    struct htf_sequence *s,
			       htf_sequence_id_t sequence_id);
static void _htf_read_loop(struct htf_thread *th,
			    struct htf_loop *l,
			   htf_loop_id_t loop_id);
static void _htf_read_string(struct htf_archive *c,
			     struct htf_string *l,
			     int string_index);
static void _htf_read_regions(struct htf_archive* c);
static void _htf_read_containers(struct htf_archive *c);
static void _htf_read_archive(struct htf_archive* main_archive,
			      struct htf_archive* archive,
			      char* dir_name,
			      char* trace_name);

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

static FILE* _htf_get_event_file(struct htf_thread *th,
				 htf_event_id_t event_id,
				 char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u/event_%d", base_dirname_t(th), th->id, HTF_ID(event_id));
  return _htf_file_open(filename, mode);
}

static void _htf_store_event(struct htf_thread *th,
			     struct htf_event_summary *e,
			     htf_event_id_t event_id) {
  FILE* file = _htf_get_event_file(th, event_id, "w");

  htf_log(htf_dbg_lvl_debug, "\tStore event %x {.nb_timestamps=%d}\n", HTF_ID(event_id), e->nb_timestamps);

  _htf_fwrite(&e->event, sizeof(struct htf_event), 1, file);
  _htf_fwrite(&e->nb_timestamps, sizeof(e->nb_timestamps), 1, file);
  _htf_fwrite(e->timestamps, sizeof(e->timestamps[0]), e->nb_timestamps, file);
  
  fclose(file);
}

static void _htf_read_event(struct htf_thread *th,
			    struct htf_event_summary *e,
			    htf_event_id_t event_id) {
  FILE* file = _htf_get_event_file(th, event_id, "r");

  _htf_fread(&e->event, sizeof(struct htf_event), 1, file);
  _htf_fread(&e->nb_timestamps, sizeof(e->nb_timestamps), 1, file);
  e->timestamps = malloc(e->nb_timestamps * sizeof(htf_timestamp_t));
  _htf_fread(e->timestamps, sizeof(e->timestamps[0]), e->nb_timestamps, file);

  htf_log(htf_dbg_lvl_debug, "\tLoad event %x {.nb_timestamps=%d}\n", HTF_ID(event_id), e->nb_timestamps);

  fclose(file);
}


static FILE* _htf_get_sequence_file(struct htf_thread *th,
				    htf_sequence_id_t sequence_id,
				    char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u/sequence_%d", base_dirname_t(th), th->id, HTF_ID(sequence_id));
  return _htf_file_open(filename, mode);
}

static void _htf_store_sequence(struct htf_thread *th,
				struct htf_sequence *s,
				htf_sequence_id_t sequence_id) {
  FILE* file = _htf_get_sequence_file(th, sequence_id, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore sequence %x {.size=%d}\n", HTF_ID(sequence_id), s->size);
  
  _htf_fwrite(&s->size, sizeof(s->size), 1, file);
  _htf_fwrite(s->token, sizeof(s->token[0]), s->size, file);  
  fclose(file);
}

static void _htf_read_sequence(struct htf_thread *th,
			       struct htf_sequence *s,
			       htf_sequence_id_t sequence_id) {
  FILE* file = _htf_get_sequence_file(th, sequence_id, "r");
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



static FILE* _htf_get_loop_file(struct htf_thread *th, htf_loop_id_t loop_id, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u/loop_%d", base_dirname_t(th), th->id, HTF_ID(loop_id));
  return _htf_file_open(filename, mode);
}

static void _htf_store_loop(struct htf_thread* th,
			    struct htf_loop *l,
			    htf_loop_id_t loop_id) {
  FILE* file = _htf_get_loop_file(th, loop_id, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore loop %x {.nb_iterations=%d, .token=%x.%x}\n",
	  HTF_ID(loop_id), l->nb_iterations, l->token.type, l->token.id );

  _htf_fwrite(&l->nb_iterations, sizeof(l->nb_iterations), 1, file);
  _htf_fwrite(&l->token, sizeof(l->token), 1, file);
  fclose(file);
}

static void _htf_read_loop(struct htf_thread *th,
			   struct htf_loop *l,
			   htf_loop_id_t loop_id) {
  FILE* file = _htf_get_loop_file(th, loop_id, "r");
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

static void _htf_store_string(struct htf_archive *a,
			      struct htf_string *s,
			      int string_index) {
  FILE* file = _htf_get_string_file(a, string_index, "w");

  htf_log(htf_dbg_lvl_debug, "\tStore String %d {.ref=%d, .length=%d, .str='%s'}\n",
	  string_index, s->string_ref, s->length, s->str);

  _htf_fwrite(&s->string_ref, sizeof(s->string_ref), 1, file);
  _htf_fwrite(&s->length, sizeof(s->length), 1, file);
  _htf_fwrite(s->str, sizeof(char), s->length, file);

  fclose(file);
}

static void _htf_read_string(struct htf_archive *a,
			     struct htf_string *s,
			     int string_index) {
  FILE* file = _htf_get_string_file(a, string_index, "r");

  _htf_fread(&s->string_ref, sizeof(s->string_ref), 1, file);
  _htf_fread(&s->length, sizeof(s->length), 1, file);
  s->str = malloc(sizeof(char) * s->length);
  htf_assert(s->str);
  _htf_fread(s->str, sizeof(char), s->length, file);

  fclose(file);

  htf_log(htf_dbg_lvl_debug, "\tLoad String %d {.ref=%d, .length=%d, .str='%s'}\n",
	  string_index, s->string_ref, s->length, s->str);
}


static FILE* _htf_get_regions_file(struct htf_archive *a,
				   char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/regions.dat", base_dirname(a), a->id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_regions(struct htf_archive *a) {
  if(a->definitions.nb_regions == 0)
    return;

  FILE* file = _htf_get_regions_file(a, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore %d Regions\n", a->definitions.nb_regions);

  _htf_fwrite(a->definitions.regions, sizeof(struct htf_region), a->definitions.nb_regions, file);
  fclose(file);
}

static void _htf_read_regions(struct htf_archive *a) {
  if(a->definitions.nb_regions == 0)
    return;

  FILE* file = _htf_get_regions_file(a, "r");
  
  printf( "\tLoad %d regions\n", a->definitions.nb_regions);

  a->definitions.regions = malloc(sizeof(struct htf_region)* a->definitions.nb_regions);
  _htf_fread(a->definitions.regions, sizeof(struct htf_region), a->definitions.nb_regions, file);
  fclose(file);

  htf_log(htf_dbg_lvl_debug, "\tLoad %d regions\n", a->definitions.nb_regions);
}

static FILE* _htf_get_containers_file(struct htf_archive *a,
				      char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/archive_%u/containers.dat", base_dirname(a), a->id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_containers(struct htf_archive *a) {
  if(a->nb_containers == 0)
    return;

  FILE* file = _htf_get_containers_file(a, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore %d Containers\n", a->nb_containers);

  _htf_fwrite(a->containers, sizeof(struct htf_container), a->nb_containers, file);
  fclose(file);
}

static void _htf_read_containers(struct htf_archive *a) {
  if(a->nb_containers == 0)
    return;

  FILE* file = _htf_get_containers_file(a, "r");
  
  printf( "\tLoad %d containers\n", a->nb_containers);

  a->containers = malloc(sizeof(struct htf_container)* a->nb_containers);
  _htf_fread(a->containers, sizeof(struct htf_container), a->nb_containers, file);
  fclose(file);

  htf_log(htf_dbg_lvl_debug, "\tLoad %d containers\n", a->nb_containers);
}

static FILE* _htf_get_thread(struct htf_archive *archive,
			     htf_thread_id_t thread_id,
			     char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/thread_%u.dat", base_dirname(archive), thread_id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_thread(struct htf_thread *th) {
  FILE* token_file = _htf_get_thread(th->archive, th->id, "w");
 
  htf_log(htf_dbg_lvl_verbose, "\tThread %u {.nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n",
	  th->id, th->nb_events,  th->nb_sequences, th->nb_loops);

  _htf_fwrite(&th->id, sizeof(th->id), 1, token_file);
  _htf_fwrite(&th->container, sizeof(th->container), 1, token_file);

  _htf_fwrite(&th->nb_events, sizeof(th->nb_events), 1, token_file);
  _htf_fwrite(&th->nb_sequences, sizeof(th->nb_sequences), 1, token_file);
  _htf_fwrite(&th->nb_loops, sizeof(th->nb_loops), 1, token_file);

  fclose(token_file);

  for(int i=0; i<th->nb_events; i++)
    _htf_store_event(th, &th->events[i], HTF_EVENT_ID(i));
  
  for(int i=0; i<th->nb_sequences; i++)
    _htf_store_sequence(th, &th->sequences[i], HTF_SEQUENCE_ID(i));

  for(int i=0; i<th->nb_loops; i++)
    _htf_store_loop(th, &th->loops[i], HTF_LOOP_ID(i));
}

static void _htf_read_thread(struct htf_archive *archive,
			     struct htf_thread *th,
			     htf_thread_id_t thread_id) {
   
  FILE* token_file = _htf_get_thread(archive, thread_id, "r");

  th->archive = archive;
  _htf_fread(&th->id, sizeof(th->id), 1, token_file);
  _htf_fread(&th->container, sizeof(th->container), 1, token_file);

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
    _htf_read_event(th, &th->events[i], HTF_EVENT_ID(i));

  htf_log(htf_dbg_lvl_verbose, "Reading %d sequences\n", th->nb_sequences);
  for(int i=0; i<th->nb_sequences; i++)
    _htf_read_sequence(th, &th->sequences[i], HTF_SEQUENCE_ID(i));

  htf_log(htf_dbg_lvl_verbose, "Reading %d loops\n", th->nb_loops);
  for(int i=0; i<th->nb_loops; i++)
    _htf_read_loop(th, &th->loops[i], HTF_LOOP_ID(i));

  fclose(token_file);

  htf_log(htf_dbg_lvl_verbose, "\tThread %u: {.nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n",
	  th->id, th->nb_events,  th->nb_sequences, th->nb_loops);
}

void htf_storage_finalize_thread(struct htf_thread* thread) {
  if(!thread)
    return;
  _htf_store_thread(thread);

}

void htf_storage_finalize(struct htf_archive *archive) {
  if(! archive)
    return;

  int fullpath_len = strlen(archive->dir_name)+strlen(archive->trace_name)+20;
  char* fullpath = malloc(fullpath_len * sizeof(char));
  if(archive->id == 0)
    snprintf(fullpath, fullpath_len, "%s/%s.htf", archive->dir_name, archive->trace_name);
  else
    snprintf(fullpath, fullpath_len, "%s/%s_%u.htf", archive->dir_name, archive->trace_name, archive->id);
    

  FILE* f = _htf_file_open(fullpath, "w");
  free(fullpath);
  _htf_fwrite(&archive->id, sizeof(htf_archive_id_t), 1, f);
  _htf_fwrite(&archive->definitions.nb_strings, sizeof(int), 1, f);
  _htf_fwrite(&archive->definitions.nb_regions, sizeof(int), 1, f);
  _htf_fwrite(&archive->nb_containers, sizeof(int), 1, f);
  _htf_fwrite(&archive->nb_threads, sizeof(int), 1, f);
  _htf_fwrite(archive->thread_ids, sizeof(htf_thread_id_t), archive->nb_threads, f);

  _htf_fwrite(&archive->nb_archives, sizeof(int), 1, f);

  for(int i =0; i<archive->definitions.nb_strings; i++) {
    _htf_store_string(archive, &archive->definitions.strings[i], i);
  }

  _htf_store_regions(archive);

  _htf_store_containers(archive);

  for(int i =0; i<archive->nb_threads; i++) {
    _htf_store_thread(archive->threads[i]);
  }

  _htf_fwrite(archive->sub_archives, sizeof(htf_archive_id_t), archive->nb_archives, f);

  fclose(f);

  if(archive->next)
    htf_storage_finalize(archive->next);
}


static char* _sub_archive_filename(struct htf_archive* main_archive,
				   htf_archive_id_t    id) {
  
  int tracename_len = strlen(main_archive->trace_name)+1;
  int extension_index = tracename_len - 5;
  htf_assert(strcmp(&main_archive->trace_name[extension_index], ".htf")==0);

  char trace_basename[tracename_len];
  strncpy(trace_basename, main_archive->trace_name, extension_index);

  int len = strlen(trace_basename)+ 20;
  char* result = malloc(len * sizeof(char));
  snprintf(result, len, "%s_%d.htf", trace_basename, id);

  printf("sub_archive_filename(%s, %d) -> %s\n", main_archive->trace_name, id, result);
  return result;
}

/* add new_archive at the end of the archive list */
static void _push_archive(struct htf_archive* main_archive,
			  struct htf_archive* new_archive) {
  new_archive->next = NULL;
  if(main_archive->next == NULL) {
    main_archive->next = new_archive;
    return;
  }

  /* TODO: instead of adding new_archive to the end, we could sort the
   * list based on the archive_id, but is it useful ?
   */
  struct htf_archive *i = main_archive;
  while(i->next) { i = i->next; }
  i->next = new_archive;
}

char* htf_archive_fullpath(char* dir_name, char* trace_name) {
  int len = strlen(dir_name) + strlen(trace_name) + 2;
  char* fullpath = malloc(len * sizeof(char));
  snprintf(fullpath, len, "%s/%s",dir_name, trace_name);
  return fullpath;
}

static void _htf_read_archive(struct htf_archive* main_archive,
			      struct htf_archive* archive,
			      char* dir_name,
			      char* trace_name) {

  archive->fullpath = htf_archive_fullpath(dir_name, trace_name);
  archive->dir_name = strdup(dir_name);
  archive->trace_name = strdup(trace_name);
  archive->main_archive = main_archive;

  printf("archive {.dir_name='%s', .trace='%s'}\n", archive->dir_name, archive->trace_name);

  FILE* f = _htf_file_open(archive->fullpath, "r");

  _htf_fread(&archive->id, sizeof(htf_archive_id_t), 1, f);
  _htf_fread(&archive->definitions.nb_strings, sizeof(int), 1, f);
  _htf_fread(&archive->definitions.nb_regions, sizeof(int), 1, f);
  _htf_fread(&archive->nb_containers, sizeof(int), 1, f);
  _htf_fread(&archive->nb_threads, sizeof(int), 1, f);
  
  archive->thread_ids = malloc(sizeof(htf_thread_id_t) * archive->nb_threads);
  _htf_fread(archive->thread_ids, sizeof(htf_thread_id_t), archive->nb_threads, f);
  _htf_fread(&archive->nb_archives, sizeof(int), 1, f);

  archive->definitions.nb_allocated_strings = archive->definitions.nb_strings;
  archive->definitions.strings = malloc(sizeof(struct htf_string) * archive->definitions.nb_allocated_strings);
  for(int i =0; i<archive->definitions.nb_strings; i++) {
    htf_assert(strcmp( archive->dir_name, dir_name) == 0);

    _htf_read_string(archive, &archive->definitions.strings[i], i);
  }

  _htf_read_regions(archive);

  archive->nb_allocated_containers = archive->nb_containers;
  archive->containers = malloc(sizeof(struct htf_containers*)*archive->nb_allocated_containers);
  _htf_read_containers(archive);

  archive->nb_allocated_threads = archive->nb_threads;
  archive->threads = malloc(sizeof(struct htf_thread*)*archive->nb_allocated_threads);
  for(int i =0; i<archive->nb_threads; i++) {
    archive->threads[i] = malloc(sizeof(struct htf_thread));
    _htf_read_thread(archive, archive->threads[i], archive->thread_ids[i]);
  }

  archive->nb_allocated_archives = archive->nb_archives;
  archive->sub_archives = malloc(sizeof(htf_archive_id_t*)*archive->nb_allocated_archives);
  _htf_fread(archive->sub_archives, sizeof(htf_archive_id_t), archive->nb_allocated_archives, f);
  fclose(f);

  
  if(archive->nb_allocated_archives > 0) {
    struct htf_archive *sub_archives = malloc(sizeof(struct htf_archive) * archive->nb_allocated_archives);
    for(int i=0; i<archive->nb_allocated_archives; i++) {
      char* filename = _sub_archive_filename(main_archive, archive->sub_archives[i]);
      _htf_read_archive(main_archive, &sub_archives[i], main_archive->dir_name, filename);
      htf_assert(sub_archives[i].id == archive->sub_archives[i]);
      _push_archive(main_archive, &sub_archives[i]);
      free(filename);
    }
  }

#if 0
  fread(&archive->definitions.nb_strings, sizeof(int), 1, f);
  archive->definitions.nb_allocated_strings = archive->definitions.nb_strings;
  archive->definitions.strings = malloc(sizeof(struct htf_string) * archive->definitions.nb_allocated_strings);

  fread(&archive->definitions.nb_regions, sizeof(int), 1, f);
  archive->definitions.nb_allocated_regions = archive->definitions.nb_regions;
  archive->definitions.regions = malloc(sizeof(struct htf_region) * archive->definitions.nb_allocated_regions);
  
  fread(&archive->nb_containers, sizeof(int), 1, f);
  archive->nb_allocated_containers = archive->nb_containers;
  archive->containers = malloc(sizeof(struct htf_container) * archive->nb_allocated_containers);

  fread(&archive->nb_threads, sizeof(int), 1, f);
  archive->nb_allocated_threads = archive->nb_threads;
  archive->threads = malloc(sizeof(struct htf_thread) * archive->nb_allocated_threads);

  fread(&archive->nb_archives, sizeof(int), 1, f);

  for(int i =0; i<archive->definitions.nb_strings; i++) {
    _htf_read_string(archive, &archive->definitions.strings[i], i);
  }

  _htf_read_regions(archive);

  _htf_read_containers(archive);

  for(int i =0; i<archive->nb_threads; i++) {
    archive->threads[i] = malloc(sizeof(struct htf_thread));
    _htf_read_thread(archive->threads[i]);
  }
#endif
}

void htf_read_archive(struct htf_archive* archive, char* main_filename) {
  char* dir_name = dirname(strdup(main_filename));
  char* trace_name = basename(strdup(main_filename));

  _htf_read_archive(archive, archive, dir_name, trace_name);

}
