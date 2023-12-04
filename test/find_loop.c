/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "htf/htf.h"
#include "htf/htf_write.h"

static inline TokenId _htf_get_event_id(struct Thread* thread_trace, struct Event* e) {
  htf_log(Max, "Searching for event {.event_type=%d}\n", e->record);

  htf_assert(e->event_size < 256);

  for (unsigned i = 0; i < thread_trace->nb_events; i++) {
    if (memcmp(e, &thread_trace->events[i].event, e->event_size) == 0) {
      htf_log(Max, "\t found with id=%u\n", i);
      return i;
    }
  }

  if (thread_trace->nb_events >= thread_trace->nb_allocated_events) {
    //		htf_error( "too many event data!\n");
    htf_warn("Doubling mem space of events for thread trace %p\n", (void*)thread_trace);
    DOUBLE_MEMORY_SPACE(thread_trace->events, thread_trace->nb_allocated_events, struct EventSummary);
  }

  int index = thread_trace->nb_events++;
  htf_log(Max, "\tNot found. Adding it with id=%x\n", index);
  struct EventSummary* es = &thread_trace->events[index];

  memcpy(&es->event, e, e->event_size);

  return index;
}

static void init_dummy_event(struct ThreadWriter* thread_writer, enum Record record) {
  struct Event e;
  e.event_size = offsetof(struct Event, event_data);
  e.record = record;
  TokenId e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_event(thread_writer, HTF_SINGLETON, e_id, htf_get_timestamp(), NULL);
}

int main(int argc, char** argv __attribute__((unused))) {
  if (argc < 2) {
    htf_error("Not enough arguments ! 2 argument required.\n");
  }
  if (argc > 3) {
    htf_error("Too many arguments ! 3 argument required.\n");
  }
  int MAX_EVENT = atoi(argv[1]);
  int NUM_LOOPS = atoi(argv[2]);

  /* Make a dummy archive and a dummy thread writer. */
  struct Archive archive;
  htf_write_archive_open(&archive, "dummy_trace", "dummy_trace", 0);
  struct ThreadWriter thread_writer;
  htf_write_thread_open(&archive, &thread_writer, 0);

  /* Start recording some events.*/
  for (int eid = 0; eid < MAX_EVENT; eid++)
    init_dummy_event(&thread_writer, eid);

  /* Check they've been correctly registered. */
  htf_assert(thread_writer.cur_depth == 0);
  htf_assert(htf_sequence_get_size(thread_writer.og_seq[0]) == (unsigned int)MAX_EVENT);
  for (int eid = 0; eid < MAX_EVENT; eid++) {
    htf_assert(htf_sequence_get_token(thread_writer.og_seq[0], eid).type == TypeEvent);
    htf_assert(htf_sequence_get_token(thread_writer.og_seq[0], eid).id == eid);
  }

  /* Start recording some more events. This should make a first loop. */
  for (int eid = 0; eid < MAX_EVENT; eid++)
    init_dummy_event(&thread_writer, eid);

  /* This should have been recognized as a loop, so now there should be some changes. */
  htf_assert(thread_writer.cur_depth == 0);
  htf_assert(htf_sequence_get_size(thread_writer.og_seq[0]) == 1);
  htf_assert(htf_sequence_get_token(thread_writer.og_seq[0], 0).type == TypeLoop);
  /* Check that the loop is correct */
  struct Loop* l = htf_get_loop(&thread_writer.thread_trace, htf_sequence_get_token(thread_writer.og_seq[0], 0));
  htf_assert(htf_loop_get_count(l, 0) == 2);

  /* Check that the sequence inside that loop is correct */
  struct Sequence* s = htf_get_sequence(&thread_writer.thread_trace, l->repeated_token);

  htf_assert(htf_sequence_get_size(s) == (unsigned int)MAX_EVENT);

  for (int eid = 0; eid < MAX_EVENT; eid++) {
    htf_assert(htf_sequence_get_token(s, eid).type == TypeEvent);
    htf_assert(htf_sequence_get_token(s, eid).id == eid);
  }

  /* Start recording even more events. The first loop happens 3 times now.*/
  for (int eid = 0; eid < MAX_EVENT; eid++)
    init_dummy_event(&thread_writer, eid);
  htf_assert(thread_writer.cur_depth == 0);
  htf_assert(htf_sequence_get_size(thread_writer.og_seq[0]) == 1);
  htf_assert(htf_loop_get_count(l, 0) == 3);

  /* Now start recording one more event and then loop again. */
  init_dummy_event(&thread_writer, MAX_EVENT);
  DOFOR(loop_number, NUM_LOOPS) {
    DOFOR(eid, MAX_EVENT)
    init_dummy_event(&thread_writer, eid);
  }
  htf_assert(thread_writer.cur_depth == 0);
  htf_assert(htf_sequence_get_size(thread_writer.og_seq[0]) == 3);  // L0 E L0
  htf_assert(htf_loop_get_count(l, 0) == 3);
  htf_assert(htf_loop_get_count(l, 1) == (size_t)NUM_LOOPS);

  return 0;
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
