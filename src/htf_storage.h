#ifndef TRACE_STORAGE_H
#define TRACE_STORAGE_H

void trace_storage_init();

void write_thread_event(struct event_summary *e, int thread_index, int event_id);

void write_thread_sequence(struct sequence *s, int thread_index, int sequence_id);
void write_thread_loop(struct loop *l, int thread_index, int loop_id);
void write_thread_trace(struct trace*trace, int thread_index);

void write_trace(struct trace*trace);

#endif	/* TRACE_STORAGE_H */
