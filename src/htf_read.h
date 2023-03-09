#ifndef HTF_READ_H
#define HTF_READ_H

#include "htf.h"


struct thread_reader {
  struct trace *trace;
  struct thread_trace *thread_trace;

  token_t *callstack_sequence;	/* each entry contains the sequence/loop being read */
  int     *callstack_index;	/* each entry contains the index in the sequence or the loop iteration */
  int     *callstack_loop_iteration;	/* each entry contains the number of iteration of the loop at the corresponding frame */

  int     current_frame;

  int *event_index;
};


void htf_read_trace(struct trace* trace, char* filename);

void htf_read_thread_iterator_init(struct thread_reader *reader,
				   struct trace* trace,
				   int thread_index);

/* return the current event in a thread and move to the next one.
 * Return -1 in case of an error (such as the end of the trace)
 */
int htf_read_thread_next_event(struct thread_reader *reader,
			       struct event_occurence *e);
/* return the current event in a thread.
 * Return -1 in case of an error (such as the end of the trace)
 */
int htf_read_thread_cur_event(struct thread_reader *reader,
			      struct event_occurence *e);

#endif /* HTF_READ_H */
