#ifndef HTF_EVENT_H
#define HTF_EVENT_H

#include "htf.h"
#include "htf_write.h"

///* Initi a container in write mode */
//void htf_write_init(struct htf_trace *trace,
//		    const char* dirname,
//		    const char* trace_name);
//
//void htf_write_init_thread(struct htf_trace *trace,
//			   struct htf_thread_writer *thread_writer,
//			   htf_container_id_t container_id,
//			   htf_string_ref_t   name,
//			   htf_container_id_t parent_id);
//
//void htf_write_define_container(struct htf_trace *trace,
//				struct htf_container *c,
//				htf_container_id_t id,
//				htf_string_ref_t       name,
//				htf_container_id_t parent_id);
//
//void htf_write_finalize_thread(struct htf_thread_writer *thread_writer);
//
//void htf_write_finalize(struct htf_trace *trace);
//

//void htf_register_string(struct htf_container *container,
//			 htf_string_ref_t string_ref,
//			 const char* string);
//
//void htf_register_region(struct htf_container *container,
//			 htf_region_ref_t region_ref,
//			 htf_string_ref_t string_ref);
//

#if 0
/* Useless ? */
struct htf_string* htf_container_name(struct htf_container *container);

struct htf_container * htf_get_container(struct htf_trace *trace,
					 htf_container_id_t id);
#endif


#endif	/* HTF_EVENT_H */
