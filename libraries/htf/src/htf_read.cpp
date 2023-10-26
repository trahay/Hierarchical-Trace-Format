/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include "htf/htf_read.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "htf/htf_archive.h"

namespace htf {
ThreadReader::ThreadReader(Archive* archive, ThreadId threadId, int options) {
  // Setup the basic
  this->archive = archive;
  this->options = options;
  htf_assert(threadId != HTF_THREAD_ID_INVALID);
  thread_trace = archive->getThread(threadId);
  htf_assert(thread_trace != nullptr);

  if (debugLevel >= DebugLevel::Verbose) {
    htf_log(DebugLevel::Verbose, "init callstack for thread %d\n", threadId);
    htf_log(DebugLevel::Verbose, "The trace contains:\n");
    thread_trace->printSequence(Token(TypeSequence, 0));
  }

  // And initialize the callstack
  // ie set the cursor on the first event
  referential_timestamp = 0;
  current_frame = 0;
  callstack_index[0] = 0;
  callstack_loop_iteration[0] = 0;
  callstack_sequence[0].type = TypeSequence;
  callstack_sequence[0].id = 0;
}

const Token& ThreadReader::getFrameInCallstack(int frame_number) const {
  if (frame_number < 0 || frame_number >= MAX_CALLSTACK_DEPTH) {
    htf_error("Frame number is too high or negative: %d\n", frame_number);
  }
  return callstack_sequence[frame_number];
}

const Token& ThreadReader::getTokenInCallstack(int frame_number) const {
  if (frame_number < 0 || frame_number >= MAX_CALLSTACK_DEPTH) {
    htf_error("Frame number is too high or negative: %d\n", frame_number);
  }
  auto sequence = getFrameInCallstack(frame_number);
  htf_assert(sequence.isIterable());
  return thread_trace->getToken(sequence, callstack_index[frame_number]);
}
const Token& ThreadReader::getCurToken() const {
  return getTokenInCallstack(current_frame);
}
void ThreadReader::printCurToken() const {
  thread_trace->printToken(getCurToken());
}
const Token& ThreadReader::getCurSequence() const {
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

    if (current_sequence_id.type == TypeLoop) {
      auto* loop = thread_trace->getLoop(current_sequence_id);
      printf(" iter %d/%d", callstack_loop_iteration[i],
             loop->nb_iterations[tokenCount.find(current_sequence_id)->second]);
      htf_assert(callstack_loop_iteration[i] < MAX_CALLSTACK_DEPTH);
    } else if (current_sequence_id.type == TypeSequence) {
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
  htf_assert(event.type == TypeEvent);
  if (event.id < thread_trace->nb_events) {
    return &thread_trace->events[event.id];
  }
  htf_error("Given event (%d) was invalid\n", event.id);
}
htf_timestamp_t ThreadReader::getEventTimestamp(Token event, int occurence_id) const {
  htf_assert(event.type == TypeEvent);
  auto summary = getEventSummary(event);
  if (0 <= occurence_id && occurence_id < summary->nb_occurences) {
    return summary->durations->at(occurence_id);
  }
  htf_error("Given occurence_id (%d) was invalid for event %d\n", occurence_id, event.id);
}
bool ThreadReader::isEndOfSequence(int current_index, Token sequence_id) const {
  if (sequence_id.type == TypeSequence) {
    auto* sequence = thread_trace->getSequence(sequence_id);
    return current_index >= sequence->size();
    // We are in a sequence and index is beyond the end of the sequence
  }
  htf_error("The given sequence_id was the wrong type: %d\n", sequence_id.type);
}
bool ThreadReader::isEndOfLoop(int current_index, Token loop_id) const {
  if (loop_id.type == TypeLoop) {
    auto* loop = thread_trace->getLoop(loop_id);
    return current_index >= loop->nb_iterations.back();
    // We are in a loop and index is beyond the number of iterations
  }
  htf_error("The given loop_id was the wrong type: %d\n", loop_id.type);
}

htf_timestamp_t ThreadReader::getLoopDuration(Token loop_id) const {
  htf_assert(loop_id.type == TypeLoop);
  htf_timestamp_t sum = 0;
  auto* loop = thread_trace->getLoop(loop_id);
  auto* sequence = thread_trace->getSequence(loop->repeated_token);
  size_t loopIndex = tokenCount.find(loop_id)->second;
  size_t offset = tokenCount.find(loop->repeated_token)->second;
  size_t nIterations = loop->nb_iterations.at(loopIndex);
  DOFOR(i, nIterations) {
    sum += sequence->durations->at(offset + i);
  }
  return sum;
}

EventOccurence& ThreadReader::getEventOccurence(Token event_id, int occurence_id) const {
  auto* eventOccurence = new EventOccurence();
  auto* es = getEventSummary(event_id);
  eventOccurence->event = thread_trace->getEvent(event_id);

  if ((options & ThreadReaderOptions::NoTimestamps) == 0) {
    eventOccurence->timestamp = referential_timestamp;
    eventOccurence->duration = es->durations->at(occurence_id);
  }
  eventOccurence->attributes = getEventAttributeList(event_id, occurence_id);
  return *eventOccurence;
}

SequenceOccurence& ThreadReader::getSequenceOccurence(Token sequence_id, int occurence_id) const {
  auto* sequenceOccurence = new SequenceOccurence();
  sequenceOccurence->sequence = thread_trace->getSequence(sequence_id);

  if ((options & ThreadReaderOptions::NoTimestamps) == 0) {
    sequenceOccurence->timestamp = referential_timestamp;
    sequenceOccurence->duration = sequenceOccurence->sequence->durations->at(occurence_id);
  }
  sequenceOccurence->full_sequence = nullptr;
  sequenceOccurence->savestate = new Savestate(this);

  auto tokenCount = sequenceOccurence->sequence->getTokenCount(thread_trace);
  return *sequenceOccurence;
};

LoopOccurence& ThreadReader::getLoopOccurence(Token loop_id, int occurence_id) const {
  auto* loopOccurence = new LoopOccurence();
  loopOccurence->loop = thread_trace->getLoop(loop_id);
  loopOccurence->nb_iterations = loopOccurence->loop->nb_iterations[occurence_id];
  loopOccurence->full_loop = nullptr;
  if ((options & ThreadReaderOptions::NoTimestamps) == 0) {
    loopOccurence->timestamp = referential_timestamp;
    loopOccurence->duration = getLoopDuration(loop_id);
  }
  return *loopOccurence;
}

Occurence& ThreadReader::getOccurence(htf::Token id, int occurence_id) const {
  auto occurence = new Occurence();
  switch (id.type) {
  case TypeInvalid: {
    htf_error("Wrong token was given");
  }
  case TypeEvent:
    occurence->event_occurence = getEventOccurence(id, occurence_id);
    break;
  case TypeSequence:
    occurence->sequence_occurence = getSequenceOccurence(id, occurence_id);
    break;
  case TypeLoop:
    occurence->loop_occurence = getLoopOccurence(id, occurence_id);
    break;
  }
  return *occurence;
}

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
  if (debugLevel >= DebugLevel::Debug) {
    htf_log(DebugLevel::Debug, "[%d] Enter Block ", current_frame);
    printCurToken();
    printf("\n");
  }

  current_frame++;
  callstack_index[current_frame] = 0;
  callstack_loop_iteration[current_frame] = 0;
  callstack_sequence[current_frame] = new_block;
}

void ThreadReader::leaveBlock() {
  if (debugLevel >= DebugLevel::Debug) {
    htf_log(DebugLevel::Debug, "[%d] Leave ", current_frame);
    printCurSequence();
    printf("\n");
  }

  callstack_index[current_frame] = INT16_MAX;
  callstack_sequence[current_frame] = Token();
  callstack_loop_iteration[current_frame] = INT16_MAX;
  current_frame--;  // pop frame

  if (debugLevel >= DebugLevel::Debug && current_frame >= 0) {
    auto current_sequence = getCurSequence();
    htf_assert(current_sequence.type == TypeLoop || current_sequence.type == TypeSequence);
  }
}

void ThreadReader::moveToNextToken() {
  // Check if we've reached the end of the trace
  if (current_frame < 0) {
    htf_log(DebugLevel::Debug, "End of trace %d!\n", __LINE__);
    return;
  }

  int current_index = callstack_index[current_frame];
  Token current_sequence_id = callstack_sequence[current_frame];
  int current_loop_iteration = callstack_loop_iteration[current_frame];
  htf_assert(current_sequence_id.isIterable());

  /* First update the current loop / sequence. */
  if (current_sequence_id.type == TypeSequence) {
    if (isEndOfSequence(current_index + 1, current_sequence_id)) {
      /* We've reached the end of a sequence. Leave the block and give the next event. */
      leaveBlock();
      moveToNextToken();
    } else {
      /* Move to the next event in the Sequence */
      callstack_index[current_frame]++;
    }
  } else {
    if (isEndOfLoop(current_loop_iteration + 1, current_sequence_id)) {
      /* We've reached the end of the loop. Leave the block and give the next event. */
      leaveBlock();
      moveToNextToken();
    } else {
      /* just move to the next iteration in the loop */
      callstack_loop_iteration[current_frame]++;
    }
  }
}

void ThreadReader::updateReadCurToken() {
  auto current_token = getCurToken();
  switch (current_token.type) {
  case TypeSequence: {
    tokenCount[current_token]++;
    enterBlock(current_token);
    break;
  }
  case TypeLoop: {
    tokenCount[current_token]++;
    enterBlock(current_token);
    break;
  }
  case TypeEvent: {
    // Update the timestamps
    auto summary = getEventSummary(current_token);
    if ((options & ThreadReaderOptions::NoTimestamps) == 0) {
      referential_timestamp += summary->durations->at(tokenCount[current_token]);
    }
    tokenCount[current_token]++;
    break;
  }
  default:
    break;
  }
}

Token ThreadReader::getNextToken() {
  moveToNextToken();
  updateReadCurToken();
  return getCurToken();
}
void ThreadReader::loadSavestate(Savestate* savestate) {
  if ((options & ThreadReaderOptions::NoTimestamps) == 0)
    referential_timestamp = savestate->referential_timestamp;
  memcpy(callstack_sequence, savestate->callstack_sequence, sizeof(int) * MAX_CALLSTACK_DEPTH);
  memcpy(callstack_index, savestate->callstack_index, sizeof(int) * MAX_CALLSTACK_DEPTH);
  memcpy(callstack_loop_iteration, savestate->callstack_loop_iteration, sizeof(int) * MAX_CALLSTACK_DEPTH);
  current_frame = savestate->current_frame;
  tokenCount = savestate->tokenCount;
}

std::vector<TokenOccurence> ThreadReader::readCurrentLevel() {
  Token current_sequence_id = getCurSequence();
  auto* current_sequence = thread_trace->getSequence(current_sequence_id);
  htf_assert(current_sequence->size() > 0);
  auto outputVector = std::vector<TokenOccurence>();
  outputVector.resize(current_sequence->size());

  DOFOR(i, current_sequence->size()) {
    Token token = current_sequence->tokens[i];
    outputVector[i].occurence = new Occurence;
    outputVector[i].token = &current_sequence->tokens[i];
    switch (token.type) {
    case TypeEvent: {
      // Get the info
      auto& occurence = outputVector[i].occurence->event_occurence;

      occurence = getEventOccurence(token, tokenCount[token]);
      if ((options & ThreadReaderOptions::NoTimestamps) == 0) {
        referential_timestamp += occurence.duration;
      }
      tokenCount[token]++;
      break;
    }
    case TypeLoop: {
      // Get the info
      auto& occurence = outputVector[i].occurence->loop_occurence;
      auto* loop = &thread_trace->loops[token.id];

      // Write it to the occurence
      occurence.loop = loop;
      occurence.nb_iterations = loop->nb_iterations.at(tokenCount[token]);
      if ((options & ThreadReaderOptions::NoTimestamps) == 0)
        occurence.timestamp = referential_timestamp;

      // Write the loop
      enterBlock(token);

      occurence.full_loop = new SequenceOccurence[occurence.nb_iterations];
      occurence.duration = 0;
      DOFOR(j, occurence.nb_iterations) {
        occurence.full_loop[j] = getSequenceOccurence(loop->repeated_token, tokenCount[loop->repeated_token]);
        if ((options & ThreadReaderOptions::NoTimestamps) == 0) {
          occurence.duration += occurence.full_loop[j].duration;
          referential_timestamp += occurence.full_loop[j].duration;
        }
        tokenCount[loop->repeated_token]++;
      }
      leaveBlock();

      tokenCount[token]++;
      break;
    }
    case TypeSequence: {
      // Get the info
      outputVector[i].occurence->sequence_occurence = getSequenceOccurence(token, tokenCount[token]);
      if ((options & ThreadReaderOptions::NoTimestamps) == 0) {
        referential_timestamp += outputVector[i].occurence->sequence_occurence.duration;
      }
      break;
    }
    default:
      htf_error("Invalid token type\n;");
    }
  }
  return outputVector;
}

Savestate::Savestate(const ThreadReader* reader) {
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

  tokenCount = reader->tokenCount;
#ifdef DEBUG
  savestate_memory += sizeof(Savestate);
  savestate_memory += reader->current_frame * sizeof(Token);
  savestate_memory += reader->current_frame * sizeof(int) * 2;
  savestate_memory += sizeof(tokenCount) + (tokenCount.size() * (sizeof(Token) + sizeof(size_t)));
#endif
}
} /* namespace htf */

htf::ThreadReader* htf_new_thread_reader(htf::Archive* archive,
                                         htf::ThreadId thread_id, int options) {
  return new htf::ThreadReader(archive, thread_id, options);
}

// void htf_thread_reader_enter_block(htf::ThreadReader* reader, htf::Token new_block) {
//   reader->enterBlock(new_block);
// }
//
// void htf_thread_reader_leave_block(htf::ThreadReader* reader) {
//   reader->leaveBlock();
// }

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

int htf_move_to_next_token(HTF(ThreadReader) * reader) {
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
