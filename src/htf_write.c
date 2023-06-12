#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <inttypes.h>


#include "htf.h"
#include "htf_timestamp.h"
#include "htf_storage.h"
#include "htf_archive.h"
#include "htf_write.h"



_Thread_local int htf_recursion_shield = 0;

static void _htf_find_loop(struct htf_thread_writer *thread_writer);
static inline htf_sequence_id_t _htf_get_sequence_id_from_array(struct htf_thread *thread_trace,
																																htf_token_t* token_array,
																																int array_len);

#define _init_sequence(s)                                          \
	do {                                                             \
		s->token = calloc(sizeof(htf_token_t), SEQUENCE_SIZE_DEFAULT); \
		s->size = 0;                                                   \
		s->allocated = SEQUENCE_SIZE_DEFAULT;                          \
		array_new(&s->timestamps, sizeof(htf_timestamp_t));            \
	} while (0)

#define _init_loop(l)                                               \
	do {                                                              \
		l->nb_iterations = calloc(LOOP_SIZE_DEFAULT, sizeof(unsigned)); \
		l->nb_loops = 0;                                                \
		l->nb_allocated = LOOP_SIZE_DEFAULT;                            \
	} while (0)

#define _init_event(e)                                                      \
	do {                                                                      \
		e->timestamps = malloc(sizeof(htf_timestamp_t) * NB_TIMESTAMP_DEFAULT); \
		e->nb_allocated_timestamps = NB_TIMESTAMP_DEFAULT;                      \
		e->nb_timestamps = 0;                                                   \
	} while (0)

static inline struct htf_sequence* _htf_get_cur_sequence(struct htf_thread_writer* thread_writer) {
	struct htf_sequence* seq = thread_writer->og_seq[thread_writer->cur_depth];
	return seq;
}

/* search for a sequence_id that matches seq
 * if none of the registered sequence match, register a new sequence
 */
static inline htf_sequence_id_t _htf_get_sequence_id(struct htf_thread *thread_trace,
						     struct htf_sequence *seq) {
  /* TODO: this could be speed up by storing the sequence_id in the sequence structure.
   * if seq_id == seq_invalid, then search for a matching sequence.
   */
  return _htf_get_sequence_id_from_array(thread_trace, seq->token, seq->size);
}


/* search for a sequence_id that matches token_array
 * if none of the registered sequence match, register a new sequence
 */
static inline htf_sequence_id_t _htf_get_sequence_id_from_array(struct htf_thread *thread_trace,
								htf_token_t* token_array,
								int array_len) {
  htf_log(htf_dbg_lvl_debug, "Searching for sequence {.size=%d}\n", array_len);

  for(unsigned i = 1; i < thread_trace->nb_sequences; i++) {
		if (_htf_arrays_equal(token_array, array_len, thread_trace->sequences[i]->token,
													thread_trace->sequences[i]->size)) {
			htf_log(htf_dbg_lvl_debug, "\t found with id=%u\n", i);
			return HTF_SEQUENCE_ID(i);
		}
	}

	if (thread_trace->nb_sequences >= thread_trace->nb_allocated_sequences) {
		htf_warn("Doubling mem space of sequence for thread trace %p\n", thread_trace);
		DOUBLE_MEMORY_SPACE(thread_trace->sequences, thread_trace->nb_allocated_sequences, struct htf_sequence*);
		for (int i = thread_trace->nb_allocated_sequences / 2; i < thread_trace->nb_allocated_sequences; i++) {
			thread_trace->sequences[i] = malloc(sizeof(struct htf_sequence));
			_init_sequence(thread_trace->sequences[i]);
		}
	}

	int index = thread_trace->nb_sequences++;
	htf_sequence_id_t sid = HTF_SEQUENCE_ID(index);
	htf_log(htf_dbg_lvl_debug, "\tSequence not found. Adding it with id=%u\n", index);

	struct htf_sequence* s = htf_get_sequence(thread_trace, sid);
	s->size = array_len;
	s->allocated = array_len;
	s->token = malloc(sizeof(htf_token_t) * s->allocated);
	memcpy(s->token, token_array, sizeof(htf_token_t) * array_len);

	return sid;
}

static inline struct htf_loop* _htf_create_loop_id(struct htf_thread_writer* thread_writer,
																									 int start_index,
																									 int loop_len) {
	if (thread_writer->thread_trace.nb_loops >= thread_writer->thread_trace.nb_allocated_loops) {
		htf_warn("Doubling mem space of loops for thread writer %p's thread trace, cur=%d\n", thread_writer,
						 thread_writer->thread_trace.nb_allocated_loops);
		DOUBLE_MEMORY_SPACE(thread_writer->thread_trace.loops, thread_writer->thread_trace.nb_allocated_loops,
												struct htf_loop);
		for (int i = thread_writer->thread_trace.nb_allocated_loops / 2; i < thread_writer->thread_trace.nb_allocated_loops;
				 i++) {
			_init_loop((&thread_writer->thread_trace.loops[i]));
		}
	}

	struct htf_sequence* cur_seq = _htf_get_cur_sequence(thread_writer);
	htf_sequence_id_t sid =
			_htf_get_sequence_id_from_array(&thread_writer->thread_trace, &cur_seq->token[start_index], loop_len);

	int index = -1;
	DOFOR(i, thread_writer->thread_trace.nb_loops) {
		if (thread_writer->thread_trace.loops[i].token.id == sid.id) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		index = thread_writer->thread_trace.nb_loops++;
	}
	htf_log(htf_dbg_lvl_debug, "\tLoop not found. Adding it with id=%u\n", index);
	struct htf_loop* l = &thread_writer->thread_trace.loops[index];
	l->nb_iterations[l->nb_loops++] = 1;
	l->token = HTF_TOKENIZE(HTF_TYPE_SEQUENCE, HTF_TOKEN_ID(sid));
	l->id = HTF_TOKENIZE(HTF_TYPE_LOOP, index);
	return l;
}

