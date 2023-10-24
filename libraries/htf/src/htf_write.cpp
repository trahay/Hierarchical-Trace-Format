/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "htf/htf.h"
#include "htf/htf_archive.h"
#include "htf/htf_hash.h"
#include "htf/htf_storage.h"
#include "htf/htf_timestamp.h"
#include "htf/htf_write.h"

thread_local int htf_recursion_shield = 0;

namespace htf {
Token Thread::getSequenceId(htf::Sequence* sequence) {
  return getSequenceIdFromArray(sequence->tokens.data(), sequence->size());
}
/**
 * Compares two arrays of tokens array1 and array2
 */
static inline bool _htf_arrays_equal(Token* array1, size_t size1, Token* array2, size_t size2) {
  if (size1 != size2)
    return 0;
  return memcmp(array1, array2, sizeof(Token) * size1) == 0;
}

Token Thread::getSequenceIdFromArray(htf::Token* token_array, size_t array_len) {
  uint32_t hash;
  hash32(token_array, array_len, SEED, &hash);
  htf_log(Debug, "Searching for sequence {.size=%zu, .hash=%x}\n", array_len, hash);

  for (unsigned i = 1; i < nb_sequences; i++) {
    if (sequences[i]->hash == hash) {
      if (_htf_arrays_equal(token_array, array_len, sequences[i]->tokens.data(), sequences[i]->size())) {
        htf_log(Debug, "\t found with id=%u\n", i);
        return HTF_SEQUENCE_ID(i);
      } else {
        htf_warn("Found two sequences with the same hash\n");
      }
    }
  }

  if (nb_sequences >= nb_allocated_sequences) {
    htf_warn("Doubling mem space of sequence for thread trace %p\n", this);
    DOUBLE_MEMORY_SPACE(sequences, nb_allocated_sequences, Sequence*);
    for (int i = nb_allocated_sequences / 2; i < nb_allocated_sequences; i++) {
      sequences[i] = new Sequence;
    }
  }

  size_t index = nb_sequences++;
  Token sid = HTF_SEQUENCE_ID(index);
  htf_log(Debug, "\tSequence not found. Adding it with id=S%zx\n", index);

  Sequence* s = getSequence(sid);
  s->tokens.resize(array_len);
  memcpy(s->tokens.data(), token_array, sizeof(Token) * array_len);
  s->hash = hash;

  return sid;
}

Loop* ThreadWriter::createLoop(int start_index, int loop_len) {
  if (thread_trace.nb_loops >= thread_trace.nb_allocated_loops) {
    htf_warn("Doubling mem space of loops for thread writer %p's thread trace, cur=%d\n", this,
             thread_trace.nb_allocated_loops);
    DOUBLE_MEMORY_SPACE(thread_trace.loops, thread_trace.nb_allocated_loops, Loop);
  }

  auto* cur_seq = getCurrentSequence();
  Token sid = thread_trace.getSequenceIdFromArray(&cur_seq->tokens[start_index], loop_len);

  int index = -1;
  for (int i = 0; i < thread_trace.nb_loops; i++) {
    if (thread_trace.loops[i].repeated_token.id == sid.id) {
      index = i;
      htf_log(Debug, "\tLoop already exists: id=L%x containing S%x\n", index, sid.id);
      break;
    }
  }
  if (index == -1) {
    index = thread_trace.nb_loops++;
    htf_log(Debug, "\tLoop not found. Adding it with id=L%x containing S%x\n", index, sid.id);
  }

  Loop* l = &thread_trace.loops[index];
  l->nb_iterations.push_back(1);
  l->repeated_token = sid;
  l->self_id = HTF_LOOP_ID(index);
  return l;
}

void ThreadWriter::storeTimestamp(EventSummary* es, htf_timestamp_t ts) {
  htf_delta_timestamp(&es->durations->add(ts));
}

void ThreadWriter::storeAttributeList(htf::EventSummary* es,
                                      struct htf::AttributeList* attribute_list,
                                      size_t occurence_index) {
  attribute_list->index = occurence_index;
  if (es->attribute_pos + attribute_list->struct_size >= es->attribute_buffer_size) {
    if (es->attribute_buffer_size == 0) {
      htf_warn("Allocating attribute memory for event %u\n", es->id);
      es->attribute_buffer_size = NB_ATTRIBUTE_DEFAULT * sizeof(struct htf::AttributeList);
      es->attribute_buffer = new uint8_t[es->attribute_buffer_size];
      htf_assert(es->attribute_buffer != NULL);
    } else {
      htf_warn("Doubling mem space of attributes for event %u\n", es->id);
      DOUBLE_MEMORY_SPACE(es->attribute_buffer, es->attribute_buffer_size, uint8_t);
    }
    htf_assert(es->attribute_pos + attribute_list->struct_size < es->attribute_buffer_size);
  }

  memcpy(&es->attribute_buffer[es->attribute_pos], attribute_list, attribute_list->struct_size);
  es->attribute_pos += attribute_list->struct_size;

  htf_log(Debug, "store_attribute: {index: %d, struct_size: %d, nb_values: %d}\n", attribute_list->index,
          attribute_list->struct_size, attribute_list->nb_values);
}

void ThreadWriter::storeToken(htf::Sequence* seq, htf::Token t) {
  htf_log(Debug, "store_token: (%c%x) in %p (size: %zu)\n", HTF_TOKEN_TYPE_C(t), t.id, seq, seq->size() + 1);
  seq->tokens.push_back(t);
  findLoop();
}

/**
 * Adds an iteration of the given sequence to the loop.
 */
void Loop::addIteration() {
#if 0
  //	SID used to be an argument for this function, but it isn't actually required.
  //	It's only there for safety purposes.
  //	We shouldn't ask it, since	it actually takes some computing power to at it, most times.
  //	"Safety checks are made to prevent crashes, but if you program well, you don't need to prevent crashes"
  struct Sequence *s1 = htf_get_sequence(&thread_writer->thread_trace, sid);
  struct Sequence *s2 = htf_get_sequence(&thread_writer->thread_trace,
					     HTF_TOKEN_TO_SEQUENCE_ID(loop->repeated_token));
  htf_assert(_htf_sequences_equal(s1, s2));
#endif
  htf_log(Debug, "Adding an iteration to L%x n°%zu (to %u)\n", self_id.id, nb_iterations.size() - 1,
          nb_iterations.back() + 1);
  nb_iterations.back()++;
}

void ThreadWriter::replaceTokensInLoop(int loop_len, size_t index_first_iteration, size_t index_second_iteration) {
  if (index_first_iteration > index_second_iteration) {
    size_t tmp = index_second_iteration;
    index_second_iteration = index_first_iteration;
    index_first_iteration = tmp;
  }

  Loop* loop = createLoop(index_first_iteration, loop_len);
  Sequence* cur_seq = getCurrentSequence();

  // We need to go back in the current sequence in order to correctly calculate our durations
  Sequence* loop_seq = thread_trace.getSequence(loop->repeated_token);

  loop_seq->durations->add(thread_trace.getSequenceDuration(&cur_seq->tokens[index_first_iteration], loop_len));
  loop_seq->durations->add(thread_trace.getSequenceDuration(&cur_seq->tokens[index_second_iteration], loop_len));

  htf_add_timestamp_to_delta(&loop_seq->durations->back());

  cur_seq->tokens.resize(index_first_iteration);
  cur_seq->tokens.push_back(loop->self_id);

  loop->addIteration();
}

void ThreadWriter::findLoop() {
  Sequence* cur_seq = getCurrentSequence();
  size_t cur_index = cur_seq->size() - 1;

  if (debugLevel >= Debug) {
    printf("find loops in :\n");
    size_t start_index = (cur_index >= MAX_LOOP_LENGTH) ? cur_index - MAX_LOOP_LENGTH : 0;
    size_t len = (cur_index <= MAX_LOOP_LENGTH) ? cur_index + 1 : MAX_LOOP_LENGTH;
    thread_trace.printTokenArray(cur_seq->tokens.data(), start_index, len);
  }

  for (int loop_len = 1; loop_len < MAX_LOOP_LENGTH && loop_len <= cur_index; loop_len++) {
    /* search for a loop of loop_len tokens */
    size_t s1_start = cur_index + 1 - loop_len;
    size_t s2_start = cur_index + 1 - 2 * loop_len;

    if (s1_start > 0) {
      int loop_start = s1_start - 1;
      /* first, check if there's a loop that start at loop_start*/
      if (cur_seq->tokens[loop_start].type == TypeLoop) {
        Token l = cur_seq->tokens[loop_start];
        Loop* loop = thread_trace.getLoop(l);
        htf_assert(loop);

        Sequence* seq = thread_trace.getSequence(loop->repeated_token);
        htf_assert(seq);

        if (_htf_arrays_equal(&cur_seq->tokens[s1_start], loop_len, seq->tokens.data(), seq->size())) {
          // The current sequence is just another iteration of the loop
          // remove the sequence, and increment the iteration count
          htf_log(Debug, "Last tokens were a sequence from L%x aka S%x\n", loop->self_id.id, loop->repeated_token.id);
          loop->addIteration();
          htf_timestamp_t ts = thread_trace.getSequenceDuration(&cur_seq->tokens[s1_start], loop_len);
          htf_add_timestamp_to_delta(&seq->durations->add(ts));
          cur_seq->tokens.resize(s1_start);
          return;
        }
      }
    }

    if (cur_index + 1 >= 2 * loop_len) {
      /* search for a loop of loop_len tokens */
      int is_loop = 1;
      /* search for new loops */
      is_loop = _htf_arrays_equal(&cur_seq->tokens[s1_start], loop_len, &cur_seq->tokens[s2_start], loop_len);

      if (is_loop) {
        if (debugLevel >= Debug) {
          printf("Found a loop of len %d:\n", loop_len);
          thread_trace.printTokenArray(cur_seq->tokens.data(), s1_start, loop_len);
          thread_trace.printTokenArray(cur_seq->tokens.data(), s2_start, loop_len);
          printf("\n");
        }
        replaceTokensInLoop(loop_len, s1_start, s2_start);
        return;
      }
    }
  }
}

void ThreadWriter::recordEnterFunction() {
  cur_depth++;
  if (cur_depth >= max_depth) {
    htf_error("Depth = %d >= max_depth (%d) \n", cur_depth, max_depth);
  }
}

void ThreadWriter::recordExitFunction() {
  Sequence* cur_seq = getCurrentSequence();

#ifdef DEBUG
  Token first_token = cur_seq->tokens[0];
  Token last_token = cur_seq->tokens.back();
  if (first_token.type != last_token.type) {
    /* If a sequence starts with an Event (eg Enter function foo), it
       should end with an Event too (eg. Exit function foo) */
    htf_warn("When closing sequence %p: HTF_TOKEN_TYPE(%c%x) != HTF_TOKEN_TYPE(%c%x)\n", cur_seq, first_token.type,
             first_token.id, last_token.type, last_token.id);
  }

  if (first_token.type == TypeEvent) {
    Event* first_event = thread_trace.getEvent(first_token);
    Event* last_event = thread_trace.getEvent(last_token);

    enum Record expected_record;
    switch (first_event->record) {
    case HTF_EVENT_ENTER:
      expected_record = HTF_EVENT_LEAVE;
      break;
    case HTF_EVENT_MPI_COLLECTIVE_BEGIN:
      expected_record = HTF_EVENT_MPI_COLLECTIVE_END;
      break;
    case HTF_EVENT_OMP_FORK:
      expected_record = HTF_EVENT_OMP_JOIN;
      break;
    case HTF_EVENT_THREAD_FORK:
      expected_record = HTF_EVENT_THREAD_JOIN;
      break;
    case HTF_EVENT_THREAD_TEAM_BEGIN:
      expected_record = HTF_EVENT_THREAD_TEAM_END;
      break;
    case HTF_EVENT_THREAD_BEGIN:
      expected_record = HTF_EVENT_THREAD_END;
      break;
    case HTF_EVENT_PROGRAM_BEGIN:
      expected_record = HTF_EVENT_PROGRAM_END;
      break;
    default:
      htf_warn("Unexpected start_sequence event:\n");
      thread_trace.printEvent(first_event);
      printf("\n");
      htf_abort();
    }

    if (last_event->record != expected_record) {
      htf_warn("Unexpected close event:\n");
      htf_warn("\tStart_sequence event:\n");
      thread_trace.printEvent(first_event);
      printf("\n");
      htf_warn("\tEnd_sequence event:\n");
      thread_trace.printEvent(last_event);
      printf("\n");
    }
  }

  if (cur_seq != og_seq[cur_depth]) {
    htf_error("cur_seq=%p, but og_seq[%d] = %p\n", cur_seq, cur_depth, og_seq[cur_depth]);
  }
#endif

  Token seq_id = thread_trace.getSequenceId(cur_seq);
  auto* seq = thread_trace.sequences[seq_id.id];
  htf_timestamp_t ts = thread_trace.getSequenceDuration(seq->tokens.data(), seq->size());
  htf_add_timestamp_to_delta(&seq->durations->add(ts));
  htf_log(Debug, "Exiting a function, closing sequence %d (%p)\n", seq_id.id, cur_seq);

  cur_depth--;
  /* upper_seq is the sequence that called cur_seq */
  Sequence* upper_seq = getCurrentSequence();
  if (!upper_seq) {
    htf_error("upper_seq is NULL!\n");
  }

  storeToken(upper_seq, seq_id);
  cur_seq->tokens.resize(0);
  // We need to reset the token vector
  // Calling vector::clear() might be a better way to do that,
  // but depending on the implementation it might force a bunch of realloc, which isn't great.
}

size_t ThreadWriter::storeEvent(enum EventType event_type,
                                TokenId event_id,
                                htf_timestamp_t ts,
                                AttributeList* attribute_list) {
  if (event_type == HTF_BLOCK_START) {
    recordEnterFunction();
  }

  Token token = Token(TypeEvent, event_id);
  auto* sequence = getCurrentSequence();
  storeToken(sequence, token);

  EventSummary* es = &thread_trace.events[event_id];
  size_t occurrence_index = es->nb_occurences++;

  storeTimestamp(es, htf_timestamp(ts));
  if (attribute_list)
    storeAttributeList(es, attribute_list, occurrence_index);

  if (event_type == HTF_BLOCK_END) {
    recordExitFunction();
  }
  return occurrence_index;
}

void ThreadWriter::threadClose() {
  while (cur_depth > 0) {
    htf_warn("Closing unfinished sequence (lvl %d)\n", cur_depth);
    recordExitFunction();
  }
  thread_trace.finalizeThread();
}

void Archive::open(const char* dirname, const char* given_trace_name, LocationGroupId archive_id) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;
  htf_debug_level_init();

