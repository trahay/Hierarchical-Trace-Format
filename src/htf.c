#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "htf.h"
#include "htf_timestamp.h"

#define NB_TOKEN_DEFAULT 1000000
#define NB_EVENT_DEFAULT 1000
#define NB_SEQUENCE_DEFAULT 1000
#define NB_LOOP_DEFAULT 1000
#define NB_TIMESTAMP_DEFAULT 1000000
#define SEQUENCE_SIZE_DEFAULT 1024
#define CALLSTACK_DEPTH_DEFAULT 128

static _Thread_local int recursion_shield = 0;

static void print_token_array(struct thread_trace *thread_trace, token_t* token_array,
			      int index_start, int index_stop);
static void print_token(struct thread_trace *thread_trace, token_t token);

static inline struct event* _htf_get_event(struct thread_trace *thread_trace,
					   event_id evt_id) {
  struct event* evt = NULL;
  if(evt_id < thread_trace->nb_events)
    evt = &thread_trace->events[evt_id].event;
  return evt;
}

static inline event_id _htf_get_event_id(struct thread_trace *thread_trace,
					 struct event *e) {
  htf_log(dbg_lvl_max, "Searching for event {.func=%d, .event_type=%d}\n", e->function_id, e->event_type);

  for(unsigned i = 0; i < thread_trace->nb_events; i++) {
    if(memcmp(e, &thread_trace->events[i].event, sizeof(struct event)) == 0) {
      htf_log(dbg_lvl_max, "\t found with id=%u\n", i);
      return (event_id) i;
    }
  }

  if(thread_trace->nb_events >= thread_trace->nb_allocated_events) {
    htf_error( "too many event data!\n");
  }

  event_id index = thread_trace->nb_events++;
  htf_log(dbg_lvl_max, "\tNot found. Adding it with id=%u\n", index);
  struct event_summary *es = &thread_trace->events[index];

  memcpy(&es->event, e, sizeof(struct event));
  es->timestamps = malloc(sizeof(timestamp_t)* NB_TIMESTAMP_DEFAULT);
  es->nb_allocated_timestamps = NB_TIMESTAMP_DEFAULT;
  es->nb_timestamps = 0;

  return index;
}

/* return 1 if array1 and array1 are equal */
static inline int _arrays_equal(token_t *array1, int size1,
				token_t* array2, int size2) {
  if(size1 != size2)
    return 0;
  return memcmp(array1, array2, sizeof(token_t)*size1) == 0;
}

/* return 1 if s1 and s2 are equal */
static inline int _sequences_equal(struct sequence *s1,
				   struct sequence *s2) {
  if((!s1) || (!s2))
    return 0;
  return _arrays_equal(s1->token, s1->length, s2->token, s2->length);
}

static inline struct sequence* _htf_get_sequence(struct thread_trace *thread_trace,
						 sequence_id seq_id) {
  struct sequence* seq = NULL;
  if(seq_id < thread_trace->nb_sequences)
    seq = &thread_trace->sequences[seq_id];
  return seq;
}

static inline sequence_id _htf_get_sequence_id_from_array(struct thread_trace *thread_trace,
						       token_t* token_array,
						       int array_len) {
  htf_log(dbg_lvl_debug, "Searching for sequence {.length=%d}\n", array_len);

  for(unsigned i = 0; i < thread_trace->nb_sequences; i++) {
    if(_arrays_equal(token_array,  array_len,
		     thread_trace->sequences[i].token,
		     thread_trace->sequences[i].length)) {
      htf_log(dbg_lvl_debug, "\t found with id=%u\n", i);
      return (sequence_id) i;
    }
  }

  if(thread_trace->nb_sequences >= thread_trace->nb_allocated_sequences) {
    htf_error("too many event data!\n");
  }

  sequence_id index = thread_trace->nb_sequences++;
  htf_log(dbg_lvl_debug, "\tNot found. Adding it with id=%u\n", index);

  struct sequence *s = &thread_trace->sequences[index];
  s->length = array_len;
  s->token = malloc(sizeof(token_t)*s->length);
  memcpy(s->token, token_array, sizeof(token_t)*array_len);

  return index;
}

