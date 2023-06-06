#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "htf.h"
#include "htf_archive.h"
#include "htf_read.h"

static int show_structure = 0;
static int per_thread = 0;
static long max_depth = MAX_CALLSTACK_DEPTH;

/* Print one event */
static void print_event(struct htf_thread* thread, htf_token_t token, struct htf_event_occurence* e) {
	printf("%.9lf\t%.9lf\t", e->timestamp / 1e9, e->duration / 1e9);
	if (!per_thread)
		printf("%s\t", htf_get_thread_name(thread));
	htf_print_token(thread, token);
	printf("\t");
	htf_print_event(thread, &e->event);
	printf("\n");
}

static void print_sequence(struct htf_thread* thread, htf_token_t token, struct htf_sequence_occurence* seq) {
	struct htf_sequence* s = seq->sequence;
	htf_timestamp_t ts = seq->timestamp;
	htf_timestamp_t duration = seq->duration;
	printf("%.9lf\t%.9lf\t", ts / 1e9, duration / 1e9);
	if (!per_thread)
		printf("%s\t", htf_get_thread_name(thread));
	htf_print_token(thread, token);
	printf("\t");
	for (unsigned i = 0; i < s->size; i++) {
		htf_print_token(thread, s->token[i]);
		printf(" ");
	}
	printf("\n");
}

static void print_loop(struct htf_thread* thread, htf_token_t token, struct htf_loop_occurence* loop, int show_detail) {
	if (show_detail) {
		printf("%.9lf\t%.9lf\t", loop->timestamp / 1e9, loop->duration / 1e9);
	} else {
		printf("Loop");
		if (show_structure)
			printf("         ");
		printf("\t\t\t");
	}
	if (!per_thread)
		printf("%s\t", htf_get_thread_name(thread));

	struct htf_loop* l = htf_get_loop(thread, HTF_TOKEN_TO_LOOP_ID(token));

	htf_print_token(thread, token);
	printf("\t%d * ", l->nb_iterations);
	htf_print_token(thread, l->token);
	printf(" = ");
	// TODO Make sure this is a sequence
	struct htf_sequence* seq = htf_get_sequence(thread, HTF_TOKEN_TO_SEQUENCE_ID(l->token));
	for (unsigned i = 0; i < seq->size; i++) {
		htf_print_token(thread, seq->token[i]);
		printf(" ");
	}
	printf("\n");
}

static void print_token(struct htf_thread_reader* reader, struct htf_token* t, htf_occurence* e) {
	htf_log(htf_dbg_lvl_verbose, "Reading token(%x.%x) for thread %s\n", t->type, t->id,
					htf_get_thread_name(reader->thread_trace));

	// This insures we don't go deeper than necessary
	if (reader->current_frame > max_depth) {
		return;
	}
	// Prints the structure of the sequences and the loops
	if (show_structure) {
		for (int i = 0; i < reader->current_frame; i++)
			printf("--");
	}
	// printf("│ ");
	// printf("├─");
	// printf("╰─");
	// We'll fix this once we use ncurse.
	// Prints the token we first started with
	switch (t->type) {
		case HTF_TYPE_INVALID:
			htf_error("Type is invalid\n");
			break;
		case HTF_TYPE_EVENT:
			print_event(reader->thread_trace, *t, &e->event_occurence);
			break;
		case HTF_TYPE_SEQUENCE: {
			if (show_structure)
				print_sequence(reader->thread_trace, *t, &e->sequence_occurence);
			break;
		}
		case HTF_TYPE_LOOP: {
			if (reader->current_frame == max_depth) {
				print_loop(reader->thread_trace, *t, &e->loop_occurence, 1);
			} else if (show_structure)
				print_loop(reader->thread_trace, *t, &e->loop_occurence, 0);
			break;
		}
	}
}

/* Print all the events of a thread */
static void print_thread(struct htf_archive* trace, struct htf_thread* thread) {
	printf("Reading events for thread %u (%s):\n", thread->id, htf_get_thread_name(thread));
	printf("Timestamp\tDuration\tTag\tEvent\n");

	struct htf_thread_reader reader;
	htf_read_thread_iterator_init(trace, &reader, thread->id);

	htf_occurence e;
	struct htf_token t;
	while (htf_read_thread_cur_token(&reader, &t, &e) == 0) {
		print_token(&reader, &t, &e);
		htf_move_to_next_token(&reader);
	}
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
	for (int i = 0; i < trace->nb_threads; i++) {
		htf_read_thread_iterator_init(trace, &readers[i], trace->threads[i]->id);
	}

	printf("Timestamp\tDuration\tThread Name\tTag\tEvent\n");

	htf_occurence e;
	struct htf_token t;
	int thread_index = -1;
	while ((thread_index = get_next_token(readers, trace->nb_threads, &t, &e)) >= 0) {
		print_token(&readers[thread_index], &t, &e);
	}
}

static void __compute_durations(struct htf_archive* trace, struct htf_thread* thread) {
	struct htf_thread_reader reader;
	htf_read_thread_iterator_init(trace, &reader, thread->id);
	htf_token_t t;

	htf_timestamp_t* duration_callstack[MAX_CALLSTACK_DEPTH] = {0};
	while (htf_read_thread_cur_token(&reader, &t, NULL) == 0) {
		switch (t.type) {
			case HTF_TYPE_EVENT: {
				htf_timestamp_t duration = reader.thread_trace->events[t.id].timestamps[reader.event_index[t.id]];
				DOFOR(i, reader.current_frame) {
					if (duration_callstack[i])
						*duration_callstack[i] += duration;
				}
				break;
			}
			case HTF_TYPE_SEQUENCE: {
				struct htf_sequence* seq = htf_get_sequence(reader.thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(t));
				duration_callstack[reader.current_frame] = &seq->durations[reader.sequence_index[t.id]];
				break;
			}
			default:
				duration_callstack[reader.current_frame] = NULL;
				break;
		}
		htf_move_to_next_token(&reader);
	}
}

void usage(const char* prog_name) {
	printf("Usage: %s [OPTION] trace_file\n", prog_name);
	printf("\t-T          Print events per thread\n");
	printf("\t-S          Structure mode\n");
	printf("\t--max-depth Next args is the max-depth which shall be printed\n");
	printf("\t-v          Verbose mode\n");
	printf("\t-?  -h      Display this help and exit\n");
}

int main(int argc, char**argv) {
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
		} else if (!strcmp(argv[i], "--max-depth")) {
			max_depth = strtol(argv[i + 1], NULL, 10);
			nb_opts += 2;
			i++;
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

	trace_name = argv[nb_opts + 1];
	if (trace_name == NULL) {
		usage(argv[0]);
		return EXIT_SUCCESS;
	}

	struct htf_archive trace;
	htf_read_archive(&trace, trace_name);

	for (int i = 0; i < trace.nb_threads; i++) {
		__compute_durations(&trace, trace.threads[i]);
	}

	if (per_thread) {
		for (int i = 0; i < trace.nb_threads; i++) {
			printf("\n");
			print_thread(&trace, trace.threads[i]);
		}
	} else {
		print_trace(&trace);
	}

	return EXIT_SUCCESS;
}