  dir_name = strdup(dirname);
  trace_name = strdup(given_trace_name);
  fullpath = htf_archive_fullpath(dir_name, trace_name);
  id = archive_id;
  global_archive = nullptr;

  pthread_mutex_init(&lock, nullptr);

  nb_allocated_threads = NB_THREADS_DEFAULT;
  nb_threads = 0;
  threads = new Thread*[nb_allocated_threads];

  htf_storage_init(this);

  htf_recursion_shield--;
}

void ThreadWriter::open(Archive* archive, ThreadId thread_id) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf_assert(htf_archive_get_thread(archive, thread_id) == nullptr);

  htf_log(Debug, "htf_write_thread_open(%ux)\n", thread_id);

  thread_trace = Thread(archive, thread_id);
  max_depth = CALLSTACK_DEPTH_DEFAULT;
  og_seq = new Sequence*[max_depth];

  // the main sequence is in sequences[0]
  og_seq[0] = thread_trace.sequences[0];
  thread_trace.nb_sequences = 1;

  for (int i = 1; i < max_depth; i++) {
    og_seq[i] = new Sequence();
  }
  cur_depth = 0;

  htf_recursion_shield--;
}

void Archive::defineLocationGroup(LocationGroupId id, StringRef name, LocationGroupId parent) {
  LocationGroup l = LocationGroup();
  l.id = id;
  l.name = name;
  l.parent = parent;
  location_groups.push_back(l);
}

