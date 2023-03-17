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

static inline htf_event_id_t _htf_get_event_id(struct htf_thread_trace *thread_trace,
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

  if(htf_debug_level >= htf_dbg_lvl_verbose) {
    htf_log(htf_dbg_lvl_verbose, "New event: ");
    htf_print_event(thread_trace, &es->event);
    printf("\n");
  }
  return HTF_EVENT_ID(index);
}

struct htf_string* htf_get_string(struct htf_thread_trace *thread_trace,
				  htf_string_ref_t string_ref) {
  /* TODO: race condition here ? when adding a region, there may be a realloc so we may have to hold the mutex */
  for(int i = 0; i< thread_trace->nb_strings; i++) {
    struct htf_string* s = &thread_trace->strings[i];
    if(s->string_ref == string_ref) {
      return s;
    }
  }
  return NULL;
}

struct htf_region* htf_get_region(struct htf_thread_trace *thread_trace,
				  htf_region_ref_t region_ref) {
  /* TODO: race condition here ? when adding a region, there may be a realloc so we may have to hold the mutex */
  for(int i = 0; i< thread_trace->nb_regions; i++) {
    struct htf_region* s = &thread_trace->regions[i];
    if(s->region_ref == region_ref) {
      return s;
    }
  }
  return NULL;
}

struct htf_string* htf_get_global_string(struct htf_trace *trace,
					 htf_string_ref_t string_ref) {
  /* TODO: race condition here ? when adding a region, there may be a realloc so we may have to hold the mutex */
  for(int i = 0; i< trace->nb_strings; i++) {
    struct htf_string* s = &trace->strings[i];
    if(s->string_ref == string_ref) {
      return s;
    }
  }
  return NULL;
}

struct htf_region* htf_get_global_region(struct htf_trace *trace,
					 htf_region_ref_t region_ref) {
  /* TODO: race condition here ? when adding a region, there may be a realloc so we may have to hold the mutex */
  for(int i = 0; i< trace->nb_regions; i++) {
    struct htf_region* s = &trace->regions[i];
    if(s->region_ref == region_ref) {
      return s;
    }
  }
  return NULL;
}

void htf_print_event(struct htf_thread_trace *thread_trace, struct htf_event* e) {

  switch(e->record) {
  case HTF_EVENT_ENTER:
    {
      void*cursor = NULL;
      htf_region_ref_t region_ref;
      pop_data(e, &region_ref, sizeof(region_ref), &cursor);
      struct htf_region* region = htf_get_region(thread_trace, region_ref);
      htf_assert(region);
      printf("Enter %d (%s)", region_ref, htf_get_string(thread_trace, region->string_ref)->str);
      break;
    }
  case HTF_EVENT_LEAVE:
    {
      void*cursor = NULL;
      htf_region_ref_t region_ref;
      pop_data(e, &region_ref, sizeof(region_ref), &cursor);
      struct htf_region* region = htf_get_region(thread_trace, region_ref);
      htf_assert(region);
      printf("Leave %d (%s)", region_ref, htf_get_string(thread_trace, region->string_ref)->str);
      break;
    }
  default:
    printf("{.record: %x, .size:%x}", e->record, e->event_size);
  }
}


void htf_register_string(struct htf_thread_trace *thread_trace,
			 htf_string_ref_t string_ref,
			 const char* string) {
  pthread_mutex_lock(&thread_trace->strings_lock);
  if(thread_trace->nb_strings + 1 >= thread_trace->nb_allocated_strings) {
    thread_trace->nb_allocated_strings *= 2;
    thread_trace->strings = realloc(thread_trace->strings, thread_trace->nb_allocated_strings * sizeof(struct htf_string));
    if(thread_trace->strings == NULL) {
      htf_error("Failed to allocate memory\n");
    }
  }

  int index = thread_trace->nb_strings++;
  struct htf_string *s = &thread_trace->strings[index];
  s->string_ref = string_ref;
  s->length = strlen(string)+1;
  s->str = malloc(sizeof(char) * s->length);
  strncpy(s->str, string, s->length);

  htf_log(htf_dbg_lvl_verbose, "Register string #%d{.ref=%x, .length=%d, .str='%s'}\n",
	  index, s->string_ref, s->length, s->str);
  pthread_mutex_unlock(&thread_trace->strings_lock);
}