void htf_store_timestamp(struct htf_thread_writer *thread_writer,
			 htf_event_id_t e_id,
			 htf_timestamp_t ts) {
	htf_assert(HTF_ID(e_id) < thread_writer->thread_trace.nb_allocated_events);

	struct htf_event_summary* es = &thread_writer->thread_trace.events[HTF_ID(e_id)];
	htf_assert(es);

	if (es->nb_timestamps >= es->nb_allocated_timestamps) {
		htf_warn("Doubling mem space of timestamps for event %u\n", HTF_ID(e_id));
		DOUBLE_MEMORY_SPACE(es->timestamps, es->nb_allocated_timestamps, htf_timestamp_t);
	}

	es->timestamps[es->nb_timestamps] = ts;
	htf_delta_timestamp(&es->timestamps[es->nb_timestamps]);
	es->nb_timestamps++;
}

static void _htf_store_token(struct htf_thread_writer *thread_writer,
			     struct htf_sequence* seq,
			     htf_token_t t) {
  while(seq->size >= seq->allocated) {
//    htf_error( "too many tokens\n");
	  htf_warn("Doubling mem space of tokens for sequence %p, cur=%u, size=%u\n", seq, seq->allocated, seq->size);
	  DOUBLE_MEMORY_SPACE(seq->token, seq->allocated, htf_token_t);
  }

  htf_log(htf_dbg_lvl_debug, "store_token: (%c%x) in %p (size: %u)\n", HTF_TOKEN_TYPE_C(t), HTF_TOKEN_ID(t), seq, seq->size+1);
  seq->token[seq->size++] = t;
  _htf_find_loop(thread_writer);
}

/**
 * Adds an iteration of the given sequence to the loop.
 */
static inline void _htf_loop_add_iteration(struct htf_loop* loop) {
#if 0
//	SID used to be an argument for this function, but it isn't actually required.
//	It's only there for safety purposes.
//	We shouldn't ask it, since	it actually takes some computing power to get it, most times.
//	"Safety checks are made to prevent crashes, but if you program well, you don't need to prevent crashes"
	struct htf_sequence *s1 = htf_get_sequence(&thread_writer->thread_trace, sid);
  struct htf_sequence *s2 = htf_get_sequence(&thread_writer->thread_trace,
					 HTF_TOKEN_TO_SEQUENCE_ID(loop->token));
  htf_assert(_htf_sequences_equal(s1, s2));
#endif
	loop->nb_iterations[loop->nb_loops - 1]++;
}

/** Count the number time the given token appears in the given array. */
static inline uint _htf_count_token(struct htf_thread* thread,
																		htf_token_t event,
																		htf_token_t* sequence,
																		uint sequence_size) {
	uint count = 0;
	for (int i = 0; i < sequence_size; i++) {
		htf_token_t token = sequence[i];
		count += (memcmp(&event, &token, sizeof(event)) == 0);
		if (token.type == HTF_TYPE_LOOP) {
			struct htf_loop* loop = htf_get_loop(thread, HTF_TOKEN_TO_LOOP_ID(token));
			struct htf_sequence* seq = htf_get_sequence(thread, HTF_TOKEN_TO_SEQUENCE_ID(loop->token));
			count += loop->nb_iterations[loop->nb_loops - 1] * _htf_count_token(thread, event, seq->token, seq->size);
		} else if (token.type == HTF_TYPE_SEQUENCE) {
			struct htf_sequence* seq = htf_get_sequence(thread, HTF_TOKEN_TO_SEQUENCE_ID(token));
			count += _htf_count_token(thread, event, seq->token, seq->size);
		}
	}
	return count;
};

/** Returns the timestamp for the given token, but compensates for the given array. It thus counts how many times
 * that token has already appeared, recursively (ie entering loops and sequences), and then gives the correctt
 * timestamps. */