static inline sequence_id _htf_get_sequence_id(struct thread_trace *thread_trace,
					       struct ongoing_sequence *seq) {
  return _htf_get_sequence_id_from_array(thread_trace, seq->seq.token, seq->seq.length);
}

static inline struct loop* _htf_get_loop(struct thread_trace *thread_trace,
					     loop_id loop_id) {
  struct loop* loop = NULL;
  if(loop_id < thread_trace->nb_loops)
    loop = &thread_trace->loops[loop_id];
  return loop;
}

static inline loop_id _htf_create_loop_id(struct thread_writer *thread_writer,
					   int start_index,
					   int loop_len) {

  if(thread_writer->thread_trace.nb_loops >= thread_writer->thread_trace.nb_allocated_loops) {
    htf_error("too many loops!\n");
  }

  loop_id index = thread_writer->thread_trace.nb_loops++;
  htf_log(dbg_lvl_debug, "\tNot found. Adding it with id=%u\n", index);

  sequence_id sid =  _htf_get_sequence_id_from_array(&thread_writer->thread_trace,
						     &thread_writer->thread_trace.tokens[start_index],
						     loop_len);
  
  struct loop *l = &thread_writer->thread_trace.loops[index];
  l->nb_iterations = 1;
  l->token = TOKENIZE(TYPE_SEQUENCE, sid);

  return index;
}


void _htf_store_timestamp(struct thread_writer *thread_writer,
			  event_id e_id,
			  timestamp_t ts) {
  assert(e_id < thread_writer->thread_trace.nb_allocated_events);

  struct event_summary *es = &thread_writer->thread_trace.events[e_id];
  assert(es);

  if(es->nb_timestamps >= es->nb_allocated_timestamps) {
    htf_warn("Warning: realloc timestamps for event %u\n", e_id);
    es->nb_allocated_timestamps *= 2;
    es->timestamps = realloc(es->timestamps, es->nb_allocated_timestamps * sizeof(timestamp_t));
  }

  es->timestamps[es->nb_timestamps++] = ts;  
}

static void _htf_store_token(struct ongoing_sequence* seq,
			     token_t t) {
  if(seq->seq.length >= seq->nb_allocated_tokens) {
    htf_error( "too many tokens\n");
  }

  htf_log(dbg_lvl_debug, "store_token: (%d, %d)\n", TOKEN_TYPE(t), TOKEN_ID(t));
  seq->seq.token[seq->seq.length++] = t;
}

static void _htf_loop_add_iteration(struct thread_writer *thread_writer,
				    loop_id l,
				    sequence_id sid) {
  struct loop* loop = _htf_get_loop(&thread_writer->thread_trace, l);

  struct sequence *s1 = _htf_get_sequence(&thread_writer->thread_trace, sid);
  struct sequence *s2 = _htf_get_sequence(&thread_writer->thread_trace,
					  TOKEN_ID(loop->token));
  htf_assert(_sequences_equal(s1, s2));
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

  loop_id l =  _htf_create_loop_id(thread_writer,
				   index_first_iteration,
				   loop_len);

  //  x x x e1 e2 e3 e4 e1 e2 e3 e4
  //  x x x l1

  thread_writer->thread_trace.tokens[index_first_iteration] = TOKENIZE(TYPE_LOOP, l);

  sequence_id sid =  _htf_get_sequence_id_from_array(&thread_writer->thread_trace,
						     &thread_writer->thread_trace.tokens[index_second_iteration],
						     loop_len);

  _htf_loop_add_iteration(thread_writer,
			  l,
			  sid);
  thread_writer->thread_trace.nb_tokens = index_first_iteration + 1;
}