void htf_register_global_string(struct htf_trace *trace,
				htf_string_ref_t string_ref,
				const char* string) {
  pthread_mutex_lock(&trace->lock);
  if(trace->nb_strings + 1 >= trace->nb_allocated_strings) {
    trace->nb_allocated_strings *= 2;
    trace->strings = realloc(trace->strings, trace->nb_allocated_strings * sizeof(struct htf_string));
    if(trace->strings == NULL) {
      htf_error("Failed to allocate memory\n");
    }
  }

  int index = trace->nb_strings++;
  struct htf_string *s = &trace->strings[index];
  s->string_ref = string_ref;
  s->length = strlen(string)+1;
  s->str = malloc(sizeof(char) * s->length);
  strncpy(s->str, string, s->length);

  for(int i =0; i<trace->nb_threads; i++) {
    htf_register_string(trace->threads[i], string_ref, string);
  }

  htf_log(htf_dbg_lvl_verbose, "Register global string #%d{.ref=%x, .length=%d, .str='%s'}\n",
	  index, s->string_ref, s->length, s->str);
  pthread_mutex_unlock(&trace->lock);
}

void htf_register_region(struct htf_thread_trace *thread_trace,
			 htf_region_ref_t region_ref,
			 htf_string_ref_t string_ref) {

  pthread_mutex_lock(&thread_trace->regions_lock);
  if(thread_trace->nb_regions + 1 >= thread_trace->nb_allocated_regions) {
    thread_trace->nb_allocated_regions *= 2;
    thread_trace->regions = realloc(thread_trace->regions, thread_trace->nb_allocated_regions * sizeof(struct htf_region));
    if(thread_trace->regions == NULL) {
      htf_error("Failed to allocate memory\n");
    }
  }

  int index = thread_trace->nb_regions++;
  struct htf_region *r = &thread_trace->regions[index];
  r->region_ref = region_ref;
  r->string_ref = string_ref;

  htf_log(htf_dbg_lvl_verbose, "Register region #%d{.ref=%x, .str=%d ('%s')}\n",
	  index, r->region_ref, r->string_ref, htf_get_string(thread_trace, r->string_ref)->str);
  pthread_mutex_unlock(&thread_trace->regions_lock);
}

void htf_register_global_region(struct htf_trace *trace,
				htf_region_ref_t region_ref,
				htf_string_ref_t string_ref) {
  pthread_mutex_lock(&trace->lock);
  if(trace->nb_regions + 1 >= trace->nb_allocated_regions) {
    trace->nb_allocated_regions *= 2;
    trace->regions = realloc(trace->regions, trace->nb_allocated_regions * sizeof(struct htf_region));
    if(trace->regions == NULL) {
      htf_error("Failed to allocate memory\n");
    }
  }

  int index = trace->nb_regions++;
  struct htf_region *r = &trace->regions[index];
  r->region_ref = region_ref;
  r->string_ref = string_ref;

  for(int i =0; i<trace->nb_threads; i++) {
    htf_register_region(trace->threads[i], region_ref, string_ref);
  }

  htf_log(htf_dbg_lvl_verbose, "Register Global region #%d{.ref=%x, .str=%d ('%s')}\n",
	  index, r->region_ref, r->string_ref, htf_get_global_string(trace, r->string_ref)->str);
  pthread_mutex_unlock(&trace->lock);
}

void htf_record_enter(struct htf_thread_writer *thread_writer,
		      htf_attribute_list_t* attributeList,
		      htf_timestamp_t       time,
		      htf_region_ref_t          region_ref ) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_ENTER);

  push_data(&e, &region_ref, sizeof(region_ref));

  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_function_entry, e_id);

  htf_recursion_shield--;
}

void htf_record_leave(struct htf_thread_writer *thread_writer,
		      htf_attribute_list_t* attributeList,
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
