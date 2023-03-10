#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


#include "htf.h"
#include "htf_write.h"
#include "htf_event.h"

static inline void init_event(struct event *e, enum htf_record record) {
  e->event_size = offsetof(struct event, event_data);
  e->record = record;
}


static inline void push_data(struct event *e, void* data, size_t data_size) {
  size_t o = e->event_size - offsetof(struct event, event_data);
  htf_assert(o < 256);
  htf_assert(o + data_size < 256);
  memcpy(&e->event_data[o], data, data_size);
  e->event_size += data_size;
}

static inline  void pop_data(struct event *e, void* data, size_t data_size, void**cursor) {
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

static inline event_id_t _htf_get_event_id(struct thread_trace *thread_trace,
					   struct event *e) {
  //htf_log(dbg_lvl_max, "Searching for event {.func=%d, .event_type=%d}\n", e->function_id, e->event_type);

  htf_assert(e->event_size < 256);

  for(unsigned i = 0; i < thread_trace->nb_events; i++) {
    if(memcmp(e, &thread_trace->events[i].event, e->event_size) == 0) {
      htf_log(dbg_lvl_max, "\t found with id=%u\n", i);
      return EVENT_ID(i);
    }
  }

  if(thread_trace->nb_events >= thread_trace->nb_allocated_events) {
    htf_error( "too many event data!\n");
  }

  int index = thread_trace->nb_events++;
  htf_log(dbg_lvl_max, "\tNot found. Adding it with id=%x\n", index);
  struct event_summary *es = &thread_trace->events[index];

  memcpy(&es->event, e, e->event_size);
  es->timestamps = malloc(sizeof(timestamp_t)* NB_TIMESTAMP_DEFAULT);
  es->nb_allocated_timestamps = NB_TIMESTAMP_DEFAULT;
  es->nb_timestamps = 0;

#if 0
  printf("New event: ");
  htf_print_event(thread_trace, &es->event);
  printf("\n");
#endif
  return EVENT_ID(index);
}

void htf_print_event(struct thread_trace *thread_trace, struct event* e) {

  switch(e->record) {
  case HTF_EVENT_ENTER:
    {
      void*cursor = NULL;
      int region_ref;
      pop_data(e, &region_ref, sizeof(region_ref), &cursor);
      printf("Enter %d", region_ref);
      break;
    }
  case HTF_EVENT_LEAVE:
    {
      void*cursor = NULL;
      int region_ref;
      pop_data(e, &region_ref, sizeof(region_ref), &cursor);
      printf("Leave %d", region_ref);
      break;
    }
  default:
    printf("{.record: %x, .size:%x}", e->record, e->event_size);
  }
}

void htf_record_enter(struct thread_writer *thread_writer,
		      //		      htf_attribute_list*   attributeList,
		      //                      htf_timeStamp         time,
		      //                      htf_region_ref       region ) {
		      int       region ) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct event e;
  init_event(&e, HTF_EVENT_ENTER);

  push_data(&e, &region, sizeof(region));

  event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_get_timestamp());
  htf_store_event(thread_writer, function_entry, e_id);

  htf_recursion_shield--;
}

void htf_record_leave(struct thread_writer *thread_writer,
		      //		      htf_attribute_list*   attributeList,
		      //                      htf_timeStamp         time,
		      //                      htf_region_ref       region ) {
		      int       region ) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct event e;
  init_event(&e, HTF_EVENT_LEAVE);

  push_data(&e, &region, sizeof(region));

  event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_get_timestamp());
  htf_store_event(thread_writer, function_exit, e_id);

  htf_recursion_shield--;
}
