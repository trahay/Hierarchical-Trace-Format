/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "htf/htf_archive.h"
#include "htf/htf_read.h"

namespace htf {
ThreadReader::ThreadReader(Archive* archive, ThreadId threadId, ThreadReaderOptions options) {
  // Setup the basic
  this->archive = archive;
  this->options = options;
  htf_assert(threadId != HTF_THREAD_ID_INVALID);
  thread_trace = archive->getThread(threadId);
  htf_assert(thread_trace != nullptr);

  // Allocate what's needed
  event_index = new int[thread_trace->nb_events];
  sequence_index = new int[thread_trace->nb_sequences];
  loop_index = new int[thread_trace->nb_loops];

  if (debugLevel >= Verbose) {
    htf_log(Verbose, "init callstack for thread %d\n", threadId);
    htf_log(Verbose, "The trace contains:\n");
    thread_trace->printSequence(Token(HTF_TYPE_SEQUENCE, 0));
  }

  // And initialize the callstack
  // ie set the cursor on the first event
  referential_timestamp = 0;
  current_frame = 0;
  callstack_index[0] = 0;
  callstack_loop_iteration[0] = 0;
  callstack_sequence[0].type = HTF_TYPE_SEQUENCE;
  callstack_sequence[0].id = 0;
}

Token ThreadReader::getFrameInCallstack(int frame_number) const {
  if (frame_number < 0 || frame_number >= MAX_CALLSTACK_DEPTH) {
    htf_warn("Frame number is too high or negative: %d\n", frame_number);
    return Token();
  }
  return callstack_sequence[frame_number];
}

Token ThreadReader::getTokenInCallstack(int frame_number) const {
  if (frame_number < 0 || frame_number >= MAX_CALLSTACK_DEPTH) {
    htf_warn("Frame number is too high or negative: %d\n", frame_number);
    return Token();
  }
  auto sequence = getFrameInCallstack(frame_number);
  htf_assert(sequence.isIterable());
  return thread_trace->getToken(sequence, callstack_index[frame_number]);
}
Token ThreadReader::getCurToken() const {
  return getTokenInCallstack(current_frame);
}
void ThreadReader::printCurToken() const {
  thread_trace->printToken(getCurToken());
}
Token ThreadReader::getCurSequence() const {
  return getFrameInCallstack(current_frame);
}
void ThreadReader::printCurSequence() const {
  thread_trace->printSequence(getCurSequence());
}

void ThreadReader::printCallstack() const {
  printf("# Callstack (depth: %d) ------------\n", current_frame + 1);
  for (int i = 0; i < current_frame + 1; i++) {
    auto current_sequence_id = getFrameInCallstack(i);
    auto current_token = getTokenInCallstack(i);

    printf("%.*s[%d] ", i * 2, "                       ", i);
    thread_trace->printToken(current_sequence_id);

    if (current_sequence_id.type == HTF_TYPE_LOOP) {
      auto* loop = thread_trace->getLoop(current_sequence_id);
      printf(" iter %d/%d", callstack_loop_iteration[i], loop->nb_iterations[loop_index[current_sequence_id.id]]);
      htf_assert(callstack_loop_iteration[i] < MAX_CALLSTACK_DEPTH);
    } else if (current_sequence_id.type == HTF_TYPE_SEQUENCE) {
      auto* sequence = thread_trace->getSequence(current_sequence_id);
      printf(" pos %d/%lu", callstack_index[i], sequence->size());
      htf_assert(callstack_index[i] < MAX_CALLSTACK_DEPTH);
    }

    printf("\t-> ");
    htf_print_token(thread_trace, current_token);
    printf("\n");
  }
}
EventSummary* ThreadReader::getEventSummary(Token event) const {
  htf_assert(event.type == HTF_TYPE_EVENT);
  if (event.id < thread_trace->nb_events) {
    return &thread_trace->events[event.id];
  }
  htf_error("Given event (%d) was invalid\n", event.id);
}
htf_timestamp_t ThreadReader::getEventTimestamp(Token event, int occurence_id) const {
  htf_assert(event.type == HTF_TYPE_EVENT);
  auto summary = getEventSummary(event);
  if (0 <= occurence_id && occurence_id < summary->nb_occurences) {
    return summary->durations->at(occurence_id);
  }
  htf_error("Given occurence_id (%d) was invalid for event %d\n", occurence_id, event.id);
}
bool ThreadReader::isEndOfSequence(int current_index, Token sequence_id) const {
  if (sequence_id.type == HTF_TYPE_SEQUENCE) {
    auto* sequence = thread_trace->getSequence(sequence_id);
    return current_index >= sequence->size();
    // We are in a sequence and index is beyond the end of the sequence
  }
  htf_error("The given sequence_id was the wrong type: %d\n", sequence_id.type);
}
bool ThreadReader::isEndOfLoop(int current_index, Token loop_id) const {
  if (loop_id.type == HTF_TYPE_LOOP) {
    auto* loop = thread_trace->getLoop(loop_id);
    return current_index >= loop->nb_iterations.back();
    // We are in a loop and index is beyond the number of iterations
  }
  htf_error("The given loop_id was the wrong type: %d\n", loop_id.type);
}

htf_timestamp_t ThreadReader::getLoopDuration(Token loop_id) {
  htf_assert(loop_id.type == HTF_TYPE_LOOP);
  htf_timestamp_t sum = 0;
  auto* loop = thread_trace->getLoop(loop_id);
  auto* sequence = thread_trace->getSequence(loop->repeated_token);
  int offset = sequence_index[loop->repeated_token.id];
  DOFOR(i, i < loop->nb_iterations.at(loop_index[loop_id.id])) {
    sum += sequence->durations->at(offset + i);
  }
  return sum;
}

void ThreadReader::copyEventOccurence(Token event_id, int occurence_id, EventOccurence* event_occurence) const {
  auto* es = getEventSummary(event_id);
  memcpy(&event_occurence->event, &es->event, sizeof(es->event));

  if ((options & ThreadReaderOptions::NoTimestamps) == 0) {
    event_occurence->duration = es->durations->at(occurence_id);
  }
  event_occurence->attributes = getEventAttributeList(event_id, occurence_id);
}

void ThreadReader::writeSequenceOccurence(struct SequenceOccurence* sequence_occurence, Token token) {
  auto* sequence = thread_trace->getSequence(token);

  // Write it to the occurence
  if ((options & ThreadReaderOptions::NoTimestamps) == 0) {
    sequence_occurence->timestamp = referential_timestamp;
  }
  sequence_occurence->sequence = sequence;
  sequence_occurence->full_sequence = nullptr;
  sequence_occurence->savestate = new Savestate(this);

  // Update the reader
  if ((options & ThreadReaderOptions::NoTimestamps) == 0) {
    sequence_occurence->duration = sequence->durations->at(sequence_index[token.id]);
    referential_timestamp += sequence_occurence->duration;
  }

  skipSequence(token);
};

AttributeList* ThreadReader::getEventAttributeList(Token event_id, int occurence_id) const {
  auto* summary = getEventSummary(event_id);
  if (summary->attribute_buffer == nullptr)
    return 0;

  htf_assert(occurence_id < summary->nb_occurences);

  if (summary->attribute_pos < summary->attribute_buffer_size) {
    auto* l = (AttributeList*)&summary->attribute_buffer[summary->attribute_pos];

    while (l->index < occurence_id) { /* move to the next attribute until we reach the needed index */
      summary->attribute_pos += l->struct_size;
      l = (AttributeList*)&summary->attribute_buffer[summary->attribute_pos];
    }
    if (l->index == occurence_id) {
      return l;
    }
    if (l->index > occurence_id) {
      htf_error("Erro fetching attribute %d. We went too far (cur position: %d) !\n", occurence_id, l->index);
    }
  }
  return nullptr;
};

//******************* EXPLORATION FUNCTIONS ********************

void ThreadReader::enterBlock(Token new_block) {
  htf_assert(new_block.isIterable());
  if (debugLevel >= Debug) {
    htf_log(Debug, "[%d] Enter Block ", current_frame);
    printCurToken();
    printf("\n");
  }

  current_frame++;
  callstack_index[current_frame] = 0;
  callstack_loop_iteration[current_frame] = 0;
  callstack_sequence[current_frame] = new_block;
  if (new_block.type == HTF_TYPE_SEQUENCE) {
    auto current_sequence = getCurSequence();
    // TODO Make a savestate
  }
}
void ThreadReader::leaveBlock() {
  if (debugLevel >= Debug) {
    htf_log(Debug, "[%d] Leave ", current_frame);
    printCurSequence();
    printf("\n");
  }

  callstack_index[current_frame] = INT_MAX;
  callstack_sequence[current_frame] = Token();
  callstack_loop_iteration[current_frame] = INT_MAX;
  current_frame--;  // pop frame

  if (debugLevel >= Debug && current_frame >= 0) {
    auto current_sequence = getCurSequence();
    htf_assert(current_sequence.type == HTF_TYPE_LOOP || current_sequence.type == HTF_TYPE_SEQUENCE);
  }
}
Token ThreadReader::getNextToken() {
  // Check if we've reached the end of the trace
  if (current_frame < 0) {
    htf_log(Debug, "End of trace %d!\n", __LINE__);
    return {};
  }

  int current_index = callstack_index[current_frame];
  Token current_sequence_id = callstack_sequence[current_frame];
  int current_loop_iteration = callstack_loop_iteration[current_frame];
  htf_assert(current_sequence_id.isIterable());

  /* First update the current loop / sequence. */
  if (current_sequence_id.type == HTF_TYPE_SEQUENCE) {
    if (isEndOfSequence(current_index + 1, current_sequence_id)) {
      /* We've reached the end of a sequence. Leave the block and give the next event. */
      leaveBlock();
      return getNextToken();
    }
    /* Move to the next event in the Sequence */
    callstack_index[current_frame]++;
  } else {
    if (isEndOfLoop(current_loop_iteration + 1, current_sequence_id)) {
      /* We've reached the end of the loop. Leave the block and give the next event. */
      leaveBlock();
      return getNextToken();
    }
    /* just move to the next iteration in the loop */
    callstack_loop_iteration[current_frame]++;
  }

  auto current_token = getCurToken();
  // We need to update the reader: enter the blocks if need be, or update the timestamps if it's an Event.
  switch (current_token.type) {
  case HTF_TYPE_SEQUENCE: {
    sequence_index[current_token.id]++;
    enterBlock(current_token);
    break;
  }
  case HTF_TYPE_LOOP: {
    loop_index[current_token.id]++;
    enterBlock(current_token);
    break;
  }
  case HTF_TYPE_EVENT: {
    // Update the timestamps
    auto summary = getEventSummary(current_token);
    if ((options & ThreadReaderOptions::NoTimestamps) == 0) {
      referential_timestamp += summary->durations->at(event_index[current_token.id]);
    }
    event_index[current_token.id]++;
    break;
  }
  default:
    break;
  }
  return current_token;
}
void ThreadReader::loadSavestate(Savestate* savestate) {
  if ((options & ThreadReaderOptions::NoTimestamps) == 0)
    referential_timestamp = savestate->referential_timestamp;
  memcpy(callstack_sequence, savestate->callstack_sequence, sizeof(int) * MAX_CALLSTACK_DEPTH);
  memcpy(callstack_index, savestate->callstack_index, sizeof(int) * MAX_CALLSTACK_DEPTH);
  memcpy(callstack_loop_iteration, savestate->callstack_loop_iteration, sizeof(int) * MAX_CALLSTACK_DEPTH);
  current_frame = savestate->current_frame;
  memcpy(event_index, savestate->event_index, sizeof(int) * thread_trace->nb_events);
  memcpy(sequence_index, savestate->sequence_index, sizeof(int) * thread_trace->nb_sequences);
  memcpy(loop_index, savestate->loop_index, sizeof(int) * thread_trace->nb_loops);
}

std::pair<Token, union Occurence>* ThreadReader::readCurrentLevel() {
  Token current_sequence_id = getCurSequence();
  auto* current_sequence = thread_trace->getSequence(current_sequence_id);
  htf_assert(current_sequence->size() > 0);
  auto outputVector = new std::pair<Token, union Occurence>[current_sequence->size()];

  DOFOR(i, current_sequence->size()) {
    Token token = current_sequence->tokens[i];
    switch (token.type) {
    case HTF_TYPE_EVENT: {
      // Get the info
      auto occurence = outputVector[i].second.event_occurence;

      copyEventOccurence(token, event_index[token.id], &occurence);
      if ((options & ThreadReaderOptions::NoTimestamps) == 0) {
        occurence.timestamp = referential_timestamp;
        // Update the reader for the next ones
        referential_timestamp += occurence.duration;
      }
      event_index[token.id]++;
      break;
    }
    case HTF_TYPE_LOOP: {
      // Get the info
      auto occurence = outputVector[i].second.loop_occurence;
      auto* loop = &thread_trace->loops[token.id];

      // Write it to the occurence
      occurence.loop = loop;
      occurence.nb_iterations = loop->nb_iterations.at(loop_index[token.id]);
      if ((options & ThreadReaderOptions::NoTimestamps) == 0)
        occurence.timestamp = referential_timestamp;

      // Write the loop
      enterBlock(token);

      occurence.full_loop = new SequenceOccurence[occurence.nb_iterations];
      occurence.duration = 0;
      DOFOR(j, occurence.nb_iterations) {
        writeSequenceOccurence(&occurence.full_loop[j], loop->repeated_token);
        if ((options & ThreadReaderOptions::NoTimestamps) == 0) {
          occurence.duration += occurence.full_loop[j].duration;
        }
      }
      leaveBlock();

      loop_index[token.id]++;
      break;
    }
    case HTF_TYPE_SEQUENCE: {
      // Get the info
      writeSequenceOccurence(&outputVector[i].second.sequence_occurence, token);
      break;
    }
    default:
      htf_error("Invalid token type\n;");
    }
  }
}

Savestate::Savestate(ThreadReader* reader) {
  if ((reader->options & ThreadReaderOptions::NoTimestamps) == 0) {
    referential_timestamp = reader->referential_timestamp;
  }

  callstack_sequence = new Token[reader->current_frame];
  memcpy(callstack_sequence, reader->callstack_sequence, sizeof(Token) * reader->current_frame);

  callstack_index = new int[reader->current_frame];
  memcpy(callstack_index, reader->callstack_index, sizeof(int) * reader->current_frame);

  callstack_loop_iteration = new int[reader->current_frame];
  memcpy(callstack_loop_iteration, reader->callstack_loop_iteration, sizeof(int) * reader->current_frame);

  current_frame = reader->current_frame;

  event_index = new int[reader->thread_trace->nb_events];
  memcpy(event_index, reader->event_index, sizeof(int) * reader->thread_trace->nb_events);

  sequence_index = new int[reader->thread_trace->nb_sequences];
  memcpy(sequence_index, reader->sequence_index, sizeof(int) * reader->thread_trace->nb_sequences);

  loop_index = new int[reader->thread_trace->nb_loops];
  memcpy(loop_index, reader->loop_index, sizeof(int) * reader->thread_trace->nb_loops);

#ifdef DEBUG
  savestate_memory += sizeof(Savestate);
  savestate_memory += reader->current_frame * sizeof(Token);
  savestate_memory += reader->current_frame * sizeof(int) * 2;
  savestate_memory += reader->thread_trace->nb_events * sizeof(int);
  savestate_memory += reader->thread_trace->nb_sequences * sizeof(int);
  savestate_memory += reader->thread_trace->nb_loops * sizeof(int);
#endif
}
} /* namespace htf */

