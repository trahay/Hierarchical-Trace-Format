#ifndef HTF_READ_H
#define HTF_READ_H

#include "htf.h"

struct htf_thread_reader {
	/** Archive being read by this reader. */
	struct htf_archive* archive;
	/** Thread being read. */
	struct htf_thread* thread_trace;

	/** Stack containing the sequences/loops being read. */
	htf_token_t* callstack_sequence;

	/** Stack containing the index in the sequence or the loop iteration. */
	int* callstack_index;

	/** Stack containing the number of iteration of the loop at the corresponding frame */
	int* callstack_loop_iteration;

	/** Current frame = index of the event/loop being read in the callstacks.
	 * You can view this as the "depth" of the callstack. */
	int current_frame;

	/** At any point, an event e has been seen event_index[e.id] times.
	 * Use this to grab the timestamps and other information on the event. */
	int* event_index;
	/** At any point, a sequence s has been seen sequence_index[s.id] times.
	 * Use this to grab the timestamps and other information on the sequence. */
	int* sequence_index;

	/**
	 * Current depth, ie number of sequences + loop we are in. Only used in cosmetics, to print the structure.
	 */
	int depth;
};

void htf_read_global_archive(struct htf_archive* archive, char* main_filename);

void htf_read_archive(struct htf_archive* archive, char* filename);

void htf_read_thread_iterator_init(struct htf_archive *archive,
				   struct htf_thread_reader *reader,
				   htf_thread_id_t thread_id);

/** Copies the value of the current event in a thread to the given pointer, and moves to the next one.
 * @return -1 in case of an error (such as the end of the trace), else 0.
 */
int htf_read_thread_next_event(struct htf_thread_reader* reader, struct htf_event_occurence* e);
/** Copies the value of the current event in a thread to the given pointer.
 * @return -1 in case of an error (such as the end of the trace), else 0.
 */
int htf_read_thread_cur_event(struct htf_thread_reader* reader, struct htf_event_occurence* e);
/**
 * Copies the value of the current token in a thread to the given pointer.
 * If it is an event, copies the informations to the event_occurence pointer.
 * Be aware that exiting a block is not a token, but entering is.
 * @return -1 in case of an error (such as the end of the trace), else 0.
 */
int htf_read_thread_cur_token(struct htf_thread_reader* reader, struct htf_token* t, struct htf_event_occurence* e);
/**
 * Copies the value of the current token in a thread to the given pointer, and moves to the next one.
 * If it is an event, copies the informations to the event_occurence pointer.
 * Be aware that exiting a block is not a token, but entering is.
 * @return -1 in case of an error (such as the end of the trace), else 0.
 */
int htf_read_thread_next_token(struct htf_thread_reader* reader, struct htf_token* t, struct htf_event_occurence* e);
/** The offset is here for when you advanced in the trace. It only happens during the reading phase.
 * @return The timestamps at which the given Sequence or Loop starts, without modifying the reader's state.
 */
htf_timestamp_t htf_get_starting_timestamp(struct htf_thread_reader* reader, struct htf_token token, int offset);
#define MAX_CALLSTACK_DEPTH 100
#endif /* HTF_READ_H */