void Archive::defineLocation(ThreadId id, StringRef name, LocationGroupId parent) {
  Location l = Location();
  l.id = id;
  htf_assert(l.id != HTF_THREAD_ID_INVALID);
  l.name = name;
  l.parent = parent;
  locations.push_back(l);
}

void Archive::close() {
  htf_storage_finalize(this);
}

static inline void init_event(Event* e, enum Record record) {
  e->event_size = offsetof(Event, event_data);
  e->record = record;
}

static inline void push_data(Event* e, void* data, size_t data_size) {
  size_t o = e->event_size - offsetof(Event, event_data);
  htf_assert(o < 256);
  htf_assert(o + data_size < 256);
  memcpy(&e->event_data[o], data, data_size);
  e->event_size += data_size;
}

static inline void pop_data(Event* e, void* data, size_t data_size, void** cursor) {
  if (*cursor == nullptr) {
    /* initialize the cursor to the begining of event data */
    *cursor = &e->event_data[0];
  }

  uintptr_t last_event_byte = ((uintptr_t)e) + e->event_size;
  uintptr_t last_read_byte = ((uintptr_t)*cursor) + data_size;
  htf_assert(last_read_byte <= last_event_byte);

  memcpy(data, *cursor, data_size);
  *(size_t**)cursor += data_size;
  // We have to cast because we can't do arithmetic on void* pointer apparently
}