static inline htf_timestamp_t _htf_get_timestamp(struct htf_thread* thread,
																								 htf_token_t token,
																								 htf_token_t* array,
																								 uint size) {
	uint count = _htf_count_token(thread, token, array, size);
#define CASE_EVENT_SEQUENCE(token, count)                                                 \
	case HTF_TYPE_EVENT: {                                                                  \
		struct htf_event_summary es = thread->events[token.id];                               \
		return es.timestamps[es.nb_timestamps - count];                                       \
	}                                                                                       \
	case HTF_TYPE_SEQUENCE: {                                                               \
		struct htf_sequence* seq = htf_get_sequence(thread, HTF_TOKEN_TO_SEQUENCE_ID(token)); \
		return *(htf_timestamp_t*)array_get(&seq->timestamps, seq->timestamps.size - count);  \
	}
	switch (token.type) {
		CASE_EVENT_SEQUENCE(token, count)
		case HTF_TYPE_LOOP: {
			struct htf_loop* loop = htf_get_loop(thread, HTF_TOKEN_TO_LOOP_ID(token));
			uint second_count =
					loop->nb_iterations[loop->nb_loops - 1] * count + _htf_count_token(thread, loop->token, array, size);
			switch (loop->token.type) {
				CASE_EVENT_SEQUENCE(loop->token, second_count)
				case HTF_TYPE_LOOP:
					htf_error("Loop inside a loop\n");
				case HTF_TYPE_INVALID:
					htf_error("Invalid token type.\n");
			}
		}
		case HTF_TYPE_INVALID:
			htf_error("Invalid token type.\n");
	}
}

static void _htf_create_loop(struct htf_thread_writer* thread_writer,
														 int loop_len,
														 int index_first_iteration,
														 int index_second_iteration) {
	if (index_first_iteration > index_second_iteration) {
		int tmp = index_second_iteration;
		index_second_iteration = index_first_iteration;
		index_first_iteration = tmp;
	}

	struct htf_loop* loop = _htf_create_loop_id(thread_writer, index_first_iteration, loop_len);
	//  x x x e1 e2 e3 e4 e1 e2 e3 e4
	//  x x x l1

	struct htf_sequence* cur_seq = _htf_get_cur_sequence(thread_writer);

	// We need to go back in the current sequence in order to correctly get our timestamp
	htf_token_t first_token = cur_seq->token[index_first_iteration];

	struct htf_sequence* loop_seq = htf_get_sequence(&thread_writer->thread_trace, HTF_TOKEN_TO_SEQUENCE_ID(loop->token));
	htf_timestamp_t ts = _htf_get_timestamp(&thread_writer->thread_trace, first_token,
																					&cur_seq->token[index_first_iteration], 2 * loop_len);
	array_add(&loop_seq->timestamps, &ts);
	ts = _htf_get_timestamp(&thread_writer->thread_trace, first_token, &cur_seq->token[index_second_iteration], loop_len);
	array_add(&loop_seq->timestamps, &ts);

	cur_seq->size = index_first_iteration;
	cur_seq->token[cur_seq->size] = loop->id;
	_htf_loop_add_iteration(loop);
	cur_seq->size++;
}

