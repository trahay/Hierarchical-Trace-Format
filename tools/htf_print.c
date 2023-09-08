/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
#include <stdlib.h>
#include <string.h>

#include "htf.h"
#include "htf_read.h"

static int show_structure = 0;
static int per_thread = 0;
static char* structure_indent[MAX_CALLSTACK_DEPTH];
static short store_timestamps = 1;

static int print_timestamp = 1;
static int print_duration = 0;
static int unroll_loops = 0;
static int verbose = 0;

static void _print_timestamp(htf_timestamp_t ts) {
  if (print_timestamp) {
    printf("%21.9lf\t", ts / 1e9);
  }
}
static void _print_timestamp_header() {
  if (print_timestamp) {
    printf("%21s\t", "Timestamp");
  }
}

static void _print_duration(htf_timestamp_t d) {
  if (print_duration) {
    printf("%21.9lf\t", d / 1e9);
  }
}

static void _print_duration_header() {
  if (print_duration) {
    printf("%21s\t", "Duration");
  }
}

static void _print_indent(const char* current_indent) {
  if(current_indent)
    printf(current_indent);
}

/* Print one event */
static void print_event(const char* current_indent,
			struct htf_thread* thread,
			htf_token_t token,
			struct htf_event_occurence* e) {
  _print_timestamp(e->timestamp);
  _print_duration( e->duration);
  _print_indent(current_indent);

  if (!per_thread)
    printf("%s\t", htf_get_thread_name(thread));
  if(verbose) {
    htf_print_token(thread, token);
    printf("\t");
  }
  htf_print_event(thread, &e->event);
  printf("\n");
}

static void print_sequence(const char* current_indent,
			   struct htf_thread* thread,
                           htf_token_t token,
                           struct htf_sequence_occurence* seq,
                           struct htf_loop_occurence* containing_loop) {
  struct htf_sequence* s = seq->sequence;
  htf_timestamp_t ts = seq->timestamp;
  htf_timestamp_t duration = seq->duration;

  _print_timestamp(ts);
  _print_duration( duration);
  _print_indent(current_indent);

  if (show_structure) {
    printf("Sequence ");
    htf_print_token(thread, token);
  }


  if (!per_thread)
    printf("%s\t", htf_get_thread_name(thread));

  if (verbose) {
    htf_print_token(thread, token);
    printf("\t");
    for (unsigned i = 0; i < s->size; i++) {
      htf_print_token(thread, s->token[i]);
      printf(" ");
    }
  }

  if(containing_loop) {    
    htf_timestamp_t mean = containing_loop->duration / containing_loop->nb_iterations;
    if (mean < duration)
      printf("-");
    else
      printf("+");
    uint64_t diff = (mean < duration) ? duration - mean : mean - duration;
    float percentile = (float)diff / (float)mean * 100;
    printf("%5.2f%%", percentile);
  }  
  printf("\n");
}

static void print_loop(const char* current_indent,
		       struct htf_thread* thread,
		       htf_token_t token,
		       struct htf_loop_occurence* loop) {

  _print_timestamp(loop->timestamp);
  _print_duration(loop->duration);
  _print_indent(current_indent);

  if (show_structure) {
      printf("Loop ");
  }
  
  if (!per_thread)
    printf("%s\t", htf_get_thread_name(thread));

  struct htf_loop* l = loop->loop;

  htf_print_token(thread, token);
  printf("\t%d * ", loop->nb_iterations);
  htf_print_token(thread, l->token);
  printf("\n");
}