htf::ThreadReader* htf_new_thread_reader(htf::Archive* archive,
                                         htf::ThreadId thread_id,
                                         htf::ThreadReaderOptions options) {
  return new htf::ThreadReader(archive, thread_id, options);
}

void htf_thread_reader_enter_block(htf::ThreadReader* reader, htf::Token new_block) {
  reader->enterBlock(new_block);
}

void htf_thread_reader_leave_block(htf::ThreadReader* reader) {
  reader->leaveBlock();
}

C_CXX(_Thread_local, thread_local) size_t savestate_memory = 0;

struct htf::Savestate* create_savestate(htf::ThreadReader* reader) {
  return new htf::Savestate(reader);
}

void load_savestate(htf::ThreadReader* reader, htf::Savestate* savestate) {
  reader->loadSavestate(savestate);
}

int htf_read_thread_cur_token(htf::ThreadReader* reader) {
  htf_error("Not implemented yet!\n");
}

int htf_move_to_next_token(HTF(ThreadReaderName) * reader) {
  htf_error("Not implemented yet!\n");
}

int htf_read_thread_cur_level(htf::ThreadReader* reader) {
  htf_error("Not implemented yet!\n");
}

void skip_sequence(htf::ThreadReader* reader, htf::Token token) {
  reader->skipSequence(token);
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
