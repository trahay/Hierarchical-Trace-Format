#ifndef HTF_WRITE_H
#define HTF_WRITE_H

#include "htf.h"

struct htf_thread_writer {
  struct htf_thread_trace thread_trace;
  struct htf_sequence **og_seq;
  int cur_depth;
  int max_depth;
  int thread_rank;
};



void htf_store_timestamp(struct htf_thread_writer *thread_writer,
			 htf_event_id_t e_id,
			 htf_timestamp_t ts);
void htf_store_event(struct htf_thread_writer *thread_writer,
		     enum htf_event_type event_type,
		     htf_event_id_t id);



#define NB_EVENT_DEFAULT 1000
#define NB_SEQUENCE_DEFAULT 1000
#define NB_LOOP_DEFAULT 1000
#define NB_STRING_DEFAULT 100
#define NB_REGION_DEFAULT 100
#define NB_TIMESTAMP_DEFAULT 1000000
#define SEQUENCE_SIZE_DEFAULT 1024
#define CALLSTACK_DEPTH_DEFAULT 128

extern _Thread_local int htf_recursion_shield;


#endif /* HTF_WRITE_H */