static void _htf_find_loop(struct thread_writer *thread_writer) {

  int cur_index = thread_writer->thread_trace.nb_tokens-1;
  int max_len = 10; 		/* TODO: don't hardcode this */

  for(int loop_len=1; loop_len < max_len; loop_len++) {

    /* search for a loop of loop_len tokens */
  
    int s1_start = cur_index+1 - loop_len;
    int s2_start = cur_index+1 - 2*loop_len;

    if(s1_start > 0) {
      int loop_start = s1_start - 1;
      /* first, check if there's a loop that start at loop_start*/
      if(TOKEN_TYPE(thread_writer->thread_trace.tokens[loop_start]) == TYPE_LOOP) {
	loop_id l = TOKEN_ID(thread_writer->thread_trace.tokens[loop_start]);
	struct loop *loop = _htf_get_loop(&thread_writer->thread_trace, l);
	htf_assert(loop);

	struct sequence *seq = _htf_get_sequence(&thread_writer->thread_trace, TOKEN_ID(loop->token));
	htf_assert(seq);
	
	if(_arrays_equal(&thread_writer->thread_trace.tokens[s1_start], loop_len,
			 seq->token,  seq->length)) {

	  /* the current sequence is just another iteration of the loop
	   * remove the sequence, and increment the iteration count
	   */
	  sequence_id sid =  _htf_get_sequence_id_from_array(&thread_writer->thread_trace,
							     &thread_writer->thread_trace.tokens[s1_start],
							     loop_len);
	  _htf_loop_add_iteration(thread_writer, l, sid);
	  thread_writer->thread_trace.nb_tokens = s1_start;
	  cur_index = thread_writer->thread_trace.nb_tokens-1;
	  return;
	}
      }
    }

    if(s2_start >= 0) {
      /* search for a loop of loop_len tokens */

      int is_loop = 1;
      /* search for new loops */
      is_loop = _arrays_equal(&thread_writer->thread_trace.tokens[s1_start], loop_len,
			      &thread_writer->thread_trace.tokens[s2_start], loop_len);      

      if(is_loop) {
	if(htf_debug_level >= dbg_lvl_debug) {
	  printf("Found a loop of len %d:\n", loop_len);
	  print_token_array(&thread_writer->thread_trace, thread_writer->thread_trace.tokens,
			    s1_start, s1_start+loop_len);
	  print_token_array(&thread_writer->thread_trace, thread_writer->thread_trace.tokens,
			    s2_start, s2_start + loop_len);
	  printf("\n");
	}

	_htf_create_loop(thread_writer, loop_len, s1_start, s2_start);
      }    
    }
  }
  
}
			   
static void _htf_store_token_in_main_sequence(struct thread_writer *thread_writer,
					      token_t t) {
  if(thread_writer->thread_trace.nb_tokens >= thread_writer->thread_trace.nb_allocated_tokens) {
    htf_log(dbg_lvl_verbose, "Warning: realloc !\n");
    thread_writer->thread_trace.nb_allocated_tokens *= 2;
    void* ptr=realloc(thread_writer->thread_trace.tokens, sizeof(token_t) * thread_writer->thread_trace.nb_allocated_tokens);
    if(! ptr) {
      htf_error("cannot allocate memory\n");
    }
    thread_writer->thread_trace.tokens = ptr;    
  }

  htf_log(dbg_lvl_debug, "store_token: (%d, %d)\n", TOKEN_TYPE(t), TOKEN_ID(t));
  thread_writer->thread_trace.tokens[thread_writer->thread_trace.nb_tokens++] = t;
  _htf_find_loop(thread_writer);
}

static void _htf_store_event(struct thread_writer *thread_writer,
		      event_id id) {
  token_t t = TOKENIZE(TYPE_EVENT, id);
  _htf_store_token(thread_writer->thread_trace.ongoing_sequence, t);
}