static void _htf_find_loop(struct htf_thread_writer *thread_writer) {

  struct htf_sequence* cur_seq = _htf_get_cur_sequence(thread_writer);
  int cur_index = cur_seq->size-1;

  if(htf_debug_level >= htf_dbg_lvl_debug) {
		printf("find loops in :\n");
		htf_print_token_array(&thread_writer->thread_trace, cur_seq->token, cur_index - MAX_LOOP_LENGTH, cur_index + 1);
	}

  for(int loop_len=1; loop_len < MAX_LOOP_LENGTH; loop_len++) {
    /* search for a loop of loop_len tokens */
    int s1_start = cur_index+1 - loop_len;
    int s2_start = cur_index+1 - 2*loop_len;

    if(s1_start > 0) {
      int loop_start = s1_start - 1;
      /* first, check if there's a loop that start at loop_start*/
      if(HTF_TOKEN_TYPE(cur_seq->token[loop_start]) == HTF_TYPE_LOOP) {
				htf_loop_id_t l = HTF_TOKEN_TO_LOOP_ID(cur_seq->token[loop_start]);
				struct htf_loop *loop = htf_get_loop(&thread_writer->thread_trace, l);
				htf_assert(loop);

				struct htf_sequence *seq = htf_get_sequence(&thread_writer->thread_trace,
									HTF_TOKEN_TO_SEQUENCE_ID(loop->token));
				htf_assert(seq);

				if (_htf_arrays_equal(&cur_seq->token[s1_start], loop_len, seq->token, seq->size)) {
					// The current sequence is just another iteration of the loop
					// remove the sequence, and increment the iteration count
					htf_log(htf_dbg_lvl_debug, "Last tokens were a sequence from L%x\n", loop->id.id);
					_htf_loop_add_iteration(loop);
					htf_timestamp_t ts = _htf_get_timestamp(&thread_writer->thread_trace, cur_seq->token[s1_start],
																									&cur_seq->token[s1_start], cur_seq->size - s1_start);
					array_add(&seq->timestamps, &ts);
					cur_seq->size = s1_start;
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
				if(htf_debug_level >= htf_dbg_lvl_debug) {
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


void _htf_record_enter_function(struct htf_thread_writer *thread_writer) {
	thread_writer->cur_depth++;
	if (thread_writer->cur_depth >= thread_writer->max_depth) {
		htf_error("depth = %d >= max_depth (%d) \n", thread_writer->cur_depth, thread_writer->max_depth);
	}
}

void _htf_record_exit_function(struct htf_thread_writer *thread_writer) {
	struct htf_sequence* cur_seq = _htf_get_cur_sequence(thread_writer);

#ifdef DEBUG
	htf_token_t first_token = cur_seq->token[0];
  htf_token_t last_token = cur_seq->token[cur_seq->size-1];
  if(HTF_TOKEN_TYPE(first_token) != HTF_TOKEN_TYPE(last_token)) {
		/* If a sequence starts with an Event (eg Enter function foo), it
			 should end with an Event too (eg. Exit function foo) */
		htf_warn("When closing sequence %p: HTF_TOKEN_TYPE(%c%x) != HTF_TOKEN_TYPE(%c%x)\n", cur_seq,
						 HTF_TOKEN_TYPE(first_token), HTF_TOKEN_ID(first_token), HTF_TOKEN_TYPE(last_token),
						 HTF_TOKEN_ID(last_token));
	}

	if(HTF_TOKEN_TYPE(first_token) == HTF_TYPE_EVENT) {
    struct htf_event *first_event = htf_get_event(&thread_writer->thread_trace, HTF_TOKEN_TO_EVENT_ID(first_token));
    struct htf_event *last_event = htf_get_event(&thread_writer->thread_trace, HTF_TOKEN_TO_EVENT_ID(last_token));
    
    enum htf_record expected_record;
    switch(first_event->record) {
    case HTF_EVENT_ENTER: expected_record = HTF_EVENT_LEAVE; break;
    case HTF_EVENT_MPI_COLLECTIVE_BEGIN: expected_record = HTF_EVENT_MPI_COLLECTIVE_END; break;
    case HTF_EVENT_OMP_FORK: expected_record = HTF_EVENT_OMP_JOIN; break;
    case HTF_EVENT_THREAD_FORK: expected_record = HTF_EVENT_THREAD_JOIN; break;
    case HTF_EVENT_THREAD_TEAM_BEGIN: expected_record = HTF_EVENT_THREAD_TEAM_END; break;
    case HTF_EVENT_THREAD_BEGIN: expected_record = HTF_EVENT_THREAD_END; break;
    case HTF_EVENT_PROGRAM_BEGIN: expected_record = HTF_EVENT_PROGRAM_END; break;
    default:
      htf_warn("Unexpected start_sequence event:\n");
      htf_print_event(&thread_writer->thread_trace, first_event);
      printf("\n");
      htf_abort();
    }
    
    if(last_event->record != expected_record) {
		htf_warn("Unexpected close event:\n");
		htf_warn("\tstart_sequence event:\n");
		htf_print_event(&thread_writer->thread_trace, first_event);
		printf("\n");
		htf_warn("\tend_sequence event:\n");
		htf_print_event(&thread_writer->thread_trace, last_event);
		printf("\n");
		}
	}

	if (cur_seq != thread_writer->og_seq[thread_writer->cur_depth]) {
		htf_error("cur_seq=%p, but og_seq[%d] = %p\n", cur_seq, thread_writer->cur_depth,
							thread_writer->og_seq[thread_writer->cur_depth]);
	}
#endif

	htf_sequence_id_t seq_id = _htf_get_sequence_id(&thread_writer->thread_trace, cur_seq);
	struct htf_sequence* seq = thread_writer->thread_trace.sequences[seq_id.id];
	htf_timestamp_t ts = _htf_get_timestamp(&thread_writer->thread_trace, seq->token[0], seq->token, seq->size);
	array_add(&seq->timestamps, &ts);
	htf_log(htf_dbg_lvl_debug, "Exiting a function, closing sequence %d (%p)\n", seq_id.id, cur_seq);

	thread_writer->cur_depth--;
	/* upper_seq is the sequence that called cur_seq */
	struct htf_sequence* upper_seq = _htf_get_cur_sequence(thread_writer);
	if (!upper_seq) {
		htf_error("upper_seq is NULL!\n");
	}

	_htf_store_token(thread_writer, upper_seq, HTF_TOKENIZE(HTF_TYPE_SEQUENCE, HTF_TOKEN_ID(seq_id)));
	cur_seq->size = 0;
}

void htf_store_event(struct htf_thread_writer *thread_writer,
		     enum htf_event_type event_type,
		     htf_event_id_t id) {

  if(event_type == htf_block_start) {
    _htf_record_enter_function(thread_writer);
  } 

  htf_token_t t = HTF_TOKENIZE(HTF_TYPE_EVENT, HTF_TOKEN_ID(id));
  struct htf_sequence *seq =  _htf_get_cur_sequence(thread_writer);
  _htf_store_token(thread_writer, seq, t);

  if(event_type == htf_block_end) {
    _htf_record_exit_function(thread_writer);
  }
}


void htf_write_thread_close(struct htf_thread_writer* thread_writer) {

  while(thread_writer->cur_depth>0) {
    htf_warn("Closing unfinished sequence (lvl %d)\n", thread_writer->cur_depth);
    _htf_record_exit_function(thread_writer);
  }
  htf_storage_finalize_thread(&thread_writer->thread_trace);
}

/* only called by the main process */
void htf_write_archive_close(struct htf_archive* archive) {
  if(!archive)
    return;

  htf_storage_finalize(archive);
}

static void _init_definition(struct htf_definition* d) {
  d->strings = malloc(sizeof(struct htf_string) * NB_STRING_DEFAULT);
  d->nb_allocated_strings = NB_STRING_DEFAULT;
  d->nb_strings = 0;

  d->regions = malloc(sizeof(struct htf_region) * NB_REGION_DEFAULT);
  d->nb_allocated_regions = NB_REGION_DEFAULT;
  d->nb_regions = 0;
}


void htf_write_archive_open(struct htf_archive *archive,
			    const char* dirname,
			    const char* trace_name,
			    htf_location_group_id_t archive_id) {

  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf_debug_level_init();

  archive->dir_name = strdup(dirname);
  archive->trace_name = strdup(trace_name);
  archive->fullpath = htf_archive_fullpath(archive->dir_name, archive->trace_name);
  archive->id = archive_id;
  archive->global_archive = NULL;

  pthread_mutex_init(&archive->lock, NULL);
  _init_definition(&archive->definitions);

  archive->nb_allocated_location_groups = NB_LOCATION_GROUPS_DEFAULT;
  archive->nb_location_groups = 0;
  archive->location_groups = malloc(sizeof(struct htf_location_group) * archive->nb_allocated_location_groups);

  archive->nb_allocated_locations = NB_LOCATIONS_DEFAULT;
  archive->nb_locations = 0;
  archive->locations = malloc(sizeof(struct htf_location) * archive->nb_allocated_locations);

  archive->nb_allocated_threads = NB_THREADS_DEFAULT;
  archive->nb_threads = 0;
  archive->threads = malloc(sizeof(struct htf_thread*) * archive->nb_allocated_threads);

  htf_storage_init(archive);

  htf_recursion_shield--;
}

struct htf_location_group * htf_get_location_group(struct htf_archive *archive,
						   htf_location_group_id_t location_group) {
  for(int i=0; i<archive->nb_location_groups; i++) {
    if(archive->location_groups[i].id == location_group)
      return &archive->location_groups[i];
  }
  return NULL;
}

struct htf_location * htf_get_location(struct htf_archive *archive,
				       htf_thread_id_t id) {
  for(int i=0; i<archive->nb_locations; i++) {
    if(archive->locations[i].id == id)
      return &archive->locations[i];
  }
  return NULL;
}

static void _init_thread(struct htf_archive *archive,
			 struct htf_thread* t,
			 htf_thread_id_t thread_id) {
  t->archive = archive;
  t->id = thread_id;

  t->nb_allocated_events = NB_EVENT_DEFAULT;
  t->events = calloc(t->nb_allocated_events, sizeof(struct htf_event_summary));
  t->nb_events = 0;

  t->nb_allocated_sequences = NB_SEQUENCE_DEFAULT;
	t->sequences = calloc(t->nb_allocated_sequences, sizeof(struct htf_sequence*));
	t->nb_sequences = 0;

	t->nb_allocated_loops = NB_LOOP_DEFAULT;
	t->loops = calloc(t->nb_allocated_loops, sizeof(struct htf_loop));
	t->nb_loops = 0;

	pthread_mutex_lock(&t->archive->lock);
	while (t->archive->nb_threads >= t->archive->nb_allocated_threads) {
		DOUBLE_MEMORY_SPACE(t->archive->threads, t->archive->nb_allocated_threads, struct htf_thread*);
	}
	for (int i = 0; i < t->nb_allocated_sequences; i++) {
		t->sequences[i] = calloc(sizeof(struct htf_sequence), 1);
		_init_sequence(t->sequences[i]);
	}
	DOFOR(i, t->nb_allocated_loops) {
		_init_loop((&t->loops[i]));
	}
	t->archive->threads[t->archive->nb_threads++] = t;
	pthread_mutex_unlock(&t->archive->lock);
}

void htf_write_thread_open(struct htf_archive* archive,
			   struct htf_thread_writer* thread_writer,
			   htf_thread_id_t thread_id) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf_assert(htf_archive_get_thread(archive, thread_id) == NULL);

  htf_log(htf_dbg_lvl_debug, "htf_write_thread_open(%ux)\n", thread_id);

  _init_thread(archive, &thread_writer->thread_trace, thread_id);
  thread_writer->max_depth = CALLSTACK_DEPTH_DEFAULT;
	thread_writer->og_seq = calloc(thread_writer->max_depth, sizeof(struct htf_sequence*));

	// the main sequence is in sequences[0]
	// FIXME This one is going to have issues when reallocating
	thread_writer->og_seq[0] = thread_writer->thread_trace.sequences[0];
	thread_writer->thread_trace.nb_sequences = 1;
	_init_sequence(thread_writer->og_seq[0]);

	for (int i = 1; i < thread_writer->max_depth; i++) {
		thread_writer->og_seq[i] = calloc(1, sizeof(struct htf_sequence));
		_init_sequence(thread_writer->og_seq[i]);
	}
	thread_writer->cur_depth = 0;

	htf_recursion_shield--;
}

void htf_write_define_location_group(struct htf_archive *archive,
				     htf_location_group_id_t id,
				     htf_string_ref_t name,
				     htf_location_group_id_t parent) {

  while(archive->nb_location_groups >= archive->nb_allocated_location_groups) {
	  htf_warn("Doubling mem space of location groups for archive %p\n", archive);
	  DOUBLE_MEMORY_SPACE(archive->location_groups, archive->nb_allocated_location_groups, struct htf_location_group);
  }

  int index = archive->nb_location_groups++;
  struct htf_location_group*l = &archive->location_groups[index];
  l->id = id;
  l->name = name;
  l->parent = parent;
}


void htf_write_define_location(struct htf_archive *archive,
			       htf_thread_id_t id,
			       htf_string_ref_t name,
			       htf_location_group_id_t parent) {

  while(archive->nb_locations >= archive->nb_allocated_locations) {
	  htf_warn("Doubling mem space of location for archive %p\n", archive);
	  DOUBLE_MEMORY_SPACE(archive->locations, archive->nb_allocated_locations, struct htf_location);
  }

  int index = archive->nb_locations++;
  struct htf_location* l = &archive->locations[index];
  l->id = id;
  htf_assert(l->id != HTF_THREAD_ID_INVALID);
  l->name = name;
  l->parent = parent;
}


void htf_write_global_archive_open(struct htf_archive* archive,
				   const char* dir_name,
				   const char* trace_name) {

  htf_write_archive_open(archive, dir_name, trace_name, HTF_MAIN_LOCATION_GROUP_ID);
}


void htf_write_global_archive_close(struct htf_archive* archive) {
  if(!archive)
    return;

  htf_storage_finalize(archive);
}

static inline void init_event(struct htf_event *e, enum htf_record record) {
  e->event_size = offsetof(struct htf_event, event_data);
  e->record = record;
}


static inline void push_data(struct htf_event *e, void* data, size_t data_size) {
  size_t o = e->event_size - offsetof(struct htf_event, event_data);
  htf_assert(o < 256);
  htf_assert(o + data_size < 256);
  memcpy(&e->event_data[o], data, data_size);
  e->event_size += data_size;
}

static inline  void pop_data(struct htf_event *e, void* data, size_t data_size, void**cursor) {
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

void htf_print_event(struct htf_thread *t, struct htf_event* e) {

  switch(e->record) {
  case HTF_EVENT_ENTER:
    {
      void*cursor = NULL;
      htf_region_ref_t region_ref;
      pop_data(e, &region_ref, sizeof(region_ref), &cursor);
      struct htf_region* region = htf_archive_get_region(t->archive, region_ref);
      const char* region_name = region ? htf_archive_get_string(t->archive, region->string_ref)->str: "INVALID";
      printf("Enter %d (%s)", region_ref, region_name);
      break;
    }
  case HTF_EVENT_LEAVE:
    {
      void*cursor = NULL;
      htf_region_ref_t region_ref;
      pop_data(e, &region_ref, sizeof(region_ref), &cursor);
      struct htf_region* region = htf_archive_get_region(t->archive, region_ref);
      const char* region_name = region ? htf_archive_get_string(t->archive, region->string_ref)->str: "INVALID";
      printf("Leave %d (%s)", region_ref, region_name);
      break;
    }

  case HTF_EVENT_THREAD_BEGIN:
      printf("THREAD_BEGIN()");
      break;

  case HTF_EVENT_THREAD_END:
      printf("THREAD_END()");
      break;

  case HTF_EVENT_THREAD_TEAM_BEGIN:
      printf("THREAD_TEAM_BEGIN()");
      break;

  case HTF_EVENT_THREAD_TEAM_END:
      printf("THREAD_TEAM_END()");
      break;

  case HTF_EVENT_MPI_SEND:
    {
      void*cursor = NULL;
      uint32_t                  receiver;
      uint32_t                  communicator;
      uint32_t                  msgTag;
      uint64_t                  msgLength;

      pop_data(e, &receiver, sizeof(receiver), &cursor);
      pop_data(e, &communicator, sizeof(communicator), &cursor);
      pop_data(e, &msgTag, sizeof(msgTag), &cursor);
      pop_data(e, &msgLength, sizeof(msgLength), &cursor);
      printf("MPI_SEND(dest=%d, comm=%x, tag=%x, len=%"PRIu64")", receiver, communicator, msgTag, msgLength);
      break;
    }
  case HTF_EVENT_MPI_ISEND:
    {
      void*cursor = NULL;
      uint32_t                  receiver;
      uint32_t                  communicator;
      uint32_t                  msgTag;
      uint64_t                  msgLength;
      uint64_t                  requestID;

      pop_data(e, &receiver, sizeof(receiver), &cursor);
      pop_data(e, &communicator, sizeof(communicator), &cursor);
      pop_data(e, &msgTag, sizeof(msgTag), &cursor);
      pop_data(e, &msgLength, sizeof(msgLength), &cursor);
      pop_data(e, &requestID, sizeof(requestID), &cursor);
      printf("MPI_ISEND(dest=%d, comm=%x, tag=%x, len=%"PRIu64", req=%"PRIx64")", receiver, communicator, msgTag, msgLength, requestID);
      break;
    }
  case HTF_EVENT_MPI_ISEND_COMPLETE:
    {
      void*cursor = NULL;
      uint64_t                  requestID;
      pop_data(e, &requestID, sizeof(requestID), &cursor);
      printf("MPI_ISEND_COMPLETE(req=%"PRIx64")", requestID);
      break;
    }
  case HTF_EVENT_MPI_IRECV_REQUEST:
    {
      void*cursor = NULL;
      uint64_t                  requestID;
      pop_data(e, &requestID, sizeof(requestID), &cursor);
      printf("MPI_IRECV_REQUEST(req=%"PRIx64")", requestID);
      break;
    }
  case HTF_EVENT_MPI_RECV:
    {
      void*cursor = NULL;
      uint32_t              sender;
      uint32_t              communicator;
      uint32_t              msgTag;
      uint64_t              msgLength;

      pop_data(e, &sender, sizeof(sender), &cursor);
      pop_data(e, &communicator, sizeof(communicator), &cursor);
      pop_data(e, &msgTag, sizeof(msgTag), &cursor);
      pop_data(e, &msgLength, sizeof(msgLength), &cursor);

      printf("MPI_RECV(src=%d, comm=%x, tag=%x, len=%"PRIu64")", sender, communicator, msgTag, msgLength);
      break;
    }
  case HTF_EVENT_MPI_IRECV:
    {
      void*cursor = NULL;
      uint32_t              sender;
      uint32_t              communicator;
      uint32_t              msgTag;
      uint64_t              msgLength;
      uint64_t              requestID;
      pop_data(e, &sender, sizeof(sender), &cursor);
      pop_data(e, &communicator, sizeof(communicator), &cursor);
      pop_data(e, &msgTag, sizeof(msgTag), &cursor);
      pop_data(e, &msgLength, sizeof(msgLength), &cursor);
      pop_data(e, &requestID, sizeof(requestID), &cursor);

      printf("MPI_IRECV(src=%d, comm=%x, tag=%x, len=%"PRIu64", req=%"PRIu64")", sender, communicator, msgTag, msgLength, requestID);
      break;
    }
  case HTF_EVENT_MPI_COLLECTIVE_BEGIN:
    {
      printf("MPI_COLLECTIVE_BEGIN()");
      break;
    }
  case HTF_EVENT_MPI_COLLECTIVE_END:
    {
      void*cursor = NULL;
      uint32_t collectiveOp;
      uint32_t communicator;
      uint32_t root;
      uint64_t sizeSent;
      uint64_t sizeReceived;

      pop_data(e, &collectiveOp, sizeof(collectiveOp), &cursor);
      pop_data(e, &communicator, sizeof(communicator), &cursor);
      pop_data(e, &root, sizeof(root), &cursor);
      pop_data(e, &sizeSent, sizeof(sizeSent), &cursor);
      pop_data(e, &sizeReceived, sizeof(sizeReceived), &cursor);

      printf("MPI_COLLECTIVE_END(op=%x, comm=%x, root=%d, sent=%"PRIu64", recved=%"PRIu64")",
	     collectiveOp,
	     communicator,
	     root,
	     sizeSent,
	     sizeReceived);
      break;
    }
	default:
    printf("{.record: %x, .size:%x}", e->record, e->event_size);
  }
}

static inline htf_event_id_t _htf_get_event_id(
		struct htf_thread *thread_trace,
		struct htf_event *e) {
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
		htf_warn("Doubling mem space of events for thread trace %p\n", thread_trace);
		DOUBLE_MEMORY_SPACE(thread_trace->events, thread_trace->nb_allocated_events, struct htf_event_summary);
	}

	int index = thread_trace->nb_events++;
	htf_log(htf_dbg_lvl_max, "\tNot found. Adding it with id=%x\n", index);
	struct htf_event_summary *es = &thread_trace->events[index];

	memcpy(&es->event, e, e->event_size);
	_init_event(es);

	return HTF_EVENT_ID(index);
}

void htf_record_enter(struct htf_thread_writer *thread_writer,
		      htf_attribute_list_t* attributeList __attribute__((unused)),
		      htf_timestamp_t       time,
		      htf_region_ref_t          region_ref ) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_ENTER);

  push_data(&e, &region_ref, sizeof(region_ref));

  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_block_start, e_id);

  htf_recursion_shield--;
}

void htf_record_leave(struct htf_thread_writer *thread_writer,
		      htf_attribute_list_t* attributeList __attribute__((unused)),
		      htf_timestamp_t       time,
		      htf_region_ref_t          region_ref ) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_LEAVE);

  push_data(&e, &region_ref, sizeof(region_ref));

  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_block_end, e_id);

  htf_recursion_shield--;
}