void Thread::printEvent(htf::Event* e) const {
  switch (e->record) {
  case HTF_EVENT_ENTER: {
    void* cursor = NULL;
    RegionRef region_ref;
    pop_data(e, &region_ref, sizeof(region_ref), &cursor);
    const Region* region = archive->getRegion(region_ref);
    const char* region_name = region ? archive->getString(region->string_ref)->str : "INVALID";
    printf("Enter %d (%s)", region_ref, region_name);
    break;
  }
  case HTF_EVENT_LEAVE: {
    void* cursor = NULL;
    RegionRef region_ref;
    pop_data(e, &region_ref, sizeof(region_ref), &cursor);
    const Region* region = archive->getRegion(region_ref);
    const char* region_name = region ? archive->getString(region->string_ref)->str : "INVALID";
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

  case HTF_EVENT_MPI_SEND: {
    void* cursor = NULL;
    uint32_t receiver;
    uint32_t communicator;
    uint32_t msgTag;
    uint64_t msgLength;

    pop_data(e, &receiver, sizeof(receiver), &cursor);
    pop_data(e, &communicator, sizeof(communicator), &cursor);
    pop_data(e, &msgTag, sizeof(msgTag), &cursor);
    pop_data(e, &msgLength, sizeof(msgLength), &cursor);
    printf("MPI_SEND(dest=%d, comm=%x, tag=%x, len=%" PRIu64 ")", receiver, communicator, msgTag, msgLength);
    break;
  }
  case HTF_EVENT_MPI_ISEND: {
    void* cursor = NULL;
    uint32_t receiver;
    uint32_t communicator;
    uint32_t msgTag;
    uint64_t msgLength;
    uint64_t requestID;

    pop_data(e, &receiver, sizeof(receiver), &cursor);
    pop_data(e, &communicator, sizeof(communicator), &cursor);
    pop_data(e, &msgTag, sizeof(msgTag), &cursor);
    pop_data(e, &msgLength, sizeof(msgLength), &cursor);
    pop_data(e, &requestID, sizeof(requestID), &cursor);
    printf("MPI_ISEND(dest=%d, comm=%x, tag=%x, len=%" PRIu64 ", req=%" PRIx64 ")", receiver, communicator, msgTag,
           msgLength, requestID);
    break;
  }
  case HTF_EVENT_MPI_ISEND_COMPLETE: {
    void* cursor = NULL;
    uint64_t requestID;
    pop_data(e, &requestID, sizeof(requestID), &cursor);
    printf("MPI_ISEND_COMPLETE(req=%" PRIx64 ")", requestID);
    break;
  }
  case HTF_EVENT_MPI_IRECV_REQUEST: {
    void* cursor = NULL;
    uint64_t requestID;
    pop_data(e, &requestID, sizeof(requestID), &cursor);
    printf("MPI_IRECV_REQUEST(req=%" PRIx64 ")", requestID);
    break;
  }
  case HTF_EVENT_MPI_RECV: {
    void* cursor = NULL;
    uint32_t sender;
    uint32_t communicator;
    uint32_t msgTag;
    uint64_t msgLength;

    pop_data(e, &sender, sizeof(sender), &cursor);
    pop_data(e, &communicator, sizeof(communicator), &cursor);
    pop_data(e, &msgTag, sizeof(msgTag), &cursor);
    pop_data(e, &msgLength, sizeof(msgLength), &cursor);

    printf("MPI_RECV(src=%d, comm=%x, tag=%x, len=%" PRIu64 ")", sender, communicator, msgTag, msgLength);
    break;
  }
  case HTF_EVENT_MPI_IRECV: {
    void* cursor = NULL;
    uint32_t sender;
    uint32_t communicator;
    uint32_t msgTag;
    uint64_t msgLength;
    uint64_t requestID;
    pop_data(e, &sender, sizeof(sender), &cursor);
    pop_data(e, &communicator, sizeof(communicator), &cursor);
    pop_data(e, &msgTag, sizeof(msgTag), &cursor);
    pop_data(e, &msgLength, sizeof(msgLength), &cursor);
    pop_data(e, &requestID, sizeof(requestID), &cursor);

    printf("MPI_IRECV(src=%d, comm=%x, tag=%x, len=%" PRIu64 ", req=%" PRIu64 ")", sender, communicator, msgTag,
           msgLength, requestID);
    break;
  }
  case HTF_EVENT_MPI_COLLECTIVE_BEGIN: {
    printf("MPI_COLLECTIVE_BEGIN()");
    break;
  }
  case HTF_EVENT_MPI_COLLECTIVE_END: {
    void* cursor = NULL;
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

    printf("MPI_COLLECTIVE_END(op=%x, comm=%x, root=%d, sent=%" PRIu64 ", recved=%" PRIu64 ")", collectiveOp,
           communicator, root, sizeSent, sizeReceived);
    break;
  }
  default:
    printf("{.record: %x, .size:%x}", e->record, e->event_size);
  }
}

TokenId Thread::getEventId(htf::Event* e) {
  htf_log(Max, "Searching for event {.event_type=%d}\n", e->record);

  htf_assert(e->event_size < 256);

  for (TokenId i = 0; i < nb_events; i++) {
    if (memcmp(e, &events[i].event, e->event_size) == 0) {
      htf_log(Max, "\t found with id=%u\n", i);
      return i;
    }
  }

  if (nb_events >= nb_allocated_events) {
    htf_warn("Doubling mem space of events for thread trace %p\n", this);
    DOUBLE_MEMORY_SPACE(events, nb_allocated_events, EventSummary);
  }

  TokenId index = nb_events++;
  htf_log(Max, "\tNot found. Adding it with id=%x\n", index);
  auto* new_event = &events[index];

  new_event->id = index;
  memcpy(&new_event->event, e, e->event_size);

  return index;
}
htf_timestamp_t Thread::getSequenceDuration(Token* array, size_t size) {
  htf_timestamp_t sum = 0;
  auto tokenCount = TokenCountMap();
  for (size_t i = 0; i < size; i++) {
    auto& token = array[i];
    tokenCount[token]++;
    switch (token.type) {
    case TypeInvalid: {
      htf_error("Error parsing the given array, a Token was invalid\n");
      break;
    }
    case TypeEvent: {
      auto summary = getEventSummary(token);
      sum += summary->durations->at(summary->durations->size - tokenCount[token]);
      break;
    }
    case TypeSequence: {
      auto sequence = getSequence(token);
      sum += sequence->durations->at(sequence->durations->size - tokenCount[token]);
      tokenCount += sequence->getTokenCount(this);
      break;
    }
    case TypeLoop: {
      auto loop = getLoop(token);
      auto nb_iterations = loop->nb_iterations[loop->nb_iterations.size() - tokenCount[token]];
      auto sequence = getSequence(loop->repeated_token);
      for (size_t j = 0; j < nb_iterations; j++) {
        tokenCount[loop->repeated_token]++;
        sum += sequence->durations->at(sequence->durations->size - tokenCount[loop->repeated_token]);
      }
      tokenCount += sequence->getTokenCount(this) * (size_t)nb_iterations;
      break;
    }
    }
  }
  return sum;
}
}  // namespace htf

