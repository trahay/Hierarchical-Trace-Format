#ifndef HTF_EVENT_H
#define HTF_EVENT_H

#include "htf.h"
#include "htf_write.h"

typedef uint32_t string_ref_t;

/* Initialize a trace in write mode */
void htf_write_init(struct trace *trace, const char* dirname);
void htf_write_init_thread(struct trace* trace,
			   struct thread_writer *thread_writer,
			   int thread_rank);
void htf_write_finalize(struct trace* trace);



void htf_print_event(struct thread_trace *thread_trace, struct event* e);

void htf_record_enter(struct thread_writer *thread_writer, int region );
void htf_record_leave(struct thread_writer *thread_writer, int region );

#endif	/* HTF_EVENT_H */