void _htf_record_enter_function(struct thread_writer *thread_writer) {
    struct ongoing_sequence *new_seq = thread_writer->og_seq[thread_writer->cur_depth++];
    if(thread_writer->cur_depth >= thread_writer->max_depth) {
      htf_error("depth = %d >= max_depth (%d) \n", thread_writer->cur_depth, thread_writer->max_depth);
      abort();
    }
    new_seq->seq.length = 0;
    new_seq->enclosing_seq = thread_writer->thread_trace.ongoing_sequence;
    thread_writer->thread_trace.ongoing_sequence = new_seq;
}

void _htf_record_exit_function(struct thread_writer *thread_writer) {
  struct ongoing_sequence *cur_seq = thread_writer->thread_trace.ongoing_sequence;
#if DEBUG
  if(thread_writer->cur_seq != thread_writer->og_seq[thread_writer->cur_depth-1]) {
    htf_error("cur_seq=%p, but og_seq[%d] = %p\n", thread_writer->cur_seq, thread_writer->cur_depth-1, thread_writer->og_seq[thread_writer->cur_depth-1]);
  }
#endif
    
  sequence_id seq_id = _htf_get_sequence_id(&thread_writer->thread_trace, cur_seq);
  struct ongoing_sequence *enclosing_seq = cur_seq->enclosing_seq;

  if(enclosing_seq) {
    _htf_store_token(enclosing_seq, TOKENIZE(TYPE_SEQUENCE, seq_id));
    thread_writer->thread_trace.ongoing_sequence = enclosing_seq;
  } else {
    _htf_store_token_in_main_sequence(thread_writer, TOKENIZE(TYPE_SEQUENCE, seq_id));
    thread_writer->thread_trace.ongoing_sequence = NULL;
  }

  cur_seq->enclosing_seq = NULL;
  cur_seq->seq.length = 0;
  thread_writer->cur_depth--;
}