/* C Callbacks */
extern void htf_write_global_archive_open(htf::Archive* archive, const char* dir_name, const char* trace_name) {
  archive->globalOpen(dir_name, trace_name);
};
extern void htf_write_global_archive_close(htf::Archive* archive) {
  archive->close();
};

extern void htf_write_thread_open(htf::Archive* archive, htf::ThreadWriter* thread_writer, htf::ThreadId thread_id) {
  thread_writer->open(archive, thread_id);
};

extern void htf_write_thread_close(htf::ThreadWriter* thread_writer) {
  thread_writer->threadClose();
};

extern void htf_write_define_location_group(htf::Archive* archive,
                                            htf::LocationGroupId id,
                                            htf::StringRef name,
                                            htf::LocationGroupId parent) {
  archive->defineLocationGroup(id, name, parent);
};

extern void htf_write_define_location(htf::Archive* archive,
                                      htf::ThreadId id,
                                      htf::StringRef name,
                                      htf::LocationGroupId parent) {
  archive->defineLocation(id, name, parent);
};

extern void htf_write_archive_open(htf::Archive* archive,
                                   const char* dir_name,
                                   const char* trace_name,
                                   htf::LocationGroupId location_group) {
  archive->open(dir_name, trace_name, location_group);
};

extern void htf_write_archive_close(HTF(Archive) * archive) {
  archive->close();
};

