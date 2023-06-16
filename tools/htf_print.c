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
	printf("%.9lf", ts / 1e9);
	if (show_structure)
		printf(" ");
	printf("\t%.9lf\t", duration / 1e9);
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
		printf("\t%.6lf * %d", loop->duration / (1e9 * loop->nb_iterations), loop->nb_iterations);
		printf("\t");
	}
	if (!per_thread)
		printf("%s\t", htf_get_thread_name(thread));

	struct htf_loop* l = loop->loop;

	htf_print_token(thread, token);
	printf("\t%d * ", loop->nb_iterations);
	htf_print_token(thread, l->token);
	printf(" = ");
	struct htf_sequence* seq = htf_get_sequence(thread, HTF_TOKEN_TO_SEQUENCE_ID(l->token));
	for (unsigned i = 0; i < seq->size; i++) {
		htf_print_token(thread, seq->token[i]);
		printf(" ");
	}
	printf("\n");
}
static int seq_loop_to_close[MAX_CALLSTACK_DEPTH] = {0};
static void print_token(struct htf_thread* thread, struct htf_token* t, htf_occurence* e, int depth, int last_one) {
	htf_log(htf_dbg_lvl_verbose, "Reading token(%x.%x) for thread %s\n", t->type, t->id, htf_get_thread_name(thread));

	// Prints the structure of the sequences and the loops
	if (show_structure) {
		if (last_one) {
			if (t->type == HTF_TYPE_SEQUENCE || t->type == HTF_TYPE_LOOP) {
				seq_loop_to_close[depth] = 1;
				DOFOR(i, depth - 1) printf("│");
				if (depth)
					printf("├┬");
			} else {
				int nb_last_sequences = 1;
				for (int i = depth - 1; i > 0; i--) {
					if (!seq_loop_to_close[i])
						break;
					seq_loop_to_close[i] = 0;
					nb_last_sequences++;
				}
				DOFOR(i, depth - nb_last_sequences) printf("│");
				DOFOR(i, nb_last_sequences) printf("╰");
			}
		} else {
			if (t->type == HTF_TYPE_SEQUENCE || t->type == HTF_TYPE_LOOP) {
				DOFOR(i, depth - 1) printf("│");
				if (depth)
					printf("├┬");
			} else {
				DOFOR(i, depth) printf("│");
			}
		}
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
			print_event(thread, *t, &e->event_occurence);
			break;
		case HTF_TYPE_SEQUENCE: {
			if (show_structure)
				print_sequence(thread, *t, &e->sequence_occurence);
			break;
		}
		case HTF_TYPE_LOOP: {
			if (depth == max_depth) {
				print_loop(thread, *t, &e->loop_occurence, 1);
			} else if (show_structure)
				print_loop(thread, *t, &e->loop_occurence, 0);
			break;
		}
	}
}

static void free_occurence(htf_token_t token, htf_occurence* occurence) {
	htf_error("This function does not work yet\n");
	// Not too sure what to do with all that tbf
	switch (token.type) {
		case HTF_TYPE_SEQUENCE: {
			struct htf_sequence_occurence seq = occurence->sequence_occurence;
			DOFOR(i, seq.sequence->size) {
				free_occurence(seq.sequence->token[i], &((htf_occurence*)seq.full_sequence)[i]);
			}
			free(seq.full_sequence);
			free(&(occurence->sequence_occurence));
			break;
		}
		case HTF_TYPE_LOOP: {
			struct htf_loop_occurence loop = occurence->loop_occurence;
			DOFOR(i, loop.nb_iterations) {
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
}

static void display_sequence(struct htf_thread_reader* reader,
														 htf_token_t token,
														 struct htf_sequence_occurence* occurence,
														 int depth) {
	if (occurence) {
		load_savestate(reader, &occurence->savestate);
		enter_block(reader, token);
	}
	htf_occurence* current_level = NULL;
	htf_token_t* current_level_token = NULL;
	int size = 0;
	htf_read_thread_cur_level(reader, &current_level, &current_level_token, &size);
	DOFOR(i, size) {
		print_token(reader->thread_trace, &current_level_token[i], &current_level[i], depth, i == (size - 1));
		if (depth < max_depth) {
			if (current_level_token[i].type == HTF_TYPE_SEQUENCE) {
				display_sequence(reader, current_level_token[i], &current_level[i].sequence_occurence, depth + 1);
			}
			if (current_level_token[i].type == HTF_TYPE_LOOP) {
				struct htf_loop_occurence loop = current_level[i].loop_occurence;
				DOFOR(j, loop.nb_iterations) {
					print_token(reader->thread_trace, &loop.loop->token, (htf_occurence*)&loop.full_loop[j], depth + 1,
											j == (loop.nb_iterations - 1));
					display_sequence(reader, loop.loop->token, &loop.full_loop[j], depth + 2);
				}
			}
		}
	}
	if (occurence)
		occurence->full_sequence = current_level;
	else {
		//		DOFOR(i, size) {
		//				free_occurence(current_level_token[i], &current_level[i]);
		//		}
	}
	leave_block(reader);
}

/* Print all the events of a thread */
static void print_thread(struct htf_archive* trace, struct htf_thread* thread) {
	printf("Reading events for thread %u (%s):\n", thread->id, htf_get_thread_name(thread));
	printf("Timestamp\tDuration\tTag\tEvent\n");

	struct htf_thread_reader reader;
	int reader_options = OPTION_NONE;
	if(show_structure)
	  reader_options |= OPTION_SHOW_STRUCTURE;

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
	if(show_structure)
	  reader_options |= OPTION_SHOW_STRUCTURE;

	for (int i = 0; i < trace->nb_threads; i++) {
	  htf_read_thread_iterator_init(trace, &readers[i], trace->threads[i]->id, reader_options);
	}

	printf("Timestamp\tDuration\tThread Name\tTag\tEvent\n");

	htf_occurence e;
	struct htf_token t;
	int thread_index = -1;
	//	while ((thread_index = get_next_token(readers, trace->nb_threads, &t, &e)) >= 0) {
	//		print_token(&readers[thread_index], &t, &e);
	//	}
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
