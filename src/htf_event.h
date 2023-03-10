#ifndef HTF_EVENT_H
#define HTF_EVENT_H

#include "htf.h"
#include "htf_write.h"

/* Initialize a trace in write mode */
void htf_write_init(struct trace *trace, const char* dirname);
void htf_write_init_thread(struct trace* trace,
			   struct thread_writer *thread_writer,
			   int thread_rank);
void htf_write_finalize(struct trace* trace);


void htf_register_string(struct thread_trace *thread_trace,
			 string_ref_t string_ref,
			 char* string);

void htf_register_region(struct thread_trace *thread_trace,
			 region_ref_t region_ref,
			 string_ref_t string_ref);

void htf_print_event(struct thread_trace *thread_trace, struct event* e);

void htf_record_enter(struct thread_writer *thread_writer, region_ref_t region_ref );
void htf_record_leave(struct thread_writer *thread_writer, region_ref_t region_ref );

#endif	/* HTF_EVENT_H */
