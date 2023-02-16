#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "liblock.h"
#include "timestamp.h"
#include "trace_storage.h"

static char *base_dirname = NULL;

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
  FILE* file = fopen(filename, "w");
  assert(file);

  fwrite(&e->event, sizeof(struct event), 1, file);
  fwrite(&e->nb_timestamps, sizeof(e->nb_timestamps), 1, file);
  fwrite(e->timestamps, sizeof(e->timestamps[0]), e->nb_timestamps, file);
  
  fclose(file);
}


void write_thread_sequence(struct sequence *s, int thread_index, int sequence_id) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/sequence_%d", base_dirname, thread_index, sequence_id);
  FILE* file = fopen(filename, "w");
  assert(file);

  fwrite(&s->length, sizeof(s->length), 1, file);
  fwrite(s->token, sizeof(s->token[0]), s->length, file);
  
  fclose(file);
}

void write_thread_loop(struct loop *l, int thread_index, int loop_id) {
  char filename[1024];
  snprintf(filename, 1024, "%s/%d/loop_%d", base_dirname, thread_index, loop_id);
  FILE* file = fopen(filename, "w");
  assert(file);

  fwrite(&l, sizeof(l), 1, file);
  
  fclose(file);
}

void write_thread_trace(struct trace*trace, int thread_index) {
  /* write tokens */
  char token_filename[1024];
  snprintf(token_filename, 1024, "%s/%d.tok", base_dirname, thread_index);
  FILE* token_file = fopen(token_filename, "w");
  assert(token_file);

  struct thread_trace *th = trace->threads[thread_index];
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

void write_trace(struct trace*trace) {
  mkdir(base_dirname, 0777);

  char main_filename[1024];
  snprintf(main_filename, 1024, "%s/main.htf", base_dirname);
  FILE* main_file = fopen(main_filename, "w");
  fwrite(&trace->nb_threads, sizeof(int), 1, main_file);
  fclose(main_file);

  for(int i = 0; i<trace->nb_threads; i++) {
    write_thread_trace(trace, i);
  }
}