void htf_store_event(HTF(ThreadWriter) * thread_writer,
                     enum HTF(EventType) event_type,
                     HTF(TokenId) id,
                     htf_timestamp_t ts,
                     HTF(AttributeList) * attribute_list) {
  thread_writer->storeEvent(event_type, id, ts, attribute_list);
};

void htf_record_enter(htf::ThreadWriter* thread_writer,
                      struct htf::AttributeList* attribute_list __attribute__((unused)),
                      htf_timestamp_t time,
                      htf::RegionRef region_ref) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf::Event e;
  init_event(&e, htf::HTF_EVENT_ENTER);

  push_data(&e, &region_ref, sizeof(region_ref));

  htf::TokenId e_id = thread_writer->thread_trace.getEventId(&e);

  thread_writer->storeEvent(htf::HTF_BLOCK_START, e_id, time, attribute_list);

  htf_recursion_shield--;
}

void htf_record_leave(htf::ThreadWriter* thread_writer,
                      struct htf::AttributeList* attribute_list __attribute__((unused)),
                      htf_timestamp_t time,
                      htf::RegionRef region_ref) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf::Event e;
  init_event(&e, htf::HTF_EVENT_LEAVE);

  push_data(&e, &region_ref, sizeof(region_ref));

  htf::TokenId e_id = thread_writer->thread_trace.getEventId(&e);
  thread_writer->storeEvent(htf::HTF_BLOCK_END, e_id, time, attribute_list);

  htf_recursion_shield--;
}

