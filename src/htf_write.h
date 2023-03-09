#ifndef HTF_WRITE_H
#define HTF_WRITE_H

#include "htf.h"

struct thread_writer {
  struct thread_trace thread_trace;
  struct sequence **og_seq;
  int cur_depth;
  int max_depth;
  int thread_rank;
};

/* Initialize a trace in write mode */
void htf_write_init(struct trace *trace, const char* dirname);
void htf_write_init_thread(struct trace* trace,
			   struct thread_writer *thread_writer,
			   int thread_rank);

void htf_record_event(struct thread_writer* thread_writer,
		      enum event_type event_type,
		      int function_id);
void htf_write_finalize(struct trace* trace);

#endif /* HTF_WRITE_H */