void htf_record_thread_begin(struct htf_thread_writer *thread_writer,
			     htf_attribute_list_t* attributeList __attribute__((unused)),
			     htf_timestamp_t       time) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_THREAD_BEGIN);

  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_block_start, e_id);

  htf_recursion_shield--;
}

void htf_record_thread_end(struct htf_thread_writer *thread_writer,
			   htf_attribute_list_t* attributeList __attribute__((unused)),
			   htf_timestamp_t       time) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_THREAD_END);
  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_block_end, e_id);

  htf_recursion_shield--;
}

void htf_record_thread_team_begin(struct htf_thread_writer *thread_writer,
				  htf_attribute_list_t* attributeList __attribute__((unused)),
				  htf_timestamp_t       time) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_THREAD_TEAM_BEGIN);
  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_block_start, e_id);

  htf_recursion_shield--;
}

void htf_record_thread_team_end(struct htf_thread_writer *thread_writer,
				htf_attribute_list_t* attributeList __attribute__((unused)),
				htf_timestamp_t       time) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_THREAD_TEAM_END);
  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_block_end, e_id);

  htf_recursion_shield--;
}

void htf_record_mpi_send(struct htf_thread_writer *thread_writer,
			 htf_attribute_list_t*     attributeList __attribute__((unused)),
			 htf_timestamp_t           time,
			 uint32_t                  receiver,
			 uint32_t                  communicator,
			 uint32_t                  msgTag,
			 uint64_t                  msgLength ) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_MPI_SEND);

  push_data(&e, &receiver, sizeof(receiver));
  push_data(&e, &communicator, sizeof(communicator));
  push_data(&e, &msgTag, sizeof(msgTag));
  push_data(&e, &msgLength, sizeof(msgLength));

  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_singleton, e_id);

  htf_recursion_shield--;
  return;
}

