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

//struct container {
//  htf_container_id_t id;
//  htf_string_ref_t   name;
//  htf_container_id_t parent;
//
//  /* todo: pas besoin ? on peut retrouver ces infos à partir de la
//     liste de conteneurs et leurs parent */
//  htf_container_id_t *children;
//  int nb_children;
//};
//

//struct htf_archive {
//  htf_archive_id_t id;
//  struct htf_string *strings;
//  int nb_strings;
//  int nb_allocated_strings;
//
//  struct htf_regions *regions;
//  int nb_regions;
//  int nb_allocated_regions;
//
//  htf_archive_id_t *sub_archives;
//  int nb_archives;
//  int nb_allocated_archives;
//
//  struct container *containers;
//  int nb_containers;
//  int nb_allocated_containers;
//};

void htf_write_thread_open(struct htf_archive* archive,
			   struct htf_thread_writer* thread_writer,
			   htf_thread_id_t thread_id);

void htf_write_thread_close(struct htf_thread_writer* thread_writer);

void htf_write_define_container(struct htf_archive *archive,
				htf_container_id_t id,
				htf_string_ref_t name,
				htf_container_id_t parent,
				htf_thread_id_t thread);


 
void htf_write_archive_open(struct htf_archive* archive,
			    const char* trace_name,
			    const char* dir_name,
			    htf_archive_id_t archive_id);

void htf_write_add_subarchive(struct htf_archive* archive,
			      htf_archive_id_t subarchive);

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
#define NB_CONTAINERS_DEFAULT 16
#define NB_THREADS_DEFAULT 16

extern _Thread_local int htf_recursion_shield;


#endif /* HTF_WRITE_H */
