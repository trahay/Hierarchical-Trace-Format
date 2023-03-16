#ifndef HTF_EVENT_H
#define HTF_EVENT_H

#include "htf.h"
#include "htf_write.h"

/* Initi a trace in write mode */
void htf_write_init(struct htf_trace *trace, const char* dirname);
void htf_write_init_thread(struct htf_trace* trace,
			   struct htf_thread_writer *thread_writer,
			   int thread_rank);
void htf_write_finalize(struct htf_trace* trace);


void htf_register_string(struct htf_thread_trace *thread_trace,
			 htf_string_ref_t string_ref,
			 char* string);

void htf_register_region(struct htf_thread_trace *thread_trace,
			 htf_region_ref_t region_ref,
			 htf_string_ref_t string_ref);

void htf_print_event(struct htf_thread_trace *thread_trace,
		     struct htf_event* e);

void htf_record_enter(struct htf_thread_writer *thread_writer,
		      htf_attribute_list_t* attributeList,
		      htf_timestamp_t       time,
		      htf_region_ref_t      region_ref );

void htf_record_leave(struct htf_thread_writer *thread_writer,
		      htf_attribute_list_t* attributeList,
		      htf_timestamp_t       time,
		      htf_region_ref_t      region_ref );

#endif	/* HTF_EVENT_H */
