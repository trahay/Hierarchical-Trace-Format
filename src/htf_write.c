#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "htf.h"
#include "htf_timestamp.h"
#include "htf_storage.h"
#include "htf_write.h"


#define NB_EVENT_DEFAULT 1000
#define NB_SEQUENCE_DEFAULT 1000
#define NB_LOOP_DEFAULT 1000
#define NB_TIMESTAMP_DEFAULT 1000000
#define SEQUENCE_SIZE_DEFAULT 1024
#define CALLSTACK_DEPTH_DEFAULT 128

static _Thread_local int recursion_shield = 0;

static void _htf_find_loop(struct thread_writer *thread_writer);
static inline sequence_id_t _htf_get_sequence_id_from_array(struct thread_trace *thread_trace,
							    token_t* token_array,
							    int array_len);

static inline struct sequence* _htf_get_cur_sequence(struct thread_writer *thread_writer) {
  struct sequence *seq = thread_writer->og_seq[thread_writer->cur_depth];
  return seq;
}

/* search for a sequence_id that matches seq
 * if none of the registered sequence match, register a new sequence
 */
static inline sequence_id_t _htf_get_sequence_id(struct thread_trace *thread_trace,
						 struct sequence *seq) {
  /* TODO: this could be speed up by storing the sequence_id in the sequence structure.
   * if seq_id == seq_invalid, then search for a matching sequence.
   */
  return _htf_get_sequence_id_from_array(thread_trace, seq->token, seq->size);
}