void htf_record_event(struct thread_writer *thread_writer,
		      enum event_type event_type,
		      int function_id) {
  if(recursion_shield)
    return;
  recursion_shield++;

  struct event e = {.function_id = function_id, .event_type = event_type};
  event_id e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  _htf_store_timestamp(thread_writer, e_id, htf_get_timestamp());

#define HIERARCHICAL_STORAGE 1
#if HIERARCHICAL_STORAGE
  if(event_type == function_entry) {
    _htf_record_enter_function(thread_writer);
  } 

  _htf_store_event(thread_writer, e_id);

  if(event_type == function_exit) {
    _htf_record_exit_function(thread_writer);
  }
#else  /* HIERARCHICAL_STORAGE */
    _htf_store_token_in_main_sequence(thread_writer, e_id);
#endif

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

  assert(thread_rank >= 0);
  assert(thread_rank >= trace->nb_threads);
  thread_writer->thread_rank = thread_rank;

  thread_writer->max_depth = CALLSTACK_DEPTH_DEFAULT;
  thread_writer->og_seq = malloc(sizeof(struct ongoing_sequence*) * thread_writer->max_depth);
  for(int i =0; i<thread_writer->max_depth; i++) {
    thread_writer->og_seq[i] = malloc(sizeof(struct ongoing_sequence));
    thread_writer->og_seq[i]->seq.token = malloc(sizeof(token_t) * SEQUENCE_SIZE_DEFAULT);
    thread_writer->og_seq[i]->seq.length = 0;
    thread_writer->og_seq[i]->nb_allocated_tokens = SEQUENCE_SIZE_DEFAULT;
    thread_writer->og_seq[i]->enclosing_seq = NULL;
  }

  thread_writer->thread_trace.ongoing_sequence = thread_writer->og_seq[0];
  thread_writer->cur_depth++;

  thread_writer->thread_trace.trace = trace;
  thread_writer->thread_trace.tokens = malloc(sizeof(token_t)* NB_TOKEN_DEFAULT);
  thread_writer->thread_trace.nb_allocated_tokens = NB_TOKEN_DEFAULT;
  thread_writer->thread_trace.nb_tokens = 0;

  thread_writer->thread_trace.ongoing_sequence = NULL;
  
  thread_writer->thread_trace.events = malloc(sizeof(struct event_summary) * NB_EVENT_DEFAULT);
  thread_writer->thread_trace.nb_allocated_events = NB_EVENT_DEFAULT;
  thread_writer->thread_trace.nb_events = 0;

  thread_writer->thread_trace.sequences = malloc(sizeof(struct sequence) * NB_SEQUENCE_DEFAULT);
  thread_writer->thread_trace.nb_allocated_sequences = NB_SEQUENCE_DEFAULT;
  thread_writer->thread_trace.nb_sequences = 0;

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


static token_t _get_token_in_main_sequence(struct thread_reader *reader,
					   int index) {  
  if(index >= reader->thread_trace->nb_tokens) {
    htf_error("invalid index (%d) in main sequence\n", index);  
  }
  return reader->thread_trace->tokens[index];
}

static token_t _get_token_in_sequence(struct thread_reader *reader,
				      sequence_id seq_id,
				      int index) {  
  if(seq_id >= reader->thread_trace->nb_sequences) {
    htf_error("invalid sequence id: %d\n", seq_id);
  }
  struct sequence *s = &reader->thread_trace->sequences[seq_id];
  if(index >= s->length) {
    htf_error("invalid index (%d) in sequence #%d\n", index, seq_id);  
  }
  return s->token[index];
}

static token_t _get_token_in_loop(struct thread_reader *reader,
				  loop_id loop_id,
				  int index) {
  if(loop_id >= reader->thread_trace->nb_loops) {
    htf_error("invalid loop id: %d\n", loop_id);
  }
  struct loop *l = &reader->thread_trace->loops[loop_id];
  if(index >= l->nb_iterations) {
    htf_error("invalid index (%d) in loop #%d\n", index, loop_id);  
  }
  return l->token;
}

static void print_token(struct thread_trace *thread_trace, token_t token) {
  switch(TOKEN_TYPE(token)) {
  case TYPE_EVENT:
    {
#define ET2C(et) (((et) == function_entry? 'E':	\
		   (et)==function_exit? 'L':	\
		   'S'))

      struct event* e =  _htf_get_event(thread_trace, TOKEN_ID(token));
      printf("E_%d (%c %d)", TOKEN_ID(token), ET2C(e->event_type), e->function_id);
      break;
    }
  case TYPE_SEQUENCE:
    printf("S_%d", TOKEN_ID(token));
    break;
  case TYPE_LOOP:
    printf("L_%d", TOKEN_ID(token));
    break;
  default:
    printf("U_%d_%d", TOKEN_TYPE(token), TOKEN_ID(token));
    break;
  }
}

static token_t get_token(struct thread_reader *reader,
			 token_t sequence,
			 int index) {
  switch(TOKEN_TYPE(sequence)){
  case TYPE_SEQUENCE:
    {
      sequence_id seq_id = TOKEN_ID(sequence);
      if(sequence == MAIN_SEQUENCE) {
	/* search in the main sequence */
	return _get_token_in_main_sequence(reader, index);
      } else {
	return _get_token_in_sequence(reader, seq_id, index);
      }
      break;
    }
  case TYPE_LOOP:
    {
      loop_id loop_id = TOKEN_ID(sequence);
      return _get_token_in_loop(reader, loop_id, index);
      break;
    }
  default:
    htf_error("invalid parameters\n");
  }

}

static void print_token_array(struct thread_trace *thread_trace, token_t* token_array,
			      int index_start, int index_stop) {
  if(index_start < 0)
    index_start=0;

  for(int i=0; i<index_stop-index_start; i++) {
    print_token(thread_trace, token_array[index_start+i]);
    printf(" ");
  }
  printf("\n");
}

