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

static void _htf_store_event(struct event_summary *e, int thread_index, int event_id);
static void _htf_store_sequence(struct sequence *s, int thread_index, int sequence_id);
static void _htf_store_loop(struct loop *l, int thread_index, int loop_id);
static void _htf_store_thread_trace(struct trace *trace, int thread_index);

static void _htf_read_event(struct event_summary *e, int thread_index, int event_id);
static void _htf_read_sequence(struct sequence *s, int thread_index, int sequence_id);
static void _htf_read_loop(struct loop *l, int thread_index, int loop_id);
static void _htf_read_thread_trace(struct trace *trace, int thread_index);

static FILE* _htf_file_open(char* filename, char* mode) {
  htf_log(dbg_lvl_debug, "Open %s with mode %s\n", filename, mode);

  FILE* file = fopen(filename, mode);
  if(file == NULL) {
    htf_error("Cannot open %s: %s\n", filename, strerror(errno));
  }
  return file;
}

void htf_storage_init(const char* dirname) {
  base_dirname = malloc(sizeof(char)*(strlen(dirname)+1));
  strcpy(base_dirname, dirname);
}

static FILE* _htf_get_event_file(int thread_index, int event_id, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/event_%d", base_dirname, thread_index, event_id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_event(struct event_summary *e, int thread_index, int event_id) {
  FILE* file = _htf_get_event_file(thread_index, event_id, "w");

  fwrite(&e->event, sizeof(struct event), 1, file);
  fwrite(&e->nb_timestamps, sizeof(e->nb_timestamps), 1, file);
  fwrite(e->timestamps, sizeof(e->timestamps[0]), e->nb_timestamps, file);
  
  fclose(file);
}

static void _htf_read_event(struct event_summary *e, int thread_index, int event_id) {
  FILE* file = _htf_get_event_file(thread_index, event_id, "r");

  fread(&e->event, sizeof(struct event), 1, file);
  fread(&e->nb_timestamps, sizeof(e->nb_timestamps), 1, file);
  e->timestamps = malloc(e->nb_timestamps * sizeof(timestamp_t));
  fread(e->timestamps, sizeof(e->timestamps[0]), e->nb_timestamps, file);
  
  fclose(file);
}


static FILE* _htf_get_sequence_file(int thread_index, int sequence_id, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/sequence_%d", base_dirname, thread_index, sequence_id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_sequence(struct sequence *s, int thread_index, int sequence_id) {
  FILE* file = _htf_get_sequence_file(thread_index, sequence_id, "w");
  fwrite(&s->length, sizeof(s->length), 1, file);
  fwrite(s->token, sizeof(s->token[0]), s->length, file);  
  fclose(file);
}

static void _htf_read_sequence(struct sequence *s, int thread_index, int sequence_id) {
  FILE* file = _htf_get_sequence_file(thread_index, sequence_id, "r");
  fread(&s->length, sizeof(s->length), 1, file);
  s->token = malloc(sizeof(token_t) * s->length);
  fread(s->token, sizeof(token_t), s->length, file);  
  fclose(file);
}



static FILE* _htf_get_loop_file(int thread_index, int loop_id, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/loop_%d", base_dirname, thread_index, loop_id);
  return _htf_file_open(filename, mode);
}

static void _htf_store_loop(struct loop *l, int thread_index, int loop_id) {
  FILE* file = _htf_get_loop_file(thread_index, loop_id, "w");
  fwrite(&l, sizeof(l), 1, file);
  fclose(file);
}

static void _htf_read_loop(struct loop *l, int thread_index, int loop_id) {
  FILE* file = _htf_get_loop_file(thread_index, loop_id, "r");
  fread(&l, sizeof(l), 1, file);
  fclose(file);
}


static FILE* _htf_get_thread_file(int thread_index, char* mode) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d.tok", base_dirname, thread_index);
  return _htf_file_open(filename, mode);
}

