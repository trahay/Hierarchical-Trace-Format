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

static int verbose=0;
static _Thread_local int recursion_shield = 0;

static inline struct event* _htf_get_event(struct thread_trace *thread_trace,
					   event_id evt_id) {
  struct event* evt = NULL;
  if(evt_id < thread_trace->nb_events)
    evt = &thread_trace->events[evt_id].event;
  return evt;
}

static inline event_id _htf_get_event_id(struct thread_trace *thread_trace,
					 struct event *e) {
  if(verbose)
    printf("Searching for event {.func=%d, .event_type=%d}\n", e->function_id, e->event_type);

  for(unsigned i = 0; i < thread_trace->nb_events; i++) {
    if(memcmp(e, &thread_trace->events[i].event, sizeof(struct event)) == 0) {
      if(verbose)
	printf("\t found with id=%u\n", i);
      return (event_id) i;
    }
  }

  if(thread_trace->nb_events >= thread_trace->nb_allocated_events) {
    fprintf(stderr, "error: too many event data!\n");
    abort();
  }

  event_id index = thread_trace->nb_events++;
  if(verbose)
    printf("\tNot found. Adding it with id=%u\n", index);
  struct event_summary *es = &thread_trace->events[index];

  memcpy(&es->event, e, sizeof(struct event));
  es->timestamps = malloc(sizeof(timestamp_t)* NB_TIMESTAMP_DEFAULT);
  es->nb_allocated_timestamps = NB_TIMESTAMP_DEFAULT;
  es->nb_timestamps = 0;

  return index;
}

/* return 1 if s1 and s2 are equal */
static inline int _sequences_equal(struct sequence *s1,
				   struct sequence *s2) {
  if((!s1) || (!s2))
    return 0;
  if(s1->length != s2->length)
    return 0;
  return memcmp(s1->token, s2->token, sizeof(token_t)*s1->length) == 0;
}

static inline struct sequence* _htf_get_sequence(struct thread_trace *thread_trace,
						 sequence_id seq_id) {
  struct sequence* seq = NULL;
  if(seq_id < thread_trace->nb_sequences)
    seq = &thread_trace->sequences[seq_id];
  return seq;
}

static inline event_id _htf_get_sequence_id(struct thread_trace *thread_trace,
					    struct ongoing_sequence *seq) {
  if(verbose)
    printf("Searching for sequence {.length=%d}\n", seq->seq.length);

  for(unsigned i = 0; i < thread_trace->nb_sequences; i++) {
    if(_sequences_equal(&seq->seq, &thread_trace->sequences[i])) {
      if(verbose)
      	printf("\t found with id=%u\n", i);
      return (sequence_id) i;
    }
  }

  if(thread_trace->nb_sequences >= thread_trace->nb_allocated_sequences) {
    fprintf(stderr, "error: too many event data!\n");
    abort();
  }

  sequence_id index = thread_trace->nb_sequences++;
  if(verbose)
    printf("\tNot found. Adding it with id=%u\n", index);

  struct sequence *s = &thread_trace->sequences[index];
  s->length = seq->seq.length;
  s->token = malloc(sizeof(token_t)*s->length);
  memcpy(s->token, seq->seq.token, sizeof(token_t)*seq->seq.length);

  return index;
}


void _htf_store_timestamp(struct thread_trace *thread_trace,
			  event_id e_id,
			  timestamp_t ts) {
  assert(e_id < thread_trace->nb_allocated_events);

  struct event_summary *es = &thread_trace->events[e_id];
  assert(es);

  if(es->nb_timestamps >= es->nb_allocated_timestamps) {
    fprintf(stderr, "Warning: realloc timestamps for event %u\n", e_id);
    es->nb_allocated_timestamps *= 2;
    es->timestamps = realloc(es->timestamps, es->nb_allocated_timestamps * sizeof(timestamp_t));
  }

  es->timestamps[es->nb_timestamps++] = ts;  
}

#if 0
void _htf_store_token(struct thread_trace *thread_trace,
		      token_t t) {
  if(thread_trace->nb_tokens >= thread_trace->nb_allocated_tokens) {
    fprintf(stderr, "Error: too many tokens\n");
    abort();
  }
  if(verbose)
    printf("E#%d: (%d, %d)\n", thread_trace->nb_tokens, TOKEN_TYPE(t), TOKEN_ID(t));
  thread_trace->tokens[thread_trace->nb_tokens++] = t;
}
#endif

