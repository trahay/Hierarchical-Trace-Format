#ifndef HTF_WRITE_H
#define HTF_WRITE_H

#include "htf.h"

struct htf_thread_writer {
  struct htf_thread thread_trace;
  struct htf_sequence **og_seq;
  int cur_depth;
  int max_depth;
  int thread_rank;
};


void htf_write_global_archive_open(struct htf_archive* archive,
				   const char* dir_name,
				   const char* trace_name);
void htf_write_global_archive_close(struct htf_archive* archive);


void htf_write_global_define_location_group(struct htf_archive *archive,
					    htf_location_group_id_t id,
					    htf_string_ref_t name,
					    htf_location_group_id_t parent);

void htf_write_global_define_location(struct htf_archive *archive,
				      htf_thread_id_t id,
				      htf_string_ref_t name,
				      htf_location_group_id_t parent);



void htf_write_thread_open(struct htf_archive* archive,
			   struct htf_thread_writer* thread_writer,
			   htf_thread_id_t thread_id);

void htf_write_thread_close(struct htf_thread_writer* thread_writer);

void htf_write_define_location_group(struct htf_archive *archive,
				     htf_location_group_id_t id,
				     htf_string_ref_t name,
				     htf_location_group_id_t parent);

void htf_write_define_location(struct htf_archive *archive,
			       htf_thread_id_t id,
			       htf_string_ref_t name,
			       htf_location_group_id_t parent);


void htf_write_archive_open(struct htf_archive*     archive,
			    const char*             dir_name,
			    const char*             trace_name,
			    htf_location_group_id_t location_group);

void htf_write_archive_close(struct htf_archive* archive);




void htf_store_timestamp(struct htf_thread_writer *thread_writer,
			 htf_event_id_t e_id,
			 htf_timestamp_t ts);
void htf_store_event(struct htf_thread_writer *thread_writer,
		     enum htf_event_type event_type,
		     htf_event_id_t id);



void htf_record_enter(struct htf_thread_writer *thread_writer,
		      htf_attribute_list_t* attributeList,
		      htf_timestamp_t       time,
		      htf_region_ref_t      region_ref );

void htf_record_leave(struct htf_thread_writer *thread_writer,
		      htf_attribute_list_t* attributeList,
		      htf_timestamp_t       time,
		      htf_region_ref_t      region_ref );

#define NB_EVENT_DEFAULT 1000
#define NB_SEQUENCE_DEFAULT 1000
#define NB_LOOP_DEFAULT 1000
#define NB_STRING_DEFAULT 100
#define NB_REGION_DEFAULT 100
#define NB_TIMESTAMP_DEFAULT 1000000
#define SEQUENCE_SIZE_DEFAULT 1024
#define CALLSTACK_DEPTH_DEFAULT 128
#define NB_ARCHIVES_DEFAULT 1
#define NB_THREADS_DEFAULT 16
#define NB_LOCATION_GROUPS_DEFAULT 16
#define NB_LOCATIONS_DEFAULT NB_THREADS_DEFAULT

extern _Thread_local int htf_recursion_shield;


#endif /* HTF_WRITE_H */
