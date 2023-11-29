/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include "htf/htf.h"
#include "htf/htf_archive.h"

namespace htf {
/**
 * Returns the Event corresponding to the given Token
 * Aborts if the token is incorrect.
 */
Event* Thread::getEvent(Token token) const {
  return &getEventSummary(token)->event;
}

EventSummary* Thread::getEventSummary(Token token) const {
  htf_assert(token.type == TokenType::TypeEvent);
  htf_assert(token.id < this->nb_events);
  return &this->events[token.id];
}

/**
 * Returns the Sequence corresponding to the given Token
 * Aborts if the token is incorrect.
 */
Sequence* Thread::getSequence(Token token) const {
  htf_assert(token.type == TokenType::TypeSequence);
  htf_assert(token.id < this->nb_sequences);
  return this->sequences[token.id];
}
/**
 * Returns the Loop corresponding to the given Token
 * Aborts if the token is incorrect.
 */
Loop* Thread::getLoop(Token token) const {
  htf_assert(token.type == TokenType::TypeLoop);
  htf_assert(token.id < this->nb_loops);
  return &this->loops[token.id];
}

Token& Thread::getToken(Token sequenceToken, int index) const {
  if (sequenceToken.type == TypeSequence) {
    auto sequence = getSequence(sequenceToken);
    if (!sequence) {
      htf_error("Invalid sequence ID: %d\n", sequenceToken.id);
    }
    if (index >= sequence->size()) {
      htf_error("Invalid index (%d) in sequence %d\n", index, sequenceToken.id);
    }
    return sequence->tokens[index];
  } else if (sequenceToken.type == TypeLoop) {
    auto loop = getLoop(sequenceToken);
    if (!loop) {
      htf_error("Invalid loop ID: %d\n", sequenceToken.id);
    }
    if (index >= loop->nb_iterations.back()) {
      htf_error("Invalid index (%d): this loop only has %d iterations\n", index, loop->nb_iterations.back());
    }
    return loop->repeated_token;
  }
  htf_error("Invalid parameter to getToken\n");
}

/**
 * Prints a given Token.
 */
void Thread::printToken(Token token) const {
  switch (token.type) {
  case TypeEvent: {
#define ET2C(et) (((et) == HTF_EVENT_ENTER ? 'E' : (et) == HTF_EVENT_LEAVE ? 'L' : 'S'))
    Event* event = getEvent(token);
    printf("E%x_%c", token.id, ET2C(event->record));
    break;
  }
  case TypeSequence:
    printf("S%x", token.id);
    break;
  case TypeLoop:
    printf("L%x", token.id);
    break;
  default:
    printf("U%x_%x", token.type, token.id);
    break;
  }
}

void Thread::printTokenArray(const Token* array, size_t start_index, size_t len) const {
  printf("[");
  for (int i = 0; i < len; i++) {
    printToken(array[start_index + i]);
    printf(" ");
  }
  printf("]\n");
}

void Thread::printTokenVector(const std::vector<Token>& vector) const {
  printf("[");
  for (auto& token : vector) {
    printToken(token);
    printf(" ");
  }
  printf("]\n");
}

void Thread::printSequence(htf::Token token) const {
  Sequence* sequence = getSequence(token);
  printf("#Sequence %d (%zu tokens)-------------\n", token.id, sequence->tokens.size());
  printTokenVector(sequence->tokens);
}

void Thread::initThread(Archive* a, ThreadId thread_id) {
  archive = a;
  id = thread_id;

  nb_allocated_events = NB_EVENT_DEFAULT;
  events = new EventSummary[nb_allocated_events];
  nb_events = 0;

  nb_allocated_sequences = NB_SEQUENCE_DEFAULT;
  sequences = new Sequence*[nb_allocated_sequences];
  nb_sequences = 0;

  nb_allocated_loops = NB_LOOP_DEFAULT;
  loops = new Loop[nb_allocated_loops];
  nb_loops = 0;

  pthread_mutex_lock(&archive->lock);
  while (archive->nb_threads >= archive->nb_allocated_threads) {
    DOUBLE_MEMORY_SPACE(archive->threads, archive->nb_allocated_threads, Thread*);
  }
  for (int i = 0; i < nb_allocated_sequences; i++) {
    sequences[i] = new Sequence();
  }
  archive->threads[archive->nb_threads++] = this;
  pthread_mutex_unlock(&archive->lock);
}

/**
 * Returns a Thread's name.
 */
const char* Thread::getName() const {
  return archive->getString(archive->getLocation(id)->name)->str;
}

const TokenCountMap& Sequence::getTokenCount(const Thread* thread) {
  if (tokenCount.empty()) {
    // We need to count the tokens
    for (auto t = tokens.rbegin(); t != tokens.rend(); ++t) {
      tokenCount[*t]++;
      switch (t->type) {
      case TypeSequence: {
        auto* s = thread->getSequence(*t);
        tokenCount += s->getTokenCount(thread);
        break;
      }
      case TypeLoop: {
        auto* l = thread->getLoop(*t);
        auto loopTokenCount = thread->getSequence(l->repeated_token)->getTokenCount(thread);
        tokenCount += loopTokenCount * l->nb_iterations[l->nb_iterations.size() - tokenCount[*t]];
        break;
      }
      default:
        break;
      }
    }
  }
  return tokenCount;
}
}  // namespace htf

/* C bindings now */

htf::Thread* htf_thread_new() {
  return new htf::Thread();
};

const char* htf_thread_get_name(htf::Thread* thread) {
  return thread->getName();
}

void htf_print_sequence(htf::Thread* thread, htf::Token seq_id) {
  thread->printSequence(seq_id);
}

void htf_print_token_array(htf::Thread* thread, htf::Token* token_array, int index_start, int index_stop) {
  thread->printTokenArray(token_array, index_start, index_stop);
}

void htf_print_token(htf::Thread* thread, htf::Token token) {
  thread->printToken(token);
}

void htf_print_event(htf::Thread* thread, htf::Event* e) {
  thread->printEvent(e);
}
htf::Loop* htf_get_loop(htf::Thread* thread, htf::Token id) {
  return thread->getLoop(id);
}
htf::Sequence* htf_get_sequence(htf::Thread* thread, htf::Token id) {
  return thread->getSequence(id);
}
htf::Event* htf_get_event(htf::Thread* thread, htf::Token id) {
  return thread->getEvent(id);
}
htf::Token htf_get_token(htf::Thread* thread, htf::Token sequence, int index) {
  return thread->getToken(sequence, index);
}

size_t htf_sequence_get_size(htf::Sequence* sequence) {
  return sequence->size();
}
htf::Token htf_sequence_get_token(htf::Sequence* sequence, int index) {
  return sequence->tokens[index];
}

size_t htf_loop_count(htf::Loop* loop) {
  return loop->nb_iterations.size();
};
size_t htf_loop_get_count(HTF(Loop) * loop, size_t index) {
  return loop->nb_iterations[index];
};

/* -*-
  mode: cpp;
  c-file-style: "k&r";
  c-basic-offset 2;
  tab-width 2 ;
  indent-tabs-mode nil
  -*- */