void htf_record_mpi_isend(struct htf_thread_writer *thread_writer,
			  htf_attribute_list_t*     attribute_list __attribute__((unused)),
			  htf_timestamp_t           time,
			  uint32_t                  receiver,
			  uint32_t                  communicator,
			  uint32_t                  msgTag,
			  uint64_t                  msgLength,
			  uint64_t                  requestID ) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_MPI_ISEND);

  push_data(&e, &receiver, sizeof(receiver));
  push_data(&e, &communicator, sizeof(communicator));
  push_data(&e, &msgTag, sizeof(msgTag));
  push_data(&e, &msgLength, sizeof(msgLength));
  push_data(&e, &requestID, sizeof(requestID));

  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_singleton, e_id);

  htf_recursion_shield--;
  return;
}

void htf_record_mpi_isend_complete(struct htf_thread_writer *thread_writer,
				   htf_attribute_list_t*     attribute_list __attribute__((unused)),
				   htf_timestamp_t           time,
				   uint64_t                  requestID ) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_MPI_ISEND_COMPLETE);

  push_data(&e, &requestID, sizeof(requestID));

  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_singleton, e_id);

  htf_recursion_shield--;
  return;
}


void htf_record_mpi_irecv_request(struct htf_thread_writer *thread_writer,
				  htf_attribute_list_t*     attribute_list __attribute__((unused)),
				  htf_timestamp_t           time,
				  uint64_t                  requestID ) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_MPI_IRECV_REQUEST);

  push_data(&e, &requestID, sizeof(requestID));

  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_singleton, e_id);

  htf_recursion_shield--;
  return;
}

