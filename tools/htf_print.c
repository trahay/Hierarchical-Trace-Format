#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

#include "htf.h"
#include "htf_read.h"
#include "htf_archive.h"

/* Print one event */
static void print_event(struct htf_thread* thread, htf_token_t token, struct htf_event_occurence* e) {
	printf("%.9lf\t\t%s\t", e->timestamp / 1e9, htf_get_thread_name(thread));
	htf_print_token(thread, token);
	printf("\t");
	htf_print_event(thread, &e->event);
	printf("\n");
}

static void print_sequence(struct htf_thread* thread, htf_token_t token) {
	printf("Sequence %x = ", token.id);
	struct htf_sequence* seq = htf_get_sequence(thread, HTF_SEQUENCE_ID(token.id));
	for (unsigned i = 0; i < seq->size; i++) {
		htf_print_token(thread, seq->token[i]);
		printf(" ");
	}
	printf("\n");
}

static void print_loop(struct htf_thread* thread, htf_token_t token) {
	struct htf_loop* loop = htf_get_loop(thread, HTF_LOOP_ID(token.id));
	printf("Loop %x = %d * ", token.id, loop->nb_iterations);
	htf_print_token(thread, loop->token);
	printf("\n");
}

/* Print all the events of a thread */
static void print_thread(struct htf_archive* trace, struct htf_thread* thread, int show_structure) {
	printf("Reading events for thread %u (%s):\n", thread->id, htf_get_thread_name(thread));
	printf("Timestamp\t\tThread Name\tTag\tEvent\n");

	struct htf_thread_reader reader;
	htf_read_thread_iterator_init(trace, &reader, thread->id);

	struct htf_event_occurence e;
	struct htf_token t;
	while (htf_read_thread_next_token(&reader, &t, &e) == 0) {
		htf_log(htf_dbg_lvl_verbose, "Reading token(%x.%x)\n", t.type, t.id);
		if (show_structure) {
			if (reader.depth == reader.current_frame) {
				for (int i = 0; i < reader.depth; i++)
					printf("│ ");
			}
			if (reader.depth < reader.current_frame) {
				// Means we just went deeper
				for (int i = 0; i < reader.depth; i++)
					printf("│ ");
			}
			if (reader.depth > reader.current_frame) {
				// Means we ended some blocks
				for (int i = 0; i < reader.current_frame; i++)
					printf("│ ");
				for (int i = (reader.current_frame >= 0) ? reader.current_frame : 0; i < reader.depth; i++)
					printf("╰─");
			}
		}
		reader.depth = reader.current_frame;
		switch (t.type) {
			case HTF_TYPE_INVALID:
				htf_error("Type is invalid\n");
				break;
			case HTF_TYPE_EVENT:
				print_event(thread, t, &e);
				break;
			case HTF_TYPE_SEQUENCE:
				print_sequence(thread, t);
				break;
			case HTF_TYPE_LOOP:
				print_loop(thread, t);
				break;
		}
	}
}

/* compare the timestamps of the current event on each thread and select the smallest timestamp
 * This fills the event_occurence e and returns the index of the selected thread (or -1 at the end
 * of the trace)
 */
static int get_next_event(struct htf_thread_reader *readers,
			  int nb_threads,
			  struct htf_event_occurence *e) {

  struct htf_event_occurence cur_e;
  htf_timestamp_t min_ts = HTF_TIMESTAMP_INVALID;
  int min_index = -1;

  for(int i=0; i< nb_threads; i++) {
    if(htf_read_thread_cur_event(&readers[i], &cur_e) == 0) {
      if( min_ts == HTF_TIMESTAMP_INVALID || min_ts > cur_e.timestamp) {
	min_index = i;
	min_ts = cur_e.timestamp;
      }
    }
  }

  if(min_index>=0) {
    htf_read_thread_next_event(&readers[min_index], e);
  }

  return  min_index;
}

/* Print all the events of all the threads sorted by timestamp */
void print_trace(struct htf_archive *trace) {
  int nb_threads = trace->nb_threads;
  struct htf_thread_reader *readers =  malloc(sizeof(struct htf_thread_reader) * (nb_threads));
  for(int i=0; i<trace->nb_threads; i++) {
    htf_read_thread_iterator_init(trace, &readers[i], trace->threads[i]->id);
  }

  struct htf_event_occurence e;
  int thread_index = -1;
  while((thread_index = get_next_event(readers, nb_threads, &e)) >= 0) {
		print_event(readers[thread_index].thread_trace, HTF_TOKENIZE(1, 0), &e);
	}
}

void usage(const char *prog_name) {
  printf("Usage: %s [OPTION] trace_file\n", prog_name);
  printf("\t-T          Print events per thread\n");
  printf("\t-v          Verbose mode\n");
  printf("\t-?  -h      Display this help and exit\n");
}

int main(int argc, char**argv) {
	int per_thread = 0;
	int show_structure = 0;
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
		} else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-?")) {
			usage(argv[0]);
			return EXIT_SUCCESS;
		} else {
			/* Unknown parameter name. It's probably the program name. We can stop
			 * parsing the parameter list.
			 */
			break;
		}
	}

  trace_name = argv[nb_opts + 1];
  if (trace_name == NULL) {
    usage(argv[0]);
    return EXIT_SUCCESS;
  }

  struct htf_archive trace;
  htf_read_archive(&trace, trace_name);

  if(per_thread) {
    for(int i=0; i< trace.nb_threads; i++) {
			printf("\n");
			print_thread(&trace, trace.threads[i], show_structure);
		}
	} else {
    print_trace(&trace);
  }

  return EXIT_SUCCESS;
}