static void _htf_store_thread_trace(struct trace *trace, int thread_index) {
  FILE* token_file = _htf_get_thread_file(thread_index, "w");

  struct thread_trace *th = trace->threads[thread_index];

  htf_log(dbg_lvl_verbose, "\tThread %d: {.nb_tokens=%d, .nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n",
	  thread_index, th->nb_tokens, th->nb_events,  th->nb_sequences, th->nb_loops);

  fwrite(&th->nb_tokens, sizeof(th->nb_tokens), 1, token_file);
  fwrite(&th->nb_events, sizeof(th->nb_events), 1, token_file);
  fwrite(&th->nb_sequences, sizeof(th->nb_sequences), 1, token_file);
  fwrite(&th->nb_loops, sizeof(th->nb_loops), 1, token_file);

  fwrite(th->tokens, sizeof(token_t), th->nb_tokens, token_file);
  fclose(token_file);

  char dir_filename[1024];
  snprintf(dir_filename, 1024, "%s/%d", base_dirname, thread_index);
  mkdir(dir_filename, 0777);

  for(int i=0; i<th->nb_events; i++)
    _htf_store_event(&th->events[i], thread_index, i);

  for(int i=0; i<th->nb_sequences; i++)
    _htf_store_sequence(&th->sequences[i], thread_index, i);

  for(int i=0; i<th->nb_loops; i++)
    _htf_store_loop(&th->loops[i], thread_index, i);
}

static void _htf_read_thread_trace(struct trace *trace, int thread_index) {
  FILE* token_file = _htf_get_thread_file(thread_index, "r");

  trace->threads[thread_index] = malloc(sizeof(struct thread_trace));
  struct thread_trace *th = trace->threads[thread_index];
  fread(&th->nb_tokens, sizeof(th->nb_tokens), 1, token_file);
  fread(&th->nb_events, sizeof(th->nb_events), 1, token_file);
  fread(&th->nb_sequences, sizeof(th->nb_sequences), 1, token_file);
  fread(&th->nb_loops, sizeof(th->nb_loops), 1, token_file);

  th->tokens = malloc(sizeof(token_t) * th->nb_tokens);
  th->events = malloc(sizeof(struct event_summary) * th->nb_events);
  th->sequences = malloc(sizeof(struct sequence) * th->nb_sequences);
  th->loops = malloc(sizeof(struct loop) * th->nb_loops);

  fread(th->tokens, sizeof(token_t), th->nb_tokens, token_file);
  fclose(token_file);

  htf_log(dbg_lvl_verbose, "Reading %d events\n", th->nb_events);
  for(int i=0; i<th->nb_events; i++)
    _htf_read_event(&th->events[i], thread_index, i);

  htf_log(dbg_lvl_verbose, "Reading %d sequences\n", th->nb_sequences);
  for(int i=0; i<th->nb_sequences; i++)
    _htf_read_sequence(&th->sequences[i], thread_index, i);

  htf_log(dbg_lvl_verbose, "Reading %d loops\n", th->nb_loops);
  for(int i=0; i<th->nb_loops; i++)
    _htf_read_loop(&th->loops[i], thread_index, i);
}



void htf_storage_finalize(struct trace*trace) {
  if(! trace)
    return;

  htf_log(dbg_lvl_verbose, "Write trace to path %s\n", base_dirname);
  
  mkdir(base_dirname, 0777);

  char main_filename[1024];
  snprintf(main_filename, 1024, "%s/main.htf", base_dirname);
  FILE* main_file = _htf_file_open(main_filename, "w");
  fwrite(&trace->nb_threads, sizeof(int), 1, main_file);
  fclose(main_file);

  for(int i = 0; i<trace->nb_threads; i++) {
    _htf_store_thread_trace(trace, i);
  }
}

void htf_read_trace(struct trace*trace, char* main_filename) {
  base_dirname=malloc(sizeof(char)*(strlen(main_filename)+1));
  strcpy(base_dirname, main_filename);
  base_dirname = dirname(base_dirname);

  FILE* main_file = _htf_file_open(main_filename, "r");
  fread(&trace->nb_threads, sizeof(int), 1, main_file);
  fclose(main_file);

  trace->threads = malloc(sizeof(struct thread_trace*) * trace->nb_threads);
  pthread_mutex_init(&trace->lock, NULL);

  for(int i = 0; i<trace->nb_threads; i++) {
    _htf_read_thread_trace(trace, i);
  }
}