void htf_record_mpi_recv(struct htf_thread_writer *thread_writer,
			 htf_attribute_list_t* attributeList __attribute__((unused)),
			 htf_timestamp_t       time,
			 uint32_t              sender,
			 uint32_t              communicator,
			 uint32_t              msgTag,
			 uint64_t              msgLength ) {

  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_MPI_RECV);

  push_data(&e, &sender, sizeof(sender));
  push_data(&e, &communicator, sizeof(communicator));
  push_data(&e, &msgTag, sizeof(msgTag));
  push_data(&e, &msgLength, sizeof(msgLength));

  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_singleton, e_id);

  htf_recursion_shield--;
  return;
}

void htf_record_mpi_irecv(struct htf_thread_writer *thread_writer,
			  htf_attribute_list_t*     attribute_list __attribute__((unused)),
			  htf_timestamp_t           time,
			  uint32_t                  sender,
			  uint32_t                  communicator,
			  uint32_t                  msgTag,
			  uint64_t                  msgLength,
			  uint64_t                  requestID ) {
  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_MPI_IRECV);

  push_data(&e, &sender, sizeof(sender));
  push_data(&e, &communicator, sizeof(communicator));
  push_data(&e, &msgTag, sizeof(msgTag));
  push_data(&e, &msgLength, sizeof(msgLength));
  push_data(&e, &requestID, sizeof(requestID));

  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_singleton, e_id);

  htf_recursion_shield--;
  return ;
}

