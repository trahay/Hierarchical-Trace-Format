/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "htf.h"
#include "htf_write.h"

static inline void _init_event_summary(struct htf_event_summary* e) {
  e->durations = malloc(sizeof(htf_timestamp_t) * NB_TIMESTAMP_DEFAULT);
  e->nb_allocated_events = NB_TIMESTAMP_DEFAULT;
  e->nb_events = 0;
}

static inline htf_event_id_t _htf_get_event_id(struct htf_thread* thread_trace, struct htf_event* e) {
  htf_log(htf_dbg_lvl_max, "Searching for event {.event_type=%d}\n", e->record);

  htf_assert(e->event_size < 256);

  for (unsigned i = 0; i < thread_trace->nb_events; i++) {
    if (memcmp(e, &thread_trace->events[i].event, e->event_size) == 0) {
      htf_log(htf_dbg_lvl_max, "\t found with id=%u\n", i);
      return HTF_EVENT_ID(i);
    }
  }

  if (thread_trace->nb_events >= thread_trace->nb_allocated_events) {
    //		htf_error( "too many event data!\n");
    htf_warn("Doubling mem space of events for thread trace %p\n", (void*)thread_trace);
    DOUBLE_MEMORY_SPACE(thread_trace->events, thread_trace->nb_allocated_events, struct htf_event_summary);
  }

  int index = thread_trace->nb_events++;
  htf_log(htf_dbg_lvl_max, "\tNot found. Adding it with id=%x\n", index);
  struct htf_event_summary* es = &thread_trace->events[index];

  memcpy(&es->event, e, e->event_size);
  _init_event_summary(es);

  return HTF_EVENT_ID(index);
}

static void init_dummy_event(struct htf_thread_writer* thread_writer, enum htf_record record) {
  struct htf_event e;
  e.event_size = offsetof(struct htf_event, event_data);
  e.record = record;
  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_get_timestamp());
  htf_store_event(thread_writer, htf_singleton, e_id);
}

int main(int argc, char** argv __attribute__((unused))) {
  if (argc < 2) {
    htf_error("Not enough arguments ! 2 argument required.\n");
  }
  if (argc > 3) {
    htf_error("Too many arguments ! 3 argument required.\n");
  }
  long MAX_EVENT = strtol(argv[1], NULL, 10);
  long NUM_LOOPS = strtol(argv[2], NULL, 10);

  /* Make a dummy archive and a dummy thread writer. */
  struct htf_archive archive;
  htf_write_archive_open(&archive, "dummy_trace", "dummy_trace", 0);
  struct htf_thread_writer thread_writer;
  htf_write_thread_open(&archive, &thread_writer, 0);

  /* Start recording some events.*/
  for (int eid = 0; eid < MAX_EVENT; eid++)
    init_dummy_event(&thread_writer, eid);

  /* Check they've been correctly registered. */
  htf_assert(thread_writer.cur_depth == 0);
  htf_assert(thread_writer.og_seq[0]->size == (unsigned int)MAX_EVENT);
  for (int eid = 0; eid < MAX_EVENT; eid++) {
    htf_assert(thread_writer.og_seq[0]->token[eid].type == HTF_TYPE_EVENT);
    htf_assert(thread_writer.og_seq[0]->token[eid].id == eid);
  }

  /* Start recording some more events. This should make a first loop. */
  for (int eid = 0; eid < MAX_EVENT; eid++)
    init_dummy_event(&thread_writer, eid);

  /* This should have been recognized as a loop, so now there should be some changes. */
  htf_assert(thread_writer.cur_depth == 0);
  htf_assert(thread_writer.og_seq[0]->size == 1);
  htf_assert(thread_writer.og_seq[0]->token[0].type == HTF_TYPE_LOOP);
  /* Check that the loop is correct */
  struct htf_loop* l = htf_get_loop(&thread_writer.thread_trace, HTF_LOOP_ID(thread_writer.og_seq[0]->token[0].id));
  htf_assert(*(int*)htf_vector_get(&l->nb_iterations, l->nb_iterations.size - 1) == 2);
  // htf_token_t supposed_id = HTF_TOKENIZE(HTF_TYPE_LOOP, 0);
  // htf_assert(memcmp(&l->id, &supposed_id, sizeof(l->id)) == 0);
  /* Check that the sequence inside that loop is correct */
  struct htf_sequence* s = htf_get_sequence(&thread_writer.thread_trace, HTF_SEQUENCE_ID(l->token.id));
  // supposed_id = HTF_TOKENIZE(HTF_TYPE_SEQUENCE, 1);	 // The first sequence is the main one
  // htf_assert(memcmp(&s->id, &supposed_id, sizeof(s->id)) == 0);
  htf_assert(s->size == (unsigned int)MAX_EVENT);

  for (int eid = 0; eid < MAX_EVENT; eid++) {
    htf_assert(s->token[eid].type == HTF_TYPE_EVENT);
    htf_assert(s->token[eid].id == eid);
  }

  /* Start recording even more events. The first loop happens 3 times now.*/
  for (int eid = 0; eid < MAX_EVENT; eid++)
    init_dummy_event(&thread_writer, eid);
  htf_assert(thread_writer.cur_depth == 0);
  htf_assert(thread_writer.og_seq[0]->size == 1);
  htf_assert(*(int*)htf_vector_get(&l->nb_iterations, l->nb_iterations.size - 1) == 3);

  /* Now start recording one more event and then loop again. */
  init_dummy_event(&thread_writer, MAX_EVENT);
  DOFOR(loop_number, NUM_LOOPS) {
    DOFOR(eid, MAX_EVENT)
    init_dummy_event(&thread_writer, eid);
  }
  htf_assert(thread_writer.cur_depth == 0);
  htf_assert(thread_writer.og_seq[0]->size == 3);                // L0 E L0
  htf_assert(*(int*)htf_vector_get(&l->nb_iterations, 0) == 3);  // Shouldn't have changed
  htf_assert(*(int*)htf_vector_get(&l->nb_iterations, 1) == NUM_LOOPS);

  return 0;
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