void htf_record_thread_begin(htf::ThreadWriter* thread_writer,
                             struct htf::AttributeList* attribute_list __attribute__((unused)),
                             htf_timestamp_t time) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf::Event e;
  init_event(&e, htf::HTF_EVENT_THREAD_BEGIN);

  htf::TokenId e_id = thread_writer->thread_trace.getEventId(&e);
  thread_writer->storeEvent(htf::HTF_BLOCK_START, e_id, time, attribute_list);

  htf_recursion_shield--;
}

void htf_record_thread_end(htf::ThreadWriter* thread_writer,
                           struct htf::AttributeList* attribute_list __attribute__((unused)),
                           htf_timestamp_t time) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf::Event e;
  init_event(&e, htf::HTF_EVENT_THREAD_END);
  htf::TokenId e_id = thread_writer->thread_trace.getEventId(&e);
  thread_writer->storeEvent(htf::HTF_BLOCK_END, e_id, time, attribute_list);

  htf_recursion_shield--;
}

void htf_record_thread_team_begin(htf::ThreadWriter* thread_writer,
                                  struct htf::AttributeList* attribute_list __attribute__((unused)),
                                  htf_timestamp_t time) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf::Event e;
  init_event(&e, htf::HTF_EVENT_THREAD_TEAM_BEGIN);
  htf::TokenId e_id = thread_writer->thread_trace.getEventId(&e);
  thread_writer->storeEvent(htf::HTF_BLOCK_START, e_id, time, attribute_list);

  htf_recursion_shield--;
}

void htf_record_thread_team_end(htf::ThreadWriter* thread_writer,
                                struct htf::AttributeList* attribute_list __attribute__((unused)),
                                htf_timestamp_t time) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf::Event e;
  init_event(&e, htf::HTF_EVENT_THREAD_TEAM_END);
  htf::TokenId e_id = thread_writer->thread_trace.getEventId(&e);
  thread_writer->storeEvent(htf::HTF_BLOCK_END, e_id, time, attribute_list);

  htf_recursion_shield--;
}

void htf_record_mpi_send(htf::ThreadWriter* thread_writer,
                         struct htf::AttributeList* attribute_list __attribute__((unused)),
                         htf_timestamp_t time,
                         uint32_t receiver,
                         uint32_t communicator,
                         uint32_t msgTag,
                         uint64_t msgLength) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf::Event e;
  init_event(&e, htf::HTF_EVENT_MPI_SEND);

  push_data(&e, &receiver, sizeof(receiver));
  push_data(&e, &communicator, sizeof(communicator));
  push_data(&e, &msgTag, sizeof(msgTag));
  push_data(&e, &msgLength, sizeof(msgLength));

  htf::TokenId e_id = thread_writer->thread_trace.getEventId(&e);
  thread_writer->storeEvent(htf::HTF_SINGLETON, e_id, time, attribute_list);

  htf_recursion_shield--;
  return;
}

void htf_record_mpi_isend(htf::ThreadWriter* thread_writer,
                          struct htf::AttributeList* attribute_list __attribute__((unused)),
                          htf_timestamp_t time,
                          uint32_t receiver,
                          uint32_t communicator,
                          uint32_t msgTag,
                          uint64_t msgLength,
                          uint64_t requestID) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf::Event e;
  init_event(&e, htf::HTF_EVENT_MPI_ISEND);

  push_data(&e, &receiver, sizeof(receiver));
  push_data(&e, &communicator, sizeof(communicator));
  push_data(&e, &msgTag, sizeof(msgTag));
  push_data(&e, &msgLength, sizeof(msgLength));
  push_data(&e, &requestID, sizeof(requestID));

  htf::TokenId e_id = thread_writer->thread_trace.getEventId(&e);
  thread_writer->storeEvent(htf::HTF_SINGLETON, e_id, time, attribute_list);

  htf_recursion_shield--;
  return;
}

