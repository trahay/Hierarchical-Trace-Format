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

struct htf_event* htf_get_event(struct htf_thread *thread_trace,
				htf_event_id_t evt_id) {
  struct htf_event* evt = NULL;
  if(HTF_ID(evt_id) < thread_trace->nb_events)
    evt = &thread_trace->events[HTF_ID(evt_id)].event;
  return evt;
}


struct htf_sequence* htf_get_sequence(struct htf_thread *thread_trace,
				      htf_sequence_id_t seq_id) {
  struct htf_sequence* seq = NULL;
  if(HTF_ID(seq_id) < thread_trace->nb_sequences)
    seq = &thread_trace->sequences[HTF_ID(seq_id)];
  return seq;
}

struct htf_loop* htf_get_loop(struct htf_thread *thread_trace,
			      htf_loop_id_t loop_id) {
  struct htf_loop* loop = NULL;
  if(HTF_ID(loop_id) < thread_trace->nb_loops)
    loop = &thread_trace->loops[HTF_ID(loop_id)];
  return loop;
}


static htf_token_t _htf_get_token_in_sequence(struct htf_thread *thread_trace,
					      htf_sequence_id_t seq_id,
					      int index) {  
  int seq_index = HTF_ID(seq_id);
  struct htf_sequence *s = htf_get_sequence(thread_trace, seq_id);
  if(!s) {
    htf_error("invalid sequence id: %d\n", seq_index);
  }

  if(index >= s->size) {
    htf_error("invalid index (%d) in sequence #%d\n", index, seq_index);  
  }
  return s->token[index];
}

static htf_token_t _htf_get_token_in_loop(struct htf_thread *thread_trace,
					  htf_loop_id_t loop_id,
					  int index) {
  struct htf_loop *l = htf_get_loop(thread_trace, loop_id);
  if(!l) {
    htf_error("invalid loop id: %d\n", HTF_ID(loop_id));
  }

  if(index >= l->nb_iterations) {
    htf_error("invalid index (%d) in loop #%d\n", index, HTF_ID(loop_id));  
  }
  return l->token;
}


void htf_print_token(struct htf_thread *thread_trace, htf_token_t token) {
  switch(HTF_TOKEN_TYPE(token)) {
  case HTF_TYPE_EVENT:
    {
#define ET2C(et) (((et) == HTF_EVENT_ENTER? 'E':	\
		   (et) == HTF_EVENT_LEAVE? 'L':	\
		   'S'))

      struct htf_event* e =  htf_get_event(thread_trace, HTF_TOKEN_TO_EVENT_ID(token)); 
      printf("E_%d (%c)", HTF_TOKEN_ID(token), ET2C(e->record));//, e->function_id);
      break;
    }
  case HTF_TYPE_SEQUENCE:
    printf("S_%d", HTF_TOKEN_ID(token));
    break;
  case HTF_TYPE_LOOP:
    printf("L_%d", HTF_TOKEN_ID(token));
    break;
  default:
    printf("U_%d_%d", HTF_TOKEN_TYPE(token), HTF_TOKEN_ID(token));
    break;
  }
}

htf_token_t htf_get_token(struct htf_thread *trace,
			  htf_token_t sequence,
			  int index) {
  switch(HTF_TOKEN_TYPE(sequence)){
  case HTF_TYPE_SEQUENCE:
    {
      htf_sequence_id_t seq_id = HTF_TOKEN_TO_SEQUENCE_ID(sequence);
      return _htf_get_token_in_sequence(trace, seq_id, index);
      break;
    }
  case HTF_TYPE_LOOP:
    {
      htf_loop_id_t loop_id = HTF_TOKEN_TO_LOOP_ID(sequence);
      return _htf_get_token_in_loop(trace, loop_id, index);
      break;
    }
  default:
    htf_error("invalid parameters\n");
  }

}

void htf_print_token_array(struct htf_thread *thread_trace,
			   htf_token_t* token_array,
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

void htf_print_sequence(struct htf_thread *thread, htf_sequence_id_t seq_id) {
  struct htf_sequence* seq = htf_get_sequence(thread, seq_id);

  printf("#Sequence %d (%d tokens)-------------\n", HTF_ID(seq_id), seq->size);
  for(int i=0; i<seq->size; i++) {
    htf_print_token(thread, seq->token[i]);
    printf(" ");
  }
  printf("\n");
}

struct htf_thread* htf_archive_get_thread(struct htf_archive* archive,
					  htf_thread_id_t thread_id) {
  for(int i = 0; i<archive->nb_threads; i++) {
    if(archive->threads[i]->id == thread_id)
      return archive->threads[i];
  }
  return NULL;
}

struct htf_container* htf_archive_get_container(struct htf_archive* archive,
						htf_container_id_t container_id) {
  for(int i = 0; i<archive->nb_containers; i++) {
    if(archive->containers[i].id == container_id)
      return &archive->containers[i];
  }

  for(int i = 0; i<archive->global_archive->nb_containers; i++) {
    if(archive->global_archive->containers[i].id == container_id)
      return &archive->global_archive->containers[i];
  }

  return NULL;  
}

const char* htf_get_thread_name(struct htf_thread* thread) {
  struct htf_archive* archive = thread->archive;
  struct htf_container* container = htf_archive_get_container(archive,thread->container);
  return htf_archive_get_string(archive, container->name)->str;
}
