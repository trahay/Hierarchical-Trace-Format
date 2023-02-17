#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>
#include <pthread.h>

#include "event.h"
#include "timestamp.h"
#include "trace_storage.h"

static char *base_dirname = NULL;

FILE* file_open(char* filename, char* mode) {
  printf("Open %s with mode %s\n", filename, mode);

  FILE* file = fopen(filename, mode);
  if(file == NULL) {
    fprintf(stderr, "Cannot open %s: %s\n", filename, strerror(errno));
    abort();
  }
  return file;
}

void trace_storage_init() {
  base_dirname = getenv("TRACE_FILENAME");
  if(! base_dirname) {
    base_dirname=malloc(sizeof(char)*100);
    snprintf(base_dirname, 100, "trace");
  }
}

void write_thread_event(struct event_summary *e, int thread_index, int event_id) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/event_%d", base_dirname, thread_index, event_id);
  FILE* file = file_open(filename, "w");

  fwrite(&e->event, sizeof(struct event), 1, file);
  fwrite(&e->nb_timestamps, sizeof(e->nb_timestamps), 1, file);
  fwrite(e->timestamps, sizeof(e->timestamps[0]), e->nb_timestamps, file);
  
  fclose(file);
}

void read_thread_event(struct event_summary *e, int thread_index, int event_id) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/event_%d", base_dirname, thread_index, event_id);
  FILE* file = file_open(filename, "r");

  fread(&e->event, sizeof(struct event), 1, file);
  fread(&e->nb_timestamps, sizeof(e->nb_timestamps), 1, file);
  e->timestamps = malloc(e->nb_timestamps * sizeof(timestamp_t));
  fread(e->timestamps, sizeof(e->timestamps[0]), e->nb_timestamps, file);
  
  fclose(file);
}


void write_thread_sequence(struct sequence *s, int thread_index, int sequence_id) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/sequence_%d", base_dirname, thread_index, sequence_id);
  FILE* file = file_open(filename, "w");

  fwrite(&s->length, sizeof(s->length), 1, file);
  fwrite(s->token, sizeof(s->token[0]), s->length, file);
  
  fclose(file);
}

void read_thread_sequence(struct sequence *s, int thread_index, int sequence_id) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/sequence_%d", base_dirname, thread_index, sequence_id);
  FILE* file = file_open(filename, "r");

  fread(&s->length, sizeof(s->length), 1, file);
  fread(s->token, sizeof(token_t), s->length, file);
  
  fclose(file);
}

void write_thread_loop(struct loop *l, int thread_index, int loop_id) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/loop_%d", base_dirname, thread_index, loop_id);
  FILE* file = file_open(filename, "w");
  fwrite(&l, sizeof(l), 1, file);
  fclose(file);
}

void read_thread_loop(struct loop *l, int thread_index, int loop_id) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/loop_%d", base_dirname, thread_index, loop_id);
  FILE* file = file_open(filename, "r");
  fread(&l, sizeof(l), 1, file);
  fclose(file);
}

void write_thread_trace(struct trace*trace, int thread_index) {
  /* write tokens */
  char token_filename[1024];
  snprintf(token_filename, 1024, "%s/%d.tok", base_dirname, thread_index);
  FILE* token_file = file_open(token_filename, "w");

  struct thread_trace *th = trace->threads[thread_index];
  fwrite(&th->nb_tokens, sizeof(th->nb_tokens), 1, token_file);
  fwrite(&th->nb_events, sizeof(th->nb_events), 1, token_file);
  fwrite(&th->nb_sequences, sizeof(th->nb_sequences), 1, token_file);
  fwrite(&th->nb_loops, sizeof(th->nb_loops), 1, token_file);

  fwrite(th->tokens,
	 sizeof(token_t),
	 th->nb_tokens,
	 token_file);
  fclose(token_file);

  char dir_filename[1024];
  snprintf(dir_filename, 1024, "%s/%d", base_dirname, thread_index);
  mkdir(dir_filename, 0777);

  for(int i=0; i<th->nb_events; i++)
    write_thread_event(&th->events[i], thread_index, i);

  for(int i=0; i<th->nb_sequences; i++)
    write_thread_sequence(&th->sequences[i], thread_index, i);

  for(int i=0; i<th->nb_loops; i++)
    write_thread_loop(&th->loops[i], thread_index, i);
}


void read_thread_trace(struct trace*trace, int thread_index) {
  char token_filename[1024];
  snprintf(token_filename, 1024, "%s/%d.tok", base_dirname, thread_index);
  FILE* token_file = file_open(token_filename, "r");
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

  fread(th->tokens,
	 sizeof(token_t),
	 th->nb_tokens,
	 token_file);
  fclose(token_file);

  printf("Reading %d events\n", th->nb_events);
  for(int i=0; i<th->nb_events; i++)
    read_thread_event(&th->events[i], thread_index, i);

  printf("Reading %d sequences\n", th->nb_sequences);
  for(int i=0; i<th->nb_sequences; i++)
    read_thread_sequence(&th->sequences[i], thread_index, i);

  printf("Reading %d loops\n", th->nb_loops);
  for(int i=0; i<th->nb_loops; i++)
    read_thread_loop(&th->loops[i], thread_index, i);
}

void write_trace(struct trace*trace) {
  mkdir(base_dirname, 0777);

  char main_filename[1024];
  snprintf(main_filename, 1024, "%s/main.htf", base_dirname);
  FILE* main_file = file_open(main_filename, "w");
  fwrite(&trace->nb_threads, sizeof(int), 1, main_file);
  fclose(main_file);

  for(int i = 0; i<trace->nb_threads; i++) {
    write_thread_trace(trace, i);
  }
}

void read_trace(struct trace*trace, char* main_filename) {
  base_dirname=malloc(sizeof(char)*(strlen(main_filename)+1));
  strcpy(base_dirname, main_filename);
  base_dirname = dirname(base_dirname);
  printf("Open %s -> %s\n", main_filename, base_dirname);

  FILE* main_file = file_open(main_filename, "r");
  fread(&trace->nb_threads, sizeof(int), 1, main_file);
  fclose(main_file);

  trace->threads = malloc(sizeof(struct thread_trace*) * trace->nb_threads);
  pthread_mutex_init(&trace->lock, NULL);

  for(int i = 0; i<trace->nb_threads; i++) {
    read_thread_trace(trace, i);
  }
}
