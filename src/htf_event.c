#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


#include "htf.h"
#include "htf_write.h"
#include "htf_event.h"

static inline void init_event(struct htf_event *e, enum htf_record record) {
  e->event_size = offsetof(struct htf_event, event_data);
  e->record = record;
}


static inline void push_data(struct htf_event *e, void* data, size_t data_size) {
  size_t o = e->event_size - offsetof(struct htf_event, event_data);
  htf_assert(o < 256);
  htf_assert(o + data_size < 256);
  memcpy(&e->event_data[o], data, data_size);
  e->event_size += data_size;
}

static inline  void pop_data(struct htf_event *e, void* data, size_t data_size, void**cursor) {
  if(*cursor == NULL) {
    /* initialize the cursor to the begining of event data */
    *cursor = &e->event_data[0];
  }
  
  uintptr_t last_event_byte = ((uintptr_t) e) + e->event_size;
  uintptr_t last_read_byte = ((uintptr_t) *cursor) + data_size;
  htf_assert(last_read_byte <= last_event_byte);

  memcpy(data, *cursor, data_size);
  *cursor += data_size;
}

static inline htf_event_id_t _htf_get_event_id(struct htf_thread *thread_trace,
					   struct htf_event *e) {
  htf_log(htf_dbg_lvl_max, "Searching for event {.event_type=%d}\n", e->record);

  htf_assert(e->event_size < 256);

  for(unsigned i = 0; i < thread_trace->nb_events; i++) {
    if(memcmp(e, &thread_trace->events[i].event, e->event_size) == 0) {
      htf_log(htf_dbg_lvl_max, "\t found with id=%u\n", i);
      return HTF_EVENT_ID(i);
    }
  }

  if(thread_trace->nb_events >= thread_trace->nb_allocated_events) {
    htf_error( "too many event data!\n");
  }

  int index = thread_trace->nb_events++;
  htf_log(htf_dbg_lvl_max, "\tNot found. Adding it with id=%x\n", index);
  struct htf_event_summary *es = &thread_trace->events[index];

  memcpy(&es->event, e, e->event_size);
  es->timestamps = malloc(sizeof(htf_timestamp_t)* NB_TIMESTAMP_DEFAULT);
  es->nb_allocated_timestamps = NB_TIMESTAMP_DEFAULT;
  es->nb_timestamps = 0;

//  if(htf_debug_level >= htf_dbg_lvl_verbose) {
//    htf_log(htf_dbg_lvl_verbose, "New event: ");
//    htf_print_event(thread_trace, &es->event);
//    printf("\n");
//  }
  return HTF_EVENT_ID(index);
}

struct htf_string* htf_archive_get_string(struct htf_archive *archive,
					  htf_string_ref_t string_ref) {
  /* todo: move to htf_archive.c */
  
  /* TODO: race condition here ? when adding a region, there may be a realloc so we may have to hold the mutex */
  for(int i = 0; i< archive->definitions.nb_strings; i++) {
    struct htf_string* s = &archive->definitions.strings[i];
    if(s->string_ref == string_ref) {
      return s;
    }
  }

#if 0
  /* If not found, search in the parent process */
  if(container->parent_id != HTF_CONTAINER_INVALID) {
    struct htf_container *parent = htf_get_container(container->trace, container->parent_id);
    return htf_get_string(parent, string_ref);
  }
#endif
  return NULL;
}

struct htf_region* htf_archive_get_region(struct htf_archive *archive,
					  htf_region_ref_t region_ref) {
  /* TODO: race condition here ? when adding a region, there may be a realloc so we may have to hold the mutex */
  for(int i = 0; i< archive->definitions.nb_regions; i++) {
    struct htf_region* s = &archive->definitions.regions[i];
    if(s->region_ref == region_ref) {
      return s;
    }
  }
#if 0
  /* If not found, search in the parent process */
  if(container->parent_id != HTF_CONTAINER_INVALID) {
    struct htf_container *parent = htf_get_container(container->trace, container->parent_id);
    return htf_get_region(parent, region_ref);
  }
#endif
  return NULL;
}