void htf_record_mpi_isend_complete(htf::ThreadWriter* thread_writer,
                                   struct htf::AttributeList* attribute_list __attribute__((unused)),
                                   htf_timestamp_t time,
                                   uint64_t requestID) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf::Event e;
  init_event(&e, htf::HTF_EVENT_MPI_ISEND_COMPLETE);

  push_data(&e, &requestID, sizeof(requestID));

  htf::TokenId e_id = thread_writer->thread_trace.getEventId(&e);
  thread_writer->storeEvent(htf::HTF_SINGLETON, e_id, time, attribute_list);

  htf_recursion_shield--;
  return;
}

void htf_record_mpi_irecv_request(htf::ThreadWriter* thread_writer,
                                  struct htf::AttributeList* attribute_list __attribute__((unused)),
                                  htf_timestamp_t time,
                                  uint64_t requestID) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf::Event e;
  init_event(&e, htf::HTF_EVENT_MPI_IRECV_REQUEST);

  push_data(&e, &requestID, sizeof(requestID));

  htf::TokenId e_id = thread_writer->thread_trace.getEventId(&e);
  thread_writer->storeEvent(htf::HTF_SINGLETON, e_id, time, attribute_list);

  htf_recursion_shield--;
  return;
}

void htf_record_mpi_recv(htf::ThreadWriter* thread_writer,
                         struct htf::AttributeList* attribute_list __attribute__((unused)),
                         htf_timestamp_t time,
                         uint32_t sender,
                         uint32_t communicator,
                         uint32_t msgTag,
                         uint64_t msgLength) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf::Event e;
  init_event(&e, htf::HTF_EVENT_MPI_RECV);

  push_data(&e, &sender, sizeof(sender));
  push_data(&e, &communicator, sizeof(communicator));
  push_data(&e, &msgTag, sizeof(msgTag));
  push_data(&e, &msgLength, sizeof(msgLength));

  htf::TokenId e_id = thread_writer->thread_trace.getEventId(&e);
  thread_writer->storeEvent(htf::HTF_SINGLETON, e_id, time, attribute_list);

  htf_recursion_shield--;
  return;
}

void htf_record_mpi_irecv(htf::ThreadWriter* thread_writer,
                          struct htf::AttributeList* attribute_list __attribute__((unused)),
                          htf_timestamp_t time,
                          uint32_t sender,
                          uint32_t communicator,
                          uint32_t msgTag,
                          uint64_t msgLength,
                          uint64_t requestID) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf::Event e;
  init_event(&e, htf::HTF_EVENT_MPI_IRECV);

  push_data(&e, &sender, sizeof(sender));
  push_data(&e, &communicator, sizeof(communicator));
  push_data(&e, &msgTag, sizeof(msgTag));
  push_data(&e, &msgLength, sizeof(msgLength));
  push_data(&e, &requestID, sizeof(requestID));

  htf::TokenId e_id = thread_writer->thread_trace.getEventId(&e);
  thread_writer->storeEvent(htf::HTF_SINGLETON, e_id, time, attribute_list);

  htf_recursion_shield--;
  return;
}

void htf_record_mpi_collective_begin(htf::ThreadWriter* thread_writer,
                                     struct htf::AttributeList* attribute_list __attribute__((unused)),
                                     htf_timestamp_t time) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf::Event e;
  init_event(&e, htf::HTF_EVENT_MPI_COLLECTIVE_BEGIN);

  htf::TokenId e_id = thread_writer->thread_trace.getEventId(&e);
  thread_writer->storeEvent(htf::HTF_SINGLETON, e_id, time, attribute_list);

  htf_recursion_shield--;
  return;
}

void htf_record_mpi_collective_end(htf::ThreadWriter* thread_writer,
                                   struct htf::AttributeList* attribute_list __attribute__((unused)),
                                   htf_timestamp_t time,
                                   uint32_t collectiveOp,
                                   uint32_t communicator,
                                   uint32_t root,
                                   uint64_t sizeSent,
                                   uint64_t sizeReceived) {
  if (htf_recursion_shield)
    return;
  htf_recursion_shield++;

  htf::Event e;
  init_event(&e, htf::HTF_EVENT_MPI_COLLECTIVE_END);

  push_data(&e, &collectiveOp, sizeof(collectiveOp));
  push_data(&e, &communicator, sizeof(communicator));
  push_data(&e, &root, sizeof(root));
  push_data(&e, &sizeSent, sizeof(sizeSent));
  push_data(&e, &sizeReceived, sizeof(sizeReceived));

  htf::TokenId e_id = thread_writer->thread_trace.getEventId(&e);
  thread_writer->storeEvent(htf::HTF_SINGLETON, e_id, time, attribute_list);

  htf_recursion_shield--;
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
