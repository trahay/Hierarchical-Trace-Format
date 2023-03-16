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

static char *base_dirname = NULL;

static void _htf_store_event(struct htf_event_summary *e, int thread_index, htf_event_id_t event_id);
static void _htf_store_sequence(struct htf_sequence *s, int thread_index, htf_sequence_id_t sequence_id);
static void _htf_store_loop(struct htf_loop *l, int thread_index, htf_loop_id_t loop_id);
static void _htf_store_string(struct htf_string *l, int thread_index, int string_index);
static void _htf_store_region(struct htf_region *r, int thread_index, int region_index);

static void _htf_store_thread_trace(struct htf_trace *trace, int thread_index);

static void _htf_read_event(struct htf_event_summary *e, int thread_index, htf_event_id_t event_id);
static void _htf_read_sequence(struct htf_sequence *s, int thread_index, htf_sequence_id_t sequence_id);
static void _htf_read_loop(struct htf_loop *l, int thread_index, htf_loop_id_t loop_id);
static void _htf_read_string(struct htf_string *l, int thread_index, int string_index);
static void _htf_read_region(struct htf_region *r, int thread_index, int region_index);

static void _htf_read_thread_trace(struct htf_trace *trace, int thread_index);

static FILE* _htf_file_open(char* filename, char* mode) {
  htf_log(htf_dbg_lvl_debug, "Open %s with mode %s\n", filename, mode);

  FILE* file = fopen(filename, mode);
  if(file == NULL) {
    htf_error("Cannot open %s: %s\n", filename, strerror(errno));
  }
  return file;
}

static void _htf_mkdir(char* dirname, mode_t mode) {
  if(mkdir(dirname, mode) != 0) {
    if(errno != EEXIST)
      htf_error("mkdir(%s) failed: %s\n", dirname, strerror(errno));
  }
}

void htf_storage_init(const char* dirname) {
  base_dirname = malloc(sizeof(char)*(strlen(dirname)+1));
  strcpy(base_dirname, dirname);
}

static FILE* _htf_get_event_file(int thread_index, htf_event_id_t event_id, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/event_%d", base_dirname, thread_index, HTF_ID(event_id));
  return _htf_file_open(filename, mode);
}

static void _htf_store_event(struct htf_event_summary *e, int thread_index, htf_event_id_t event_id) {
  FILE* file = _htf_get_event_file(thread_index, event_id, "w");

  htf_log(htf_dbg_lvl_debug, "\tStore event %x {.nb_timestamps=%d}\n", HTF_ID(event_id), e->nb_timestamps);

  fwrite(&e->event, sizeof(struct htf_event), 1, file);
  fwrite(&e->nb_timestamps, sizeof(e->nb_timestamps), 1, file);
  fwrite(e->timestamps, sizeof(e->timestamps[0]), e->nb_timestamps, file);
  
  fclose(file);
}

static void _htf_read_event(struct htf_event_summary *e, int thread_index, htf_event_id_t event_id) {
  FILE* file = _htf_get_event_file(thread_index, event_id, "r");

  fread(&e->event, sizeof(struct htf_event), 1, file);
  fread(&e->nb_timestamps, sizeof(e->nb_timestamps), 1, file);
  e->timestamps = malloc(e->nb_timestamps * sizeof(htf_timestamp_t));
  fread(e->timestamps, sizeof(e->timestamps[0]), e->nb_timestamps, file);

  htf_log(htf_dbg_lvl_debug, "\tLoad event %x {.nb_timestamps=%d}\n", HTF_ID(event_id), e->nb_timestamps);

  fclose(file);
}


static FILE* _htf_get_sequence_file(int thread_index, htf_sequence_id_t sequence_id, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/sequence_%d", base_dirname, thread_index, HTF_ID(sequence_id));
  return _htf_file_open(filename, mode);
}