static void print_token(struct htf_thread* thread,
                        struct htf_token* t,
                        htf_occurence* e,
                        int depth,
                        int last_one,
                        struct htf_loop_occurence* containing_loop) {
  htf_log(htf_dbg_lvl_verbose, "Reading token(%x.%x) for thread %s\n", t->type, t->id, htf_get_thread_name(thread));

  // Prints the structure of the sequences and the loops
  char current_indent[MAX_CALLSTACK_DEPTH];
  current_indent[0]='\0';
  if (show_structure) {

    structure_indent[depth - 1] = last_one ? "╰" : "├";
    DOFOR(i, depth) {
      strcat(current_indent, structure_indent[i]);
    }
    if (depth) {
      if (t->type != HTF_TYPE_EVENT) {
        strcat(current_indent, (containing_loop == NULL) ? "┬" : "─");
      } else {
        strcat(current_indent, "─");
      }
    }
    structure_indent[depth - 1] = last_one ? " " : "│";
  }

  // Prints the token we first started with
  switch (t->type) {
  case HTF_TYPE_INVALID:
    htf_error("Type is invalid\n");
    break;
  case HTF_TYPE_EVENT:
    print_event(current_indent, thread, *t, &e->event_occurence);
    break;
  case HTF_TYPE_SEQUENCE: {
    if (show_structure)
      print_sequence(current_indent, thread, *t, &e->sequence_occurence, containing_loop);
    break;
  }
  case HTF_TYPE_LOOP: {
    print_loop(current_indent, thread, *t, &e->loop_occurence);
    break;
  }
  }
}

/*static void free_occurence(htf_token_t token, htf_occurence* occurence) {
  htf_error("This function does not work yet\n");
  // Not too sure what to do with all that tbf
  switch (token.type) {
  case HTF_TYPE_SEQUENCE: {
    struct htf_sequence_occurence seq = occurence->sequence_occurence;
    DOFOR(i, (int)seq.sequence->size) {
      free_occurence(seq.sequence->token[i], &((htf_occurence*)seq.full_sequence)[i]);
    }
    free(seq.full_sequence);
    free(&(occurence->sequence_occurence));
    break;
  }
  case HTF_TYPE_LOOP: {
    struct htf_loop_occurence loop = occurence->loop_occurence;
    DOFOR(i, (int)loop.nb_iterations) {
      free_occurence(loop.loop->token, &loop.full_loop[i]);
    }
    free(loop.full_loop);
    free(&(occurence->loop_occurence));
    break;
  }
  case HTF_TYPE_EVENT:
    // Fails here ? Idk why
    free(&(occurence->event_occurence));
    break;
  default:
    htf_error("This shouldn't happen\n");
  }
}*/

static void display_sequence(struct htf_thread_reader* reader,
                             htf_token_t token,
                             struct htf_sequence_occurence* occurence,
                             int depth) {
  if (occurence) {
    load_savestate(reader, occurence->savestate);
    enter_block(reader, token);
  }
  htf_occurence* current_level = NULL;
  htf_token_t* current_level_token = NULL;
  int size = 0;
  htf_read_thread_cur_level(reader, &current_level, &current_level_token, &size);
  DOFOR(i, size) {
    print_token(reader->thread_trace, &current_level_token[i], &current_level[i], depth, i == (size - 1), NULL);
    if (current_level_token[i].type == HTF_TYPE_SEQUENCE) {
      display_sequence(reader, current_level_token[i], &current_level[i].sequence_occurence, depth + 1);
    }
    if (current_level_token[i].type == HTF_TYPE_LOOP) {
      struct htf_loop_occurence loop = current_level[i].loop_occurence;


      if(unroll_loops) {
	DOFOR(j, (int)loop.nb_iterations) {
	  /* get the jth iteration of the loop */
	  struct htf_sequence_occurence *seq = &loop.full_loop[j];
	  display_sequence(reader, loop.loop->token, seq, depth + 1);
	}
      } else {
	struct htf_sequence_occurence *seq = &loop.full_loop[0];
	display_sequence(reader, loop.loop->token, seq, depth + 1);
      }
    }
  }
  if (occurence) {
    occurence->full_sequence = current_level;
  }
  leave_block(reader);
}

/* Print all the events of a thread */
static void print_thread(struct htf_archive* trace, struct htf_thread* thread) {
  printf("Reading events for thread %u (%s):\n", thread->id, htf_get_thread_name(thread));
  _print_timestamp_header();
  _print_duration_header();

  printf("Event\n");
  
  struct htf_thread_reader reader;
  int reader_options = OPTION_NONE;
  if (show_structure)
    reader_options |= OPTION_SHOW_STRUCTURE;
  if (!store_timestamps || trace->store_timestamps == 0)
    reader_options |= OPTION_NO_TIMESTAMPS;

  htf_read_thread_iterator_init(trace, &reader, thread->id, reader_options);
  display_sequence(&reader, HTF_TOKENIZE(HTF_TYPE_SEQUENCE, 0), NULL, 0);
}

