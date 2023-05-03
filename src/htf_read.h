#ifndef HTF_READ_H
#define HTF_READ_H

#include "htf.h"

struct htf_thread_reader {
	struct htf_archive* archive;
	struct htf_thread* thread_trace;

	htf_token_t* callstack_sequence; /* each entry contains the sequence/loop being read */
	int* callstack_index;						 /* each entry contains the index in the sequence or the loop iteration */
	int*
			callstack_loop_iteration; /* each entry contains the number of iteration of the loop at the corresponding frame */

	int current_frame;

	int* event_index;

	/**
	 * Current depth, ie number of sequences + loop we are in. Only used in cosmetics.
	 */
	int depth;
};


void htf_read_global_archive(struct htf_archive* archive, char* main_filename);

void htf_read_archive(struct htf_archive* archive, char* filename);

void htf_read_thread_iterator_init(struct htf_archive *archive,
				   struct htf_thread_reader *reader,
				   htf_thread_id_t thread_id);

/* return the current event in a thread and move to the next one.
 * Return -1 in case of an error (such as the end of the trace)
 */
int htf_read_thread_next_event(struct htf_thread_reader* reader, struct htf_event_occurence* e);
/* return the current event in a thread.
 * Return -1 in case of an error (such as the end of the trace)
 */
int htf_read_thread_cur_event(struct htf_thread_reader* reader, struct htf_event_occurence* e);
/**
 * Copies the value of the current token in a thread to the given pointer.
 * If it is an event, copies the informations to the event_occurence pointer.
 * @return -1 in case of an error (such as the end of the trace), else 0.
 */
int htf_read_thread_cur_token(struct htf_thread_reader* reader, struct htf_token* t, struct htf_event_occurence* e);
/**
 * Copies the value of the current token in a thread to the given pointer, and advances in the trace.
 * If it is an event, copies the informations to the event_occurence pointer.
 * @return -1 in case of an error (such as the end of the trace), else 0.
 */
int htf_read_thread_next_token(struct htf_thread_reader* reader, struct htf_token* t, struct htf_event_occurence* e);
#define MAX_CALLSTACK_DEPTH 100
#endif /* HTF_READ_H */