static inline event_id_t _htf_get_event_id(struct thread_trace *thread_trace,
					   struct event *e) {
  htf_log(dbg_lvl_max, "Searching for event {.func=%d, .event_type=%d}\n", e->function_id, e->event_type);

  for(unsigned i = 0; i < thread_trace->nb_events; i++) {
    if(memcmp(e, &thread_trace->events[i].event, sizeof(struct event)) == 0) {
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

  memcpy(&es->event, e, sizeof(struct event));
  es->timestamps = malloc(sizeof(timestamp_t)* NB_TIMESTAMP_DEFAULT);
  es->nb_allocated_timestamps = NB_TIMESTAMP_DEFAULT;
  es->nb_timestamps = 0;

  return EVENT_ID(index);
}


/* search for a sequence_id that matches token_array
 * if none of the registered sequence match, register a new sequence
 */
static inline sequence_id_t _htf_get_sequence_id_from_array(struct thread_trace *thread_trace,
							    token_t* token_array,
							    int array_len) {
  htf_log(dbg_lvl_debug, "Searching for sequence {.size=%d}\n", array_len);

  for(unsigned i = 1; i < thread_trace->nb_sequences; i++) {
    if(_htf_arrays_equal(token_array,  array_len,
			 thread_trace->sequences[i].token,
			 thread_trace->sequences[i].size)) {
      htf_log(dbg_lvl_debug, "\t found with id=%u\n", i);
      return SEQUENCE_ID(i);
    }
  }

  if(thread_trace->nb_sequences >= thread_trace->nb_allocated_sequences) {
    htf_error("too many event data!\n");
  }

  int index = thread_trace->nb_sequences++;
  sequence_id_t sid = SEQUENCE_ID(index);
  htf_log(dbg_lvl_debug, "\tNot found. Adding it with id=%u\n", index);

  struct sequence *s = htf_get_sequence(thread_trace, sid);
  s->size = array_len;
  s->allocated = array_len;
  s->token = malloc(sizeof(token_t)*s->allocated);
  memcpy(s->token, token_array, sizeof(token_t)*array_len);

  return sid;
}


static inline loop_id_t _htf_create_loop_id(struct thread_writer *thread_writer,
					   int start_index,
					   int loop_len) {

  if(thread_writer->thread_trace.nb_loops >= thread_writer->thread_trace.nb_allocated_loops) {
    htf_error("too many loops!\n");
  }

  int index = thread_writer->thread_trace.nb_loops++;
  htf_log(dbg_lvl_debug, "\tNot found. Adding it with id=%u\n", index);

  struct sequence* cur_seq = _htf_get_cur_sequence(thread_writer);
  sequence_id_t sid =  _htf_get_sequence_id_from_array(&thread_writer->thread_trace,
						       &cur_seq->token[start_index],
						       loop_len);
  
  struct loop *l = &thread_writer->thread_trace.loops[index];
  l->nb_iterations = 1;
  l->token = TOKENIZE(TYPE_SEQUENCE, TOKEN_ID(sid));

  return LOOP_ID(index);
}


void _htf_store_timestamp(struct thread_writer *thread_writer,
			  event_id_t e_id,
			  timestamp_t ts) {
  htf_assert(ID(e_id) < thread_writer->thread_trace.nb_allocated_events);

  struct event_summary *es = &thread_writer->thread_trace.events[ID(e_id)];
  htf_assert(es);

  if(es->nb_timestamps >= es->nb_allocated_timestamps) {
    htf_warn("Warning: realloc timestamps for event %u\n", ID(e_id));
    es->nb_allocated_timestamps *= 2;
    es->timestamps = realloc(es->timestamps, es->nb_allocated_timestamps * sizeof(timestamp_t));
  }

  es->timestamps[es->nb_timestamps++] = ts;  
}

static void _htf_store_token(struct thread_writer *thread_writer,
			     struct sequence* seq,
			     token_t t) {
  if(seq->size >= seq->allocated) {
    htf_error( "too many tokens\n");
  }

  htf_log(dbg_lvl_debug, "store_token: (%x.%x) in %p (size: %d)\n", TOKEN_TYPE(t), TOKEN_ID(t), seq, seq->size+1);
  seq->token[seq->size++] = t;
  _htf_find_loop(thread_writer);
}

static void _htf_loop_add_iteration(struct thread_writer *thread_writer,
				    loop_id_t l,
				    sequence_id_t sid) {
  struct loop* loop = htf_get_loop(&thread_writer->thread_trace, l);

  struct sequence *s1 = htf_get_sequence(&thread_writer->thread_trace, sid);
  struct sequence *s2 = htf_get_sequence(&thread_writer->thread_trace,
					 TOKEN_TO_SEQUENCE_ID(loop->token));
  htf_assert(_htf_sequences_equal(s1, s2));
  loop->nb_iterations++;
}

static void _htf_create_loop(struct thread_writer *thread_writer,
			     int loop_len,
			     int index_first_iteration,
			     int index_second_iteration) {

  if(index_first_iteration >  index_second_iteration) {
    int tmp = index_second_iteration;
    index_second_iteration = index_first_iteration;
    index_first_iteration = tmp;
  }

  loop_id_t l =  _htf_create_loop_id(thread_writer,
				     index_first_iteration,
				     loop_len);

  //  x x x e1 e2 e3 e4 e1 e2 e3 e4
  //  x x x l1

  struct sequence* cur_seq = _htf_get_cur_sequence(thread_writer);
  cur_seq->token[index_first_iteration] = TOKENIZE(TYPE_LOOP, TOKEN_ID(l));

  sequence_id_t sid =  _htf_get_sequence_id_from_array(&thread_writer->thread_trace,
						       &cur_seq->token[index_second_iteration],
						       loop_len);

  _htf_loop_add_iteration(thread_writer,
			  l,
			  sid);
  cur_seq->size = index_first_iteration + 1;
}

static void _htf_find_loop(struct thread_writer *thread_writer) {

  struct sequence* cur_seq = _htf_get_cur_sequence(thread_writer);
  int cur_index = cur_seq->size-1;
  int max_len = 10; 		/* TODO: don't hardcode this */

  if(htf_debug_level >= dbg_lvl_debug) {
    printf("find loops in :\n");
    htf_print_token_array(&thread_writer->thread_trace,
			  cur_seq->token,
			  cur_index-max_len,
			  cur_index);
  }

  for(int loop_len=1; loop_len < max_len; loop_len++) {

    /* search for a loop of loop_len tokens */
    int s1_start = cur_index+1 - loop_len;
    int s2_start = cur_index+1 - 2*loop_len;

    if(s1_start > 0) {
      int loop_start = s1_start - 1;
      /* first, check if there's a loop that start at loop_start*/
      if(TOKEN_TYPE(cur_seq->token[loop_start]) == TYPE_LOOP) {
	loop_id_t l = TOKEN_TO_LOOP_ID(cur_seq->token[loop_start]);
	struct loop *loop = htf_get_loop(&thread_writer->thread_trace, l);
	htf_assert(loop);

	struct sequence *seq = htf_get_sequence(&thread_writer->thread_trace,
						TOKEN_TO_SEQUENCE_ID(loop->token));
	htf_assert(seq);
	
	if(_htf_arrays_equal(&cur_seq->token[s1_start], loop_len, seq->token,  seq->size)) {

	  /* the current sequence is just another iteration of the loop
	   * remove the sequence, and increment the iteration count
	   */
	  sequence_id_t sid =  _htf_get_sequence_id_from_array(&thread_writer->thread_trace,
							       &cur_seq->token[s1_start],
							       loop_len);
	  _htf_loop_add_iteration(thread_writer, l, sid);
	  cur_seq->size = s1_start;
	  cur_index = cur_seq->size-1;
	  return;
	}
      }
    }

    if(s2_start >= 0) {
      /* search for a loop of loop_len tokens */

      int is_loop = 1;
      /* search for new loops */
      is_loop = _htf_arrays_equal(&cur_seq->token[s1_start], loop_len,
			      &cur_seq->token[s2_start], loop_len);      

      if(is_loop) {
	if(htf_debug_level >= dbg_lvl_debug) {
	  printf("Found a loop of len %d:\n", loop_len);
	  htf_print_token_array(&thread_writer->thread_trace, cur_seq->token,
			    s1_start, s1_start+loop_len);
	  htf_print_token_array(&thread_writer->thread_trace, cur_seq->token,
			    s2_start, s2_start + loop_len);
	  printf("\n");
	}

	_htf_create_loop(thread_writer, loop_len, s1_start, s2_start);
      }    
    }
  }
}

static void _htf_store_event(struct thread_writer *thread_writer,
			     event_id_t id) {
  token_t t = TOKENIZE(TYPE_EVENT, TOKEN_ID(id));
  struct sequence *seq =  _htf_get_cur_sequence(thread_writer);
  _htf_store_token(thread_writer, seq, t);
}

void _htf_record_enter_function(struct thread_writer *thread_writer) {
  thread_writer->cur_depth++;
  if(thread_writer->cur_depth >= thread_writer->max_depth) {
    htf_error("depth = %d >= max_depth (%d) \n", thread_writer->cur_depth, thread_writer->max_depth);
    abort();
  }
  struct sequence *seq =  _htf_get_cur_sequence(thread_writer);
  seq->size = 0;
}

void _htf_record_exit_function(struct thread_writer *thread_writer) {
  struct sequence *cur_seq =  _htf_get_cur_sequence(thread_writer);

#if DEBUG
  if(thread_writer->cur_seq != thread_writer->og_seq[thread_writer->cur_depth-1]) {
    htf_error("cur_seq=%p, but og_seq[%d] = %p\n", thread_writer->cur_seq, thread_writer->cur_depth-1, thread_writer->og_seq[thread_writer->cur_depth-1]);
  }
#endif
    
  sequence_id_t seq_id = _htf_get_sequence_id(&thread_writer->thread_trace, cur_seq);

  thread_writer->cur_depth--;
  /* upper_seq is the sequence that called cur_seq */
  struct sequence *upper_seq =  _htf_get_cur_sequence(thread_writer);
  if(!upper_seq) {
    htf_error("upper_seq is NULL!\n");
  }

  _htf_store_token(thread_writer, upper_seq, TOKENIZE(TYPE_SEQUENCE, TOKEN_ID(seq_id)));
  cur_seq->size = 0;
}

void htf_record_event(struct thread_writer *thread_writer,
		      enum event_type event_type,
		      int function_id) {
  if(recursion_shield)
    return;
  recursion_shield++;

  struct event e = {.function_id = function_id, .event_type = event_type};
  event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  _htf_store_timestamp(thread_writer, e_id, htf_get_timestamp());

  if(event_type == function_entry) {
    _htf_record_enter_function(thread_writer);
  } 

  _htf_store_event(thread_writer, e_id);

  if(event_type == function_exit) {
    _htf_record_exit_function(thread_writer);
  }

  recursion_shield--;
}

void htf_write_finalize(struct trace *trace) {
  if(!trace)
    return;

  htf_storage_finalize(trace);
}

void htf_write_init(struct trace *trace, const char* dirname) {
  if(recursion_shield)
    return;
  recursion_shield++;

  trace->threads = NULL;
  trace->nb_threads = 0;
  pthread_mutex_init(&trace->lock, NULL);

  htf_debug_level_init();
  htf_storage_init(dirname);

  recursion_shield--;
}


void htf_write_init_thread(struct trace* trace,
			   struct thread_writer *thread_writer,
			   int thread_rank) {
  if(recursion_shield)
    return;
  recursion_shield++;

  htf_log(dbg_lvl_debug, "htf_write_init_thread\n");

  htf_assert(thread_rank >= 0);
  htf_assert(thread_rank >= trace->nb_threads);
  thread_writer->thread_rank = thread_rank;

  thread_writer->thread_trace.trace = trace;
  
  thread_writer->thread_trace.events = malloc(sizeof(struct event_summary) * NB_EVENT_DEFAULT);
  thread_writer->thread_trace.nb_allocated_events = NB_EVENT_DEFAULT;
  thread_writer->thread_trace.nb_events = 0;

  thread_writer->thread_trace.sequences = malloc(sizeof(struct sequence) * NB_SEQUENCE_DEFAULT);
  thread_writer->thread_trace.nb_allocated_sequences = NB_SEQUENCE_DEFAULT;

  thread_writer->max_depth = CALLSTACK_DEPTH_DEFAULT;
  thread_writer->og_seq = malloc(sizeof(struct sequence*) * thread_writer->max_depth);

#define _init_sequence(s) do {				\
    s->token = malloc(sizeof(token_t) * SEQUENCE_SIZE_DEFAULT); \
    s->size = 0;						\
    s->allocated = SEQUENCE_SIZE_DEFAULT;			\
  } while(0)

  // the main sequence is in sequences[0]
  thread_writer->og_seq[0] = &thread_writer->thread_trace.sequences[0];
  thread_writer->thread_trace.nb_sequences = 1;
  _init_sequence(thread_writer->og_seq[0]);

  for(int i = 1; i<thread_writer->max_depth; i++) {
    thread_writer->og_seq[i] = malloc(sizeof(struct sequence));
    _init_sequence(thread_writer->og_seq[i]);
  }
  thread_writer->cur_depth = 0;

  thread_writer->thread_trace.loops = malloc(sizeof(struct loop) * NB_LOOP_DEFAULT);
  thread_writer->thread_trace.nb_allocated_loops = NB_LOOP_DEFAULT;
  thread_writer->thread_trace.nb_loops = 0;

  pthread_mutex_lock(&trace->lock);
  {
    trace->nb_threads++;
    if(thread_rank > trace->nb_threads)
      trace->nb_threads = thread_rank;
    trace->threads = realloc(trace->threads, sizeof(struct thread_trace *) * trace->nb_threads);
    trace->threads[thread_rank] = &thread_writer->thread_trace;
  }
  pthread_mutex_unlock(&trace->lock);
  recursion_shield--;
}