/* TODO: merge with htf_store_token */
void _htf_store_token(struct ongoing_sequence* seq,
		      token_t t) {
  if(seq->seq.length >= seq->nb_allocated_tokens) {
    fprintf(stderr, "Error: too many tokens\n");
    abort();
  }
  if(verbose)
    printf("store_token: (%d, %d)\n", TOKEN_TYPE(t), TOKEN_ID(t));
  //  thread_trace->tokens[thread_trace->nb_tokens++] = t;
  seq->seq.token[seq->seq.length++] = t;
}

void _htf_store_event(struct thread_trace *thread_trace,
		      event_id id) {
  //  assert((2<<30) > event_id);  
  token_t t = TOKENIZE(TYPE_EVENT, id);
  _htf_store_token(thread_trace->ongoing_sequence, t);
  //  _htf_store_token(thread_trace, t);
}

void htf_record_event(struct thread_trace *thread_trace,
		      enum event_type event_type,
		      int function_id) {
  if(recursion_shield)
    return;
  recursion_shield++;

  if(event_type == function_entry || ! thread_trace->ongoing_sequence) {
    /* TODO: don't malloc on the critical path ! */
    struct ongoing_sequence *new_seq = malloc(sizeof(struct ongoing_sequence));
    new_seq->seq.token = malloc(sizeof(token_t) * SEQUENCE_SIZE_DEFAULT);
    new_seq->seq.length = 0;
    new_seq->nb_allocated_tokens = SEQUENCE_SIZE_DEFAULT;
    //    printf("Start a new sequence %p with enclosing %p\n", new_seq, thread_trace->ongoing_sequence);
    new_seq->enclosing_seq = thread_trace->ongoing_sequence;
    thread_trace->ongoing_sequence = new_seq;
  } 

  struct event e = {.function_id = function_id, .event_type = event_type};
  event_id e_id = _htf_get_event_id(thread_trace, &e);

  _htf_store_timestamp(thread_trace, e_id, htf_get_timestamp());
  _htf_store_event(thread_trace, e_id);

  if(event_type == function_exit) {
    struct ongoing_sequence *cur_seq = thread_trace->ongoing_sequence;

    sequence_id seq_id = _htf_get_sequence_id(thread_trace, cur_seq);
    struct ongoing_sequence *enclosing_seq = cur_seq->enclosing_seq;

    //    printf("# End of seq %d (len: %d)\n", seq_id, cur_seq->seq.length);
    if(enclosing_seq) {
      //      printf("\tstore sequence in the enclosing sequence %p (len: %d)\n", enclosing_seq, enclosing_seq->seq.length);
      _htf_store_token(enclosing_seq, TOKENIZE(TYPE_SEQUENCE, seq_id));
      thread_trace->ongoing_sequence = enclosing_seq;

      //      printf("\tnow, ongoing seq=%p\n", thread_trace->ongoing_sequence);

    } else {
      //      printf("\tstore sequence in the main trace at position %d\n", thread_trace->nb_tokens);
      thread_trace->tokens[thread_trace->nb_tokens++] = TOKENIZE(TYPE_SEQUENCE, seq_id);
      thread_trace->ongoing_sequence = NULL;
    }

    free(cur_seq);
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

  char* verbose_str = getenv("VERBOSE");
  if(verbose_str)
    verbose = 1;

  htf_storage_init(dirname);

  recursion_shield--;
}

void htf_write_init_thread(struct trace* trace,
			   struct thread_trace *thread_trace,
			   int thread_rank) {
  if(recursion_shield)
    return;
  recursion_shield++;

  if(verbose)
    printf("htf_write_init_thread\n");

  assert(thread_rank >= 0);
  assert(thread_rank >= trace->nb_threads);

  thread_trace->trace = trace;
  thread_trace->tokens = malloc(sizeof(token_t)* NB_TOKEN_DEFAULT);
  thread_trace->nb_allocated_tokens = NB_TOKEN_DEFAULT;
  thread_trace->nb_tokens = 0;

  thread_trace->ongoing_sequence = NULL;

  thread_trace->events = malloc(sizeof(struct event_summary) * NB_EVENT_DEFAULT);
  thread_trace->nb_allocated_events = NB_EVENT_DEFAULT;
  thread_trace->nb_events = 0;

  thread_trace->sequences = malloc(sizeof(struct sequence) * NB_SEQUENCE_DEFAULT);
  thread_trace->nb_allocated_sequences = NB_SEQUENCE_DEFAULT;
  thread_trace->nb_sequences = 0;

  thread_trace->loops = malloc(sizeof(struct loop) * NB_LOOP_DEFAULT);
  thread_trace->nb_allocated_loops = NB_LOOP_DEFAULT;
  thread_trace->nb_loops = 0;

  pthread_mutex_lock(&trace->lock);
  {
    trace->nb_threads++;
    if(thread_rank > trace->nb_threads)
      trace->nb_threads = thread_rank;
    trace->threads = realloc(trace->threads, sizeof(struct thread_trace *) * trace->nb_threads);
    trace->threads[thread_rank] = thread_trace;
  }
  pthread_mutex_unlock(&trace->lock);
  recursion_shield--;
}


token_t _get_token_in_main_sequence(struct thread_trace_reader *reader,
				    int index) {  
  if(index >= reader->thread_trace->nb_tokens) {
    fprintf(stderr, "invalid index (%d) in main sequence\n", index);
    abort();    
  }
  return reader->thread_trace->tokens[index];
}

token_t _get_token_in_sequence(struct thread_trace_reader *reader,
			       sequence_id seq_id,
			       int index) {  
  if(seq_id >= reader->thread_trace->nb_sequences) {
    fprintf(stderr, "invalid sequence id: %d\n", seq_id);
    abort();
  }
  struct sequence *s = &reader->thread_trace->sequences[seq_id];
  if(index >= s->length) {
    fprintf(stderr, "invalid index (%d) in sequence #%d\n", index, seq_id);
    abort();    
  }
  return s->token[index];
}

token_t _get_token_in_loop(struct thread_trace_reader *reader,
			   loop_id loop_id,
			   int index) {
  if(loop_id >= reader->thread_trace->nb_loops) {
    fprintf(stderr, "invalid loop id: %d\n", loop_id);
    abort();
  }
  struct loop *l = &reader->thread_trace->loops[loop_id];
  if(index >= l->nb_iterations) {
    fprintf(stderr, "invalid index (%d) in loop #%d\n", index, loop_id);
    abort();    
  }
  return l->token;
}

static void print_token(struct thread_trace_reader *reader, token_t token) {
  switch(TOKEN_TYPE(token)) {
  case TYPE_EVENT:
    printf("E_%d", TOKEN_ID(token));
    break;
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

static token_t get_token(struct thread_trace_reader *reader,
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
    fprintf(stderr, "invalid parameters\n");
    abort();
  }

}

static void print_callstack(struct thread_trace_reader *reader) {
  printf("# Callstack (depth: %d) ------------\n",   reader->callstack_depth);
  for(int i=0; i < reader->callstack_depth; i++) {
    printf("%.*s[%d]", i*2, "                       ", i);
    token_t sequence = reader->callstack_sequence[i];
    int index = reader->callstack_index[i];
    token_t t = get_token(reader, sequence, index);
    print_token(reader, t);
    printf("\n");
  }
  
}

static void print_sequence(struct thread_trace_reader *reader, sequence_id seq_id) {
  struct sequence* seq = _htf_get_sequence(reader->thread_trace, seq_id);

  printf("#Sequence %d (%d tokens)-------------\n", seq_id, seq->length);
  for(int i=0; i<seq->length; i++) {
    print_token(reader, seq->token[i]);
    printf(" ");
  }
  printf("\n");
}

static void print_token_list(struct thread_trace_reader *reader) {
  printf("#Token list (%d tokens)-------------\n", reader->thread_trace->nb_tokens);
  for(int i=0; i<reader->thread_trace->nb_tokens; i++) {
    print_token(reader, reader->thread_trace->tokens[i]);
    printf(" ");
  }
  printf("\n");
}


static void init_callstack(struct thread_trace_reader *reader) {
  reader->callstack_depth = 1;
  reader->callstack_index[0] = 0;
  reader->callstack_sequence[0] = MAIN_SEQUENCE;

  // reader->thread_trace->tokens[0];
  //  printf("init callstack\n");
  //  print_token_list(reader);

  //  _get_next_event(reader);
  
#if 0
  
  token_t cur_token = get_token(reader, MAIN_SEQUENCE, 0);
  printf("\ttoken[0] = %x\n", cur_token);
  if(TOKEN_TYPE(cur_token) == TYPE_EVENT)
    goto out;

  while(TOKEN_TYPE(cur_token) == TYPE_SEQUENCE) {
    sequence_id seq_id = TOKEN_ID(cur_token);
    reader->callstack_depth++;
    int cur_frame = reader->callstack_depth - 1;
    reader->callstack_index[cur_frame] = 0;
    reader->callstack_sequence[cur_frame] = cur_token;

    cur_token = get_token(reader, cur_token, 0);
    printf("\ttoken[%d] = %x\n", cur_frame, cur_token);
    print_sequence(reader, seq_id);
  }
#endif

  // out:
  //  print_callstack(reader);
  //  printf("\n\n\n");
}

#if 0
/* move up in the callstack until we find a "next" event
 * return 1 if there's a next event
 * return 0 if we reached the end of trace
 */
static int _frame_up(struct thread_trace_reader *reader) {
  if(reader->callstack_depth<0)
    return 0;
  printf("Frame UP !\n");

  int cur_frame = reader->callstack_depth-1;
  int cur_index = reader->callstack_index[cur_frame];

  reader->callstack_depth--;
  int up_frame = reader->callstack_depth - 1;
  int up_index = reader->callstack_index[up_frame];

  while(up_frame >= 0) {
    // decrease the callstack depth until we find and event or a loop
    // with remaining iterations
    
    token_t up_token = reader->callstack_token[up_frame];
    //    if(up_index

    reader->callstack_depth--;
    int up_frame = reader->callstack_depth - 1;
    int up_index = reader->callstack_index[up_frame];
  }

  int cur_frame = reader->callstack_depth-1;
  int cur_index = reader->callstack_index[cur_frame];
  seq = reader->callstack_token[cur_frame];
  assert(TOKEN_TYPE(seq) == TYPE_SEQUENCE);
  seq_id = TOKEN_ID(seq);
  s = &reader->thread_trace->sequences[seq_id];

}
#endif

void htf_read_thread_iterator_init(struct thread_trace_reader *reader,
				   struct trace* trace,
				   int thread_index) {
  reader->trace = trace;
  assert(thread_index < trace->nb_threads);
  reader->thread_trace = trace->threads[thread_index];
  //  reader->next_event = 0;

  reader->event_index = calloc(sizeof(int), trace->threads[thread_index]->nb_events);

  reader->callstack_sequence = calloc(sizeof(token_t), 100);
  reader->callstack_index = calloc(sizeof(int), 100);

#if 1
  printf("init callstack for thread %d\n", thread_index);
  printf("The trace contains:\n");
  print_token_list(reader);
  printf("--- end of trace\n");
#endif

  /* set the cursor on the first event */
  init_callstack(reader);
  
  //  reader->callstack_depth = 0;
  //  reader->callstack_token[0] = reader->thread_trace->tokens[0];
  
}

static token_t get_cur_token(struct thread_trace_reader *reader) {
  int cur_frame = reader->callstack_depth - 1;
  int cur_index = reader->callstack_index[cur_frame];
  token_t cur_seq_id = reader->callstack_sequence[cur_frame];
  return get_token(reader, cur_seq_id, cur_index);
}

static void print_current_event(struct thread_trace_reader *reader) {
  token_t t = get_cur_token(reader);
  print_token(reader, t);
}

/* enter a block (push a new frame in the callstack) */
static void enter_block(struct thread_trace_reader *reader, token_t new_block) {
  assert(TOKEN_TYPE(new_block) == TYPE_SEQUENCE ||
	 TOKEN_TYPE(new_block) == TYPE_LOOP);

  int cur_frame = reader->callstack_depth - 1;
  //  int cur_index = reader->callstack_index[cur_frame];
  //token_t cur_seq_id = reader->callstack_sequence[cur_frame];

  reader->callstack_depth++;  
  cur_frame = reader->callstack_depth - 1;
  reader->callstack_index[cur_frame] = 0;
  reader->callstack_sequence[cur_frame] = new_block;

#if 0
  printf("Enter frame %d: ", cur_frame);
  print_token(reader, new_block);
  printf("\n");
#endif
}

void leave_block(struct thread_trace_reader *reader) {
  int cur_frame = reader->callstack_depth - 1;
  //  int cur_index = reader->callstack_index[cur_frame];
  token_t cur_seq_id = reader->callstack_sequence[cur_frame];
#if 0
  printf("Leaving frame %d: ", cur_frame);
  print_token(reader, cur_seq_id);
  printf("\n");
#endif
  reader->callstack_depth--; // pop frame

#if 0
  cur_frame = reader->callstack_depth - 1;
  cur_index = reader->callstack_index[cur_frame];
  cur_seq_id = reader->callstack_sequence[cur_frame];
  
  
  token_t t = get_token(reader, cur_seq_id, cur_index);
#endif
}

/* return 1 if there are more events in the current sequence */
static int reached_end_of_sequence(struct thread_trace_reader *reader,
				   int cur_index,
				   token_t seq_id) {
  if(seq_id == MAIN_SEQUENCE) {
    return cur_index >= reader->thread_trace->nb_tokens;
  }
  
  if(TOKEN_TYPE(seq_id) == TYPE_SEQUENCE) {
    sequence_id sequence = TOKEN_ID(seq_id);
    struct sequence* s = _htf_get_sequence(reader->thread_trace, sequence);
#if 0
    if(!s) {
      printf("error with token %x:\n", seq_id);
      print_token(reader, seq_id);
      printf("\n");
    }
#endif
    return cur_index >= s->length;
  }
  return 1;
}

void _get_next_event(struct thread_trace_reader *reader) {
  int cur_frame = reader->callstack_depth - 1;
  int cur_index = reader->callstack_index[cur_frame];
  token_t cur_seq_id = reader->callstack_sequence[cur_frame];
  assert(TOKEN_TYPE(cur_seq_id) == TYPE_SEQUENCE);

  if(! reached_end_of_sequence(reader, cur_index+1, cur_seq_id)) {
    /* we are in the middle of a sequence */
    //    printf("move on to the next event\n");

    /* just move to the next event in the sequence */

#if 0
    /* just a few (debugging) checks to make the trace is OK */
    token_t t = get_token(reader, cur_seq_id, cur_index);
    assert(TOKEN_TYPE(t) == TYPE_EVENT);
    struct event* evt = _htf_get_event(reader->thread_trace, TOKEN_ID(t));
    assert(evt);
    assert(evt->event_type != function_exit);
#endif
    /* OK ! Let's move to the next event */
    reader->callstack_index[cur_frame]++;

    /* if it's a loop/sequence, find the first event */
    token_t t = get_cur_token(reader);
    while(TOKEN_TYPE(t) == TYPE_SEQUENCE ||
	  TOKEN_TYPE(t) == TYPE_LOOP) {
      enter_block(reader, t);
      t = get_cur_token(reader);
    }
    

#if 0
    printf("Next event: ");
    print_current_event(reader);
    printf("\n");
#endif
    return;
  }
  //  printf("End of a sequence\n");

  if( cur_seq_id  == MAIN_SEQUENCE ) {
    /* we reached the end of trace */
    reader->callstack_depth = -1;
    printf("End of trace !\n");
    return;
  }
  /* we reached the end of the current sequence */
  leave_block(reader);
  _get_next_event(reader);
}

int _htf_read_thread_next_event(struct thread_trace_reader *reader,
				struct event_occurence *e,
				int move) {
  if(reader->callstack_depth < 0)
    return -1;			/* TODO: return EOF */

  /* Get the current event */
  //  print_callstack(reader);
  int cur_frame = reader->callstack_depth - 1;
  int cur_index = reader->callstack_index[cur_frame];
  token_t cur_seq_id = reader->callstack_sequence[cur_frame];
  assert(TOKEN_TYPE(cur_seq_id) == TYPE_SEQUENCE);

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

#if 0
  printf("About to return: %x: ", t);
  print_token(reader, t);
  printf("\n");
#endif

  if(move) {
    /* Move to the next event */
#if 0
    printf("Move to next event !\n");
    printf("\tCurrent event: ");
    print_token(reader, t);
    printf("\n");
#endif
    reader->event_index[eid]++; // "consume" the event occurence

    _get_next_event(reader);
  }

  return 0;
}

int htf_read_thread_cur_event(struct thread_trace_reader *reader,
			      struct event_occurence *e) {
  return _htf_read_thread_next_event(reader, e, 0);
}

int htf_read_thread_next_event(struct thread_trace_reader *reader,
			       struct event_occurence *e) {

  return _htf_read_thread_next_event(reader, e, 1);
}