void
htf_record_mpi_collective_begin( struct htf_thread_writer *thread_writer,
				 htf_attribute_list_t*     attribute_list __attribute__((unused)),
				 htf_timestamp_t           time) {
 if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_MPI_COLLECTIVE_BEGIN);

  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_singleton, e_id);

  htf_recursion_shield--;
  return ;
}

void
htf_record_mpi_collective_end(struct htf_thread_writer *thread_writer,
			      htf_attribute_list_t*     attribute_list __attribute__((unused)),
			      htf_timestamp_t           time,
			      uint32_t                  collectiveOp,
			      uint32_t                  communicator,
			      uint32_t                  root,
			      uint64_t                  sizeSent,
			      uint64_t                  sizeReceived ) {

  if(htf_recursion_shield)
    return;
  htf_recursion_shield++;

  struct htf_event e;
  init_event(&e, HTF_EVENT_MPI_COLLECTIVE_END);

  push_data(&e, &collectiveOp, sizeof(collectiveOp));
  push_data(&e, &communicator, sizeof(communicator));
  push_data(&e, &root,	 sizeof(root));
  push_data(&e, &sizeSent, sizeof(sizeSent));
  push_data(&e, &sizeReceived,  sizeof(sizeReceived));
  
  htf_event_id_t e_id = _htf_get_event_id(&thread_writer->thread_trace, &e);
  htf_store_timestamp(thread_writer, e_id, htf_timestamp(time));
  htf_store_event(thread_writer, htf_singleton, e_id);

  htf_recursion_shield--;
}