static void print_callstack(struct thread_reader *reader)  __attribute__((unused));
static void print_callstack(struct thread_reader *reader) {
  printf("# Callstack (depth: %d) ------------\n",   reader->callstack_depth);
  for(int i=0; i < reader->callstack_depth; i++) {
    printf("%.*s[%d]", i*2, "                       ", i);
    token_t sequence = reader->callstack_sequence[i];
    int index = reader->callstack_index[i];
    token_t t = get_token(reader, sequence, index);
    print_token(reader->thread_trace, t);
    printf("\n");
  }
  
}

static void print_sequence(struct thread_reader *reader, sequence_id seq_id) __attribute__((unused));
static void print_sequence(struct thread_reader *reader, sequence_id seq_id) {
  struct sequence* seq = _htf_get_sequence(reader->thread_trace, seq_id);

  printf("#Sequence %d (%d tokens)-------------\n", seq_id, seq->length);
  for(int i=0; i<seq->length; i++) {
    print_token(reader->thread_trace, seq->token[i]);
    printf(" ");
  }
  printf("\n");
}

static void print_token_list(struct thread_reader *reader) {
  printf("#Token list (%d tokens)-------------\n", reader->thread_trace->nb_tokens);
  for(int i=0; i<reader->thread_trace->nb_tokens; i++) {
    print_token(reader->thread_trace, reader->thread_trace->tokens[i]);
    printf(" ");
  }
  printf("\n");
}


static void init_callstack(struct thread_reader *reader) {
  reader->callstack_depth = 1;
  reader->callstack_index[0] = 0;
  reader->callstack_loop_iteration[0] = 0;
  reader->callstack_sequence[0] = MAIN_SEQUENCE;
}

void htf_read_thread_iterator_init(struct thread_reader *reader,
				   struct trace* trace,
				   int thread_index) {
  reader->trace = trace;
  assert(thread_index < trace->nb_threads);
  reader->thread_trace = trace->threads[thread_index];

  reader->event_index = calloc(sizeof(int), trace->threads[thread_index]->nb_events);

  /* TODO: don't hardcode the max depth */
  reader->callstack_sequence = calloc(sizeof(token_t), 100);
  reader->callstack_index = calloc(sizeof(int), 100);
  reader->callstack_loop_iteration = calloc(sizeof(int), 100);

  if(htf_debug_level >= dbg_lvl_verbose) {
    htf_log( dbg_lvl_verbose, "init callstack for thread %d\n", thread_index);
    htf_log( dbg_lvl_verbose, "The trace contains:\n");
    print_token_list(reader);
  }

  /* set the cursor on the first event */
  init_callstack(reader);  
}

static token_t get_cur_token(struct thread_reader *reader) {
  int cur_frame = reader->callstack_depth - 1;
  int cur_index = reader->callstack_index[cur_frame];
  token_t cur_seq_id = reader->callstack_sequence[cur_frame];
  return get_token(reader, cur_seq_id, cur_index);
}

static void print_current_event(struct thread_reader *reader) __attribute__((unused));
static void print_current_event(struct thread_reader *reader) {
  token_t t = get_cur_token(reader);
  print_token(reader->thread_trace, t);
}

/* enter a block (push a new frame in the callstack) */
static void enter_block(struct thread_reader *reader, token_t new_block) {
  assert(TOKEN_TYPE(new_block) == TYPE_SEQUENCE ||
	 TOKEN_TYPE(new_block) == TYPE_LOOP);

  int cur_frame = reader->callstack_depth - 1;

  reader->callstack_depth++;  // push frame

  cur_frame = reader->callstack_depth - 1;
  reader->callstack_index[cur_frame] = 0;
  reader->callstack_loop_iteration[cur_frame] = 0;
  reader->callstack_sequence[cur_frame] = new_block;

}

static void leave_block(struct thread_reader *reader) {

  reader->callstack_depth--; // pop frame

  if(reader->callstack_depth <= 0) {
    reader->callstack_depth = -1;
    return;
  }
}