static void _htf_store_sequence(struct htf_sequence *s, int thread_index, htf_sequence_id_t sequence_id) {
  FILE* file = _htf_get_sequence_file(thread_index, sequence_id, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore sequence %x {.size=%d}\n", HTF_ID(sequence_id), s->size);
  
  fwrite(&s->size, sizeof(s->size), 1, file);
  fwrite(s->token, sizeof(s->token[0]), s->size, file);  
  fclose(file);
}

static void _htf_read_sequence(struct htf_sequence *s, int thread_index, htf_sequence_id_t sequence_id) {
  FILE* file = _htf_get_sequence_file(thread_index, sequence_id, "r");
  fread(&s->size, sizeof(s->size), 1, file);
  s->token = malloc(sizeof(htf_token_t) * s->size);
  s->allocated = s->size;
  fread(s->token, sizeof(htf_token_t), s->size, file);  
  fclose(file);

  htf_log(htf_dbg_lvl_debug, "\tLoad sequence %x {.size=%d}\n", HTF_ID(sequence_id), s->size);

  if(htf_debug_level >= htf_dbg_lvl_debug) {
    for(int i = 0; i<s->size && i< 15; i++) {
      printf("{%x.%x} ", HTF_TOKEN_TYPE(s->token[i]), HTF_TOKEN_ID(s->token[i]));
    }
    printf("\n");
  }
}



static FILE* _htf_get_loop_file(int thread_index, htf_loop_id_t loop_id, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/loop_%d", base_dirname, thread_index, HTF_ID(loop_id));
  return _htf_file_open(filename, mode);
}

static void _htf_store_loop(struct htf_loop *l, int thread_index, htf_loop_id_t loop_id) {
  FILE* file = _htf_get_loop_file(thread_index, loop_id, "w");
  htf_log(htf_dbg_lvl_debug, "\tStore loop %x {.nb_iterations=%d, .token=%x.%x}\n",
	  HTF_ID(loop_id), l->nb_iterations, l->token.type, l->token.id );

  fwrite(&l->nb_iterations, sizeof(l->nb_iterations), 1, file);
  fwrite(&l->token, sizeof(l->token), 1, file);
  fclose(file);
}

static void _htf_read_loop(struct htf_loop *l, int thread_index, htf_loop_id_t loop_id) {
  FILE* file = _htf_get_loop_file(thread_index, loop_id, "r");
  fread(&l->nb_iterations, sizeof(l->nb_iterations), 1, file);
  fread(&l->token, sizeof(l->token), 1, file);
  fclose(file);
  htf_log(htf_dbg_lvl_debug, "\tLoad loop %x {.nb_iterations=%d, .token=%x.%x}\n",
	  HTF_ID(loop_id), l->nb_iterations, l->token.type, l->token.id );
}


static FILE* _htf_get_string_file(int thread_index, int string_index, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/string_%d", base_dirname, thread_index, string_index);
  return _htf_file_open(filename, mode);
}

static void _htf_store_string(struct htf_string *s, int thread_index, int string_index) {
  FILE* file = _htf_get_string_file(thread_index, string_index, "w");

  htf_log(htf_dbg_lvl_debug, "\tStore String %d {.ref=%d, .length=%d, .str='%s'}\n",
	  string_index, s->string_ref, s->length, s->str);

  fwrite(&s->string_ref, sizeof(s->string_ref), 1, file);
  fwrite(&s->length, sizeof(s->length), 1, file);
  fwrite(s->str, sizeof(char), s->length, file);

  fclose(file);
}

static void _htf_read_string(struct htf_string *s, int thread_index, int string_index) {
  FILE* file = _htf_get_string_file(thread_index, string_index, "r");

  fread(&s->string_ref, sizeof(s->string_ref), 1, file);
  fread(&s->length, sizeof(s->length), 1, file);
  s->str = malloc(sizeof(char) * s->length);
  htf_assert(s->str);
  fread(s->str, sizeof(char), s->length, file);

  fclose(file);

  htf_log(htf_dbg_lvl_debug, "\tLoad String %d {.ref=%d, .length=%d, .str='%s'}\n",
	  string_index, s->string_ref, s->length, s->str);
}


static FILE* _htf_get_region_file(int thread_index, int region_index, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/region_%d", base_dirname, thread_index, region_index);
  return _htf_file_open(filename, mode);
}

static void _htf_store_region(struct htf_region *r, int thread_index, int region_index) {
  FILE* file = _htf_get_region_file(thread_index, region_index, "w");

  htf_log(htf_dbg_lvl_debug, "\tStore Region %d {.ref=%d, .str=%d}\n",
	  region_index, r->region_ref, r->string_ref);

  fwrite(&r->region_ref, sizeof(r->region_ref), 1, file);
  fwrite(&r->string_ref, sizeof(r->string_ref), 1, file);
  fclose(file);
}

static void _htf_read_region(struct htf_region *r, int thread_index, int region_index) {
  FILE* file = _htf_get_region_file(thread_index, region_index, "r");

  fread(&r->region_ref, sizeof(r->region_ref), 1, file);
  fread(&r->string_ref, sizeof(r->string_ref), 1, file);

  fclose(file);

  htf_log(htf_dbg_lvl_debug, "\tLoad Region %d {.ref=%d, .str=%d}\n",
	  region_index, r->region_ref, r->string_ref);
}


static FILE* _htf_get_thread_file(int thread_index, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d.tok", base_dirname, thread_index);
  return _htf_file_open(filename, mode);
}

static void _htf_store_thread_trace(struct htf_trace *trace, int thread_index) {
  FILE* token_file = _htf_get_thread_file(thread_index, "w");

  struct htf_thread_trace *th = trace->threads[thread_index];

  htf_log(htf_dbg_lvl_verbose, "\tThread %d: {.nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n",
	  thread_index, th->nb_events,  th->nb_sequences, th->nb_loops);

  fwrite(&th->nb_events, sizeof(th->nb_events), 1, token_file);
  fwrite(&th->nb_sequences, sizeof(th->nb_sequences), 1, token_file);
  fwrite(&th->nb_loops, sizeof(th->nb_loops), 1, token_file);
  fwrite(&th->nb_strings, sizeof(th->nb_strings), 1, token_file);
  fwrite(&th->nb_regions, sizeof(th->nb_regions), 1, token_file);
  fclose(token_file);

  char dir_filename[1024];
  snprintf(dir_filename, 1024, "%s/%d", base_dirname, thread_index);
  _htf_mkdir(dir_filename, 0777);

  for(int i=0; i<th->nb_events; i++)
    _htf_store_event(&th->events[i], thread_index, HTF_EVENT_ID(i));

  for(int i=0; i<th->nb_sequences; i++)
    _htf_store_sequence(&th->sequences[i], thread_index, HTF_SEQUENCE_ID(i));

  for(int i=0; i<th->nb_loops; i++)
    _htf_store_loop(&th->loops[i], thread_index, HTF_LOOP_ID(i));

  for(int i=0; i<th->nb_strings; i++)
    _htf_store_string(&th->strings[i], thread_index, i);

  for(int i=0; i<th->nb_regions; i++)
    _htf_store_region(&th->regions[i], thread_index, i);
}

static void _htf_read_thread_trace(struct htf_trace *trace, int thread_index) {
  FILE* token_file = _htf_get_thread_file(thread_index, "r");

  trace->threads[thread_index] = malloc(sizeof(struct htf_thread_trace));
  struct htf_thread_trace *th = trace->threads[thread_index];
  fread(&th->nb_events, sizeof(th->nb_events), 1, token_file);
  fread(&th->nb_sequences, sizeof(th->nb_sequences), 1, token_file);
  fread(&th->nb_loops, sizeof(th->nb_loops), 1, token_file);
  fread(&th->nb_strings, sizeof(th->nb_strings), 1, token_file);
  fread(&th->nb_regions, sizeof(th->nb_regions), 1, token_file);

  th->events = malloc(sizeof(struct htf_event_summary) * th->nb_events);
  th->sequences = malloc(sizeof(struct htf_sequence) * th->nb_sequences);
  th->loops = malloc(sizeof(struct htf_loop) * th->nb_loops);
  th->strings = malloc(sizeof(struct htf_string) * th->nb_strings);
  th->regions = malloc(sizeof(struct htf_region) * th->nb_regions);
  fclose(token_file);

  htf_log(htf_dbg_lvl_verbose, "Reading %d events\n", th->nb_events);
  for(int i=0; i<th->nb_events; i++)
    _htf_read_event(&th->events[i], thread_index, HTF_EVENT_ID(i));

  htf_log(htf_dbg_lvl_verbose, "Reading %d sequences\n", th->nb_sequences);
  for(int i=0; i<th->nb_sequences; i++)
    _htf_read_sequence(&th->sequences[i], thread_index, HTF_SEQUENCE_ID(i));

  htf_log(htf_dbg_lvl_verbose, "Reading %d loops\n", th->nb_loops);
  for(int i=0; i<th->nb_loops; i++)
    _htf_read_loop(&th->loops[i], thread_index, HTF_LOOP_ID(i));

  htf_log(htf_dbg_lvl_verbose, "Reading %d strings\n", th->nb_strings);
  for(int i=0; i<th->nb_strings; i++)
    _htf_read_string(&th->strings[i], thread_index, i);

  htf_log(htf_dbg_lvl_verbose, "Reading %d regions\n", th->nb_regions);
  for(int i=0; i<th->nb_regions; i++)
    _htf_read_region(&th->regions[i], thread_index, i);
}



void htf_storage_finalize(struct htf_trace*trace) {
  if(! trace)
    return;

  htf_log(htf_dbg_lvl_verbose, "Write trace to path %s\n", base_dirname);
  
  _htf_mkdir(base_dirname, 0777);

  char main_filename[1024];
  snprintf(main_filename, 1024, "%s/main.htf", base_dirname);
  FILE* main_file = _htf_file_open(main_filename, "w");
  fwrite(&trace->nb_threads, sizeof(int), 1, main_file);
  fclose(main_file);

  for(int i = 0; i<trace->nb_threads; i++) {
    _htf_store_thread_trace(trace, i);
  }
}

void htf_read_trace(struct htf_trace*trace, char* main_filename) {
  base_dirname=malloc(sizeof(char)*(strlen(main_filename)+1));
  strcpy(base_dirname, main_filename);
  base_dirname = dirname(base_dirname);

  FILE* main_file = _htf_file_open(main_filename, "r");
  fread(&trace->nb_threads, sizeof(int), 1, main_file);
  fclose(main_file);

  trace->threads = malloc(sizeof(struct htf_thread_trace*) * trace->nb_threads);
  pthread_mutex_init(&trace->lock, NULL);

  for(int i = 0; i<trace->nb_threads; i++) {
    _htf_read_thread_trace(trace, i);
  }
}
