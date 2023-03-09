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

struct event* htf_get_event(struct thread_trace *thread_trace,
			     event_id_t evt_id) {
  struct event* evt = NULL;
  if(ID(evt_id) < thread_trace->nb_events)
    evt = &thread_trace->events[ID(evt_id)].event;
  return evt;
}


struct sequence* htf_get_sequence(struct thread_trace *thread_trace,
				  sequence_id_t seq_id) {
  struct sequence* seq = NULL;
  if(ID(seq_id) < thread_trace->nb_sequences)
    seq = &thread_trace->sequences[ID(seq_id)];
  return seq;
}

struct loop* htf_get_loop(struct thread_trace *thread_trace,
			  loop_id_t loop_id) {
  struct loop* loop = NULL;
  if(ID(loop_id) < thread_trace->nb_loops)
    loop = &thread_trace->loops[ID(loop_id)];
  return loop;
}


static token_t _htf_get_token_in_sequence(struct thread_trace *thread_trace,
					  sequence_id_t seq_id,
					  int index) {  
  int seq_index = ID(seq_id);
  struct sequence *s = htf_get_sequence(thread_trace, seq_id);
  if(!s) {
    htf_error("invalid sequence id: %d\n", seq_index);
  }

  if(index >= s->size) {
    htf_error("invalid index (%d) in sequence #%d\n", index, seq_index);  
  }
  return s->token[index];
}

static token_t _htf_get_token_in_loop(struct thread_trace *thread_trace,
				      loop_id_t loop_id,
				      int index) {
  struct loop *l = htf_get_loop(thread_trace, loop_id);
  if(!l) {
    htf_error("invalid loop id: %d\n", ID(loop_id));
  }

  if(index >= l->nb_iterations) {
    htf_error("invalid index (%d) in loop #%d\n", index, ID(loop_id));  
  }
  return l->token;
}

void htf_print_token(struct thread_trace *thread_trace, token_t token) {
  switch(TOKEN_TYPE(token)) {
  case TYPE_EVENT:
    {
#define ET2C(et) (((et) == function_entry? 'E':	\
		   (et)==function_exit? 'L':	\
		   'S'))

      struct event* e =  htf_get_event(thread_trace, TOKEN_TO_EVENT_ID(token)); 
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

token_t htf_get_token(struct thread_trace *trace,
		      token_t sequence,
		      int index) {
  switch(TOKEN_TYPE(sequence)){
  case TYPE_SEQUENCE:
    {
      sequence_id_t seq_id = TOKEN_TO_SEQUENCE_ID(sequence);
      return _htf_get_token_in_sequence(trace, seq_id, index);
      break;
    }
  case TYPE_LOOP:
    {
      loop_id_t loop_id = TOKEN_TO_LOOP_ID(sequence);
      return _htf_get_token_in_loop(trace, loop_id, index);
      break;
    }
  default:
    htf_error("invalid parameters\n");
  }

}

void htf_print_token_array(struct thread_trace *thread_trace,
		       token_t* token_array,
		       int index_start,
			   int index_stop) {
  if(index_start < 0)
    index_start=0;

  for(int i=0; i<index_stop-index_start; i++) {
    htf_print_token(thread_trace, token_array[index_start+i]);
    printf(" ");
  }
  printf("\n");
}

void htf_print_sequence(struct thread_trace *thread_trace, sequence_id_t seq_id) {
  struct sequence* seq = htf_get_sequence(thread_trace, seq_id);

  printf("#Sequence %d (%d tokens)-------------\n", ID(seq_id), seq->size);
  for(int i=0; i<seq->size; i++) {
    htf_print_token(thread_trace, seq->token[i]);
    printf(" ");
  }
  printf("\n");
}