/* return 1 if there are more events in the current sequence */
static int end_of_a_sequence(struct thread_reader *reader,
			int cur_index,
			token_t seq_id) {
  if(seq_id == MAIN_SEQUENCE) {
    return cur_index >= reader->thread_trace->nb_tokens;
  }
  
  if(TOKEN_TYPE(seq_id) == TYPE_SEQUENCE) {
    sequence_id sequence = TOKEN_ID(seq_id);
    struct sequence* s = _htf_get_sequence(reader->thread_trace, sequence);
    return cur_index >= s->length;
  }

  return 1;
}

static void _get_next_event(struct thread_reader *reader) {
  int cur_frame = reader->callstack_depth - 1;
  int cur_index = reader->callstack_index[cur_frame];
  token_t cur_seq_id = reader->callstack_sequence[cur_frame];
  assert(TOKEN_TYPE(cur_seq_id) == TYPE_SEQUENCE ||
	 TOKEN_TYPE(cur_seq_id) == TYPE_LOOP);
  
  while( end_of_a_sequence(reader, cur_index+1, cur_seq_id)) {
    /* we reached the end of the current sequence */

    leave_block(reader);

    /* Did we reach the end of the trace ?  */
    if(reader->callstack_depth <= 0) {
      reader->callstack_depth = -1;
      return;
    }

    /* Is the sequence in a loop ? */
    token_t t = get_cur_token(reader);
    while(TOKEN_TYPE(t) == TYPE_LOOP) {
      /* if we are in the loop, increment the iteration count and
       * check if we reached the end of the loop
       */
      cur_frame = reader->callstack_depth - 1;
      reader->callstack_loop_iteration[cur_frame]++; // increment the iteration count

      loop_id l = TOKEN_ID(t);
      struct loop* loop = _htf_get_loop(reader->thread_trace, l);
      if( reader->callstack_loop_iteration[cur_frame] < loop->nb_iterations) {
	/* there are more iterations in the loop */
	goto enter_sequence;
      }

      /* end of the loop */
      leave_block(reader);
      if(reader->callstack_depth <= 0) {
	reader->callstack_depth = -1;
	return;
      }
      t = get_cur_token(reader);
    }
  }

  /* we are in the middle of a sequence */

  /* just move to the next event in the sequence */
  reader->callstack_index[cur_frame]++;

 enter_sequence:
  /* if it's a loop/sequence, find the first event */
  token_t t = get_cur_token(reader);
  while(TOKEN_TYPE(t) == TYPE_SEQUENCE ||
	TOKEN_TYPE(t) == TYPE_LOOP) {
    enter_block(reader, t);
    t = get_cur_token(reader);
  }    
}

static int _htf_read_thread_next_event(struct thread_reader *reader,
				       struct event_occurence *e,
				       int move) {
  if(reader->callstack_depth < 0)
    return -1;			/* TODO: return EOF */

  /* Get the current event */
  token_t t = get_cur_token(reader);

  while(TOKEN_TYPE(t) == TYPE_SEQUENCE ||
	TOKEN_TYPE(t) == TYPE_LOOP) {
    enter_block(reader, t);
    t = get_cur_token(reader);
  }

  event_id eid = TOKEN_ID(t);
  struct event_summary* es = &reader->thread_trace->events[eid];
  memcpy(&e->event, &es->event, sizeof(e->event));
  e->timestamp = es->timestamps[reader->event_index[eid]];

  if(move) {
    /* Move to the next event */
    reader->event_index[eid]++; // "consume" the event occurence
    _get_next_event(reader);
  }

  return 0;
}

int htf_read_thread_cur_event(struct thread_reader *reader,
			      struct event_occurence *e) {
  return _htf_read_thread_next_event(reader, e, 0);
}

int htf_read_thread_next_event(struct thread_reader *reader,
			       struct event_occurence *e) {

  return _htf_read_thread_next_event(reader, e, 1);
}
