//
// Created by khatharsis on 10/05/23.
//
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "htf.h"
#include "htf_write.h"

#define _init_event(e)                                                      \
	do {                                                                      \
		e->timestamps = malloc(sizeof(htf_timestamp_t) * NB_TIMESTAMP_DEFAULT); \
		e->nb_allocated_events = NB_TIMESTAMP_DEFAULT;                          \
		e->nb_events = 0;                                                       \
	} while (0)
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
	_init_event(es);

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
int MAX_EVENT = 4;
int main(int argc __attribute__((unused)),
	 char** argv  __attribute__((unused))) {
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

	/* Start recording some more events.*/
	for (int eid = 0; eid < MAX_EVENT; eid++)
		init_dummy_event(&thread_writer, eid);

	/* This should have been recognized as a loop, so now there should be some changes. */
	htf_assert(thread_writer.cur_depth == 0);
	htf_assert(thread_writer.og_seq[0]->size == 1);
	htf_assert(thread_writer.og_seq[0]->token[0].type == HTF_TYPE_LOOP);
	/* Check that the loop is correct */
	struct htf_loop* l = htf_get_loop(&thread_writer.thread_trace, HTF_LOOP_ID(thread_writer.og_seq[0]->token[0].id));
	htf_assert(l->nb_iterations[l->nb_loops - 1] == 2);
	// htf_token_t supposed_id = HTF_TOKENIZE(HTF_TYPE_LOOP, 0);
	// htf_assert(memcmp(&l->id, &supposed_id, sizeof(l->id)) == 0);
	/* Check that the sequence inside that loop is correct */
	struct htf_sequence* s = htf_get_sequence(&thread_writer.thread_trace, HTF_SEQUENCE_ID(l->token.id));
	// supposed_id = HTF_TOKENIZE(HTF_TYPE_SEQUENCE, 1);	 // The first sequence is the main one
	// htf_assert(memcmp(&s->id, &supposed_id, sizeof(s->id)) == 0);
	htf_assert(s->size == (unsigned int) MAX_EVENT);

	for (int eid = 0; eid < MAX_EVENT; eid++) {
		htf_assert(s->token[eid].type == HTF_TYPE_EVENT);
		htf_assert(s->token[eid].id == eid);
	}

	/* Start recording even more events.*/
	for (int eid = 0; eid < MAX_EVENT; eid++)
		init_dummy_event(&thread_writer, eid);
	htf_assert(thread_writer.cur_depth == 0);
	htf_assert(thread_writer.og_seq[0]->size == 1);
	htf_assert(l->nb_iterations[l->nb_loops - 1] == 3);
	/* Now start recording one more event and then loop again */
	init_dummy_event(&thread_writer, MAX_EVENT);
	DOFOR(i, 4) {
		for (int eid = 0; eid < MAX_EVENT; eid++)
			init_dummy_event(&thread_writer, eid);
	}
	htf_assert(thread_writer.cur_depth == 0);
	htf_assert(thread_writer.og_seq[0]->size == 3);	 // L0 E L0
	htf_assert(l->nb_iterations[0] == 3);
	htf_assert(l->nb_iterations[1] == 4);

	return 0;
}