void htf_print_event(struct htf_thread *t, struct htf_event* e) {

  switch(e->record) {
  case HTF_EVENT_ENTER:
    {
      void*cursor = NULL;
      htf_region_ref_t region_ref;
      pop_data(e, &region_ref, sizeof(region_ref), &cursor);
      struct htf_region* region = htf_archive_get_region(t->archive, region_ref);
      htf_assert(region);
      printf("Enter %d (%s)", region_ref, htf_archive_get_string(t->archive, region->string_ref)->str);
      break;
    }
  case HTF_EVENT_LEAVE:
    {
      void*cursor = NULL;
      htf_region_ref_t region_ref;
      pop_data(e, &region_ref, sizeof(region_ref), &cursor);
      struct htf_region* region = htf_archive_get_region(t->archive, region_ref);
      htf_assert(region);
      printf("Leave %d (%s)", region_ref, htf_archive_get_string(t->archive, region->string_ref)->str);
      break;
    }
  default:
    printf("{.record: %x, .size:%x}", e->record, e->event_size);
  }
}

void htf_archive_register_string_generic(struct htf_definition *d,
				 htf_string_ref_t string_ref,
				 const char* string) {
  if(d->nb_strings + 1 >= d->nb_allocated_strings) {
    d->nb_allocated_strings *= 2;
    d->strings = realloc(d->strings, d->nb_allocated_strings * sizeof(struct htf_string));
    if(d->strings == NULL) {
      htf_error("Failed to allocate memory\n");
    }
  }

  int index = d->nb_strings++;
  struct htf_string *s = &d->strings[index];
  s->string_ref = string_ref;
  s->length = strlen(string)+1;
  s->str = malloc(sizeof(char) * s->length);
  strncpy(s->str, string, s->length);

  htf_log(htf_dbg_lvl_verbose, "Register string #%d{.ref=%x, .length=%d, .str='%s'}\n",
	  index, s->string_ref, s->length, s->str);
}


void htf_archive_register_string(struct htf_archive *archive,
				 htf_string_ref_t string_ref,
				 const char* string) {
  pthread_mutex_lock(&archive->lock);
  htf_archive_register_string_generic(&archive->definitions, string_ref, string);
  pthread_mutex_unlock(&archive->lock);
}

void htf_global_archive_register_string(struct htf_global_archive *archive,
					htf_string_ref_t string_ref,
					const char* string) {
  /* TODO: add a lock */
  //  pthread_mutex_lock(&archive->lock);
  htf_archive_register_string_generic(&archive->definitions, string_ref, string);
  //  pthread_mutex_unlock(&archive->lock);
}

void htf_archive_register_region_generic(struct htf_definition *d,
					 htf_region_ref_t region_ref,
					 htf_string_ref_t string_ref) {
  if(d->nb_regions + 1 >= d->nb_allocated_regions) {
    d->nb_allocated_regions *= 2;
    d->regions = realloc(d->regions, d->nb_allocated_regions * sizeof(struct htf_region));
    if(d->regions == NULL) {
      htf_error("Failed to allocate memory\n");
    }
  }

  int index = d->nb_regions++;
  struct htf_region *r = &d->regions[index];
  r->region_ref = region_ref;
  r->string_ref = string_ref;

  htf_log(htf_dbg_lvl_verbose, "Register region #%d{.ref=%x, .str=%d}\n",
	  index, r->region_ref, r->string_ref);
}


void htf_archive_register_region(struct htf_archive *archive,
				 htf_region_ref_t region_ref,
				 htf_string_ref_t string_ref) {
  pthread_mutex_lock(&archive->lock);
  htf_archive_register_region_generic(&archive->definitions, region_ref, string_ref);
  pthread_mutex_unlock(&archive->lock);
}

void htf_global_archive_register_region(struct htf_global_archive *archive,
					htf_region_ref_t region_ref,
					htf_string_ref_t string_ref) {
  /* TODO: add a lock ?  */
  //  pthread_mutex_lock(&archive->lock); 
  htf_archive_register_region_generic(&archive->definitions, region_ref, string_ref);
  //  pthread_mutex_unlock(&archive->lock);
}

void htf_record_enter(struct htf_thread_writer *thread_writer,
		      htf_attribute_list_t* attributeList __attribute__((unused)),
		      htf_timestamp_t       time,
		      htf_region_ref_t          region_ref ) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  //htf_assert(thread_writer->thread_trace.container);
  //  htf_assert(htf_archive_get_region(thread_writer->thread_trace.archive, region_ref) != NULL);

  struct htf_event e;
  init_event(&e, HTF_EVENT_ENTER);

  push_data(&e, &region_ref, sizeof(region_ref));

  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_function_entry, e_id);

  htf_recursion_shield--;
}

void htf_record_leave(struct htf_thread_writer *thread_writer,
		      htf_attribute_list_t* attributeList __attribute__((unused)),
		      htf_timestamp_t       time,
		      htf_region_ref_t          region_ref ) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_LEAVE);

  push_data(&e, &region_ref, sizeof(region_ref));

  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_function_exit, e_id);

  htf_recursion_shield--;
}