/** Compare the timestamps of the current token on each thread and select the smallest timestamp.
 * This fills the htf_token t, and the event_occurence e if it's an event,
 * and returns the index of the selected thread (or -1 at the end of the trace).
 */
static int get_next_token(struct htf_thread_reader* readers, int nb_threads, struct htf_token* t, htf_occurence* e) {
  struct htf_token cur_t;
  htf_timestamp_t min_ts = HTF_TIMESTAMP_INVALID;
  int min_index = -1;

  for (int i = 0; i < nb_threads; i++) {
    if (htf_read_thread_cur_token(&readers[i], &cur_t, NULL) == 0) {
      htf_timestamp_t ts = htf_get_starting_timestamp(&readers[i], cur_t);
      if (min_ts == HTF_TIMESTAMP_INVALID || ts < min_ts) {
        min_index = i;
        min_ts = ts;
      }
    }
  }

  if (min_index >= 0) {
    htf_read_thread_cur_token(&readers[min_index], t, e);
    htf_move_to_next_token(&readers[min_index]);
  }

  return min_index;
}

/* Print all the events of all the threads sorted by timestamp */
void print_trace(struct htf_archive* trace) {
  struct htf_thread_reader* readers = malloc(sizeof(struct htf_thread_reader) * (trace->nb_threads));
  int reader_options = OPTION_NONE;
  if (show_structure)
    reader_options |= OPTION_SHOW_STRUCTURE;

  for (int i = 0; i < trace->nb_threads; i++) {
    htf_read_thread_iterator_init(trace, &readers[i], trace->threads[i]->id, reader_options);
  }

  printf("Timestamp\tDuration\tThread Name\tEvent\n");

  //	htf_occurence e;
  //	struct htf_token t;
  //	int thread_index = -1;
  //	while ((thread_index = get_next_token(readers, trace->nb_threads, &t, &e)) >= 0) {
  //		print_token(&readers[thread_index], &t, &e);
  //	}
}

void usage(const char* prog_name) {
  printf("Usage: %s [OPTION] trace_file\n", prog_name);
  printf("\t-T          Print events per thread\n");
  printf("\t-S          Structure mode\n");
  printf("\t-v          Verbose mode\n");
  printf("\t-d          Print duration of events\n");
  printf("\t-t          Don't print timestamps\n");
  printf("\t-u          Unroll loops\n");
  printf("\t-?  -h      Display this help and exit\n");
}

int main(int argc, char** argv) {
  int nb_opts = 0;
  char* trace_name = NULL;

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-v")) {
      htf_debug_level_set(htf_dbg_lvl_debug);
      nb_opts++;
    } else if (!strcmp(argv[i], "-T")) {
      per_thread = 1;
      nb_opts++;
    } else if (!strcmp(argv[i], "-S")) {
      show_structure = 1;
      nb_opts++;
    } else if (!strcmp(argv[i], "-d")) {
      print_duration = 1;
      nb_opts++;
    } else if (!strcmp(argv[i], "-t")) {
      print_timestamp = 0;
      nb_opts++;
    } else if (!strcmp(argv[i], "-u")) {
      unroll_loops = 1;
      nb_opts++;
    } else if (!strcmp(argv[i], "--no-timestamps")) {
      setenv("STORE_TIMESTAMPS", "FALSE", 0);
      print_timestamp = 0;
      store_timestamps = 0;
      nb_opts++;
    } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-?")) {
      usage(argv[0]);
      return EXIT_SUCCESS;
    } else {
      /* Unknown parameter name. It's probably the trace's path name. We can stop
       * parsing the parameter list.
       */
      break;
    }
  }

  if(! show_structure) {
    unroll_loops = 1;
  }

  trace_name = argv[nb_opts + 1];
  if (trace_name == NULL) {
    usage(argv[0]);
    return EXIT_SUCCESS;
  }

  struct htf_archive trace;
  htf_read_archive(&trace, trace_name);
  store_timestamps = trace.store_timestamps;

  if (per_thread) {
    for (int i = 0; i < trace.nb_threads; i++) {
      printf("\n");
      print_thread(&trace, trace.threads[i]);
    }
  } else {
    htf_error("This is currently buggy so don't do it\n");
    print_trace(&trace);
  }

  return EXIT_SUCCESS;
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
