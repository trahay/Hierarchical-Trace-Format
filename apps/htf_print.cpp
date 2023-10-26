/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
#include <algorithm>
#include <iostream>
#include <string>
#include "htf/htf.h"
#include "htf/htf_archive.h"
#include "htf/htf_read.h"
#include "htf/htf_storage.h"

static bool show_structure = false;
static bool per_thread = false;
static std::string structure_indent[MAX_CALLSTACK_DEPTH];
static short store_timestamps = 1;

static bool print_timestamp = true;
static bool print_duration = false;
static bool unroll_loops = false;
static bool explore_loop_sequences = false;
static bool verbose = false;

static void _print_timestamp(htf_timestamp_t ts) {
  if (print_timestamp) {
    printf("%21.9lf\t", ts / 1e9);
  }
}
static void _print_timestamp_header() {
  if (print_timestamp) {
    printf("%21s\t", "Timestamp");
  }
}

static void _print_duration(htf_timestamp_t d) {
  if (print_duration) {
    printf("%21.9lf\t", d / 1e9);
  }
}

static void _print_duration_header() {
  if (print_duration) {
    printf("%21s\t", "Duration");
  }
}

static void _print_indent(const std::string& current_indent) {
  std::cout << current_indent;
}

/* Print one event */
static void print_event(const std::string& current_indent,
                        const htf::Thread* thread,
                        const htf::Token token,
                        const htf::EventOccurence* e) {
  _print_timestamp(e->timestamp);
  _print_duration(e->duration);
  _print_indent(current_indent);

  if (!per_thread)
    std::cout << thread->getName() << "\t";
  if (verbose) {
    thread->printToken(token);
    std::cout << "\t";
  }
  thread->printEvent(e->event);
  thread->printEventAttribute(e);
  std::cout << std::endl;
}

static void print_sequence(const std::string& current_indent,
                           const htf::Thread* thread,
                           const htf::Token token,
                           const htf::SequenceOccurence* sequenceOccurence,
                           const htf::LoopOccurence* containingLoopOccurence = nullptr) {
  auto* sequence = sequenceOccurence->sequence;
  htf_timestamp_t ts = sequenceOccurence->timestamp;
  htf_timestamp_t duration = sequenceOccurence->duration;

  _print_timestamp(ts);
  _print_duration(duration);
  _print_indent(current_indent);

  if (!per_thread)
    std::cout << thread->getName() << "\t";
  if (show_structure) {
    std::cout << "Sequence ";
    thread->printToken(token);
  }

  if (verbose) {
    thread->printToken(token);
    printf("\t");
    for (unsigned i = 0; i < sequence->size(); i++) {
      thread->printToken(sequence->tokens[i]);
      std::cout << " ";
    }
  }

  if (containingLoopOccurence && (unroll_loops || explore_loop_sequences)) {
    //    htf_timestamp_t mean = containingLoopOccurence->duration / containingLoopOccurence->nb_iterations;
    //    std::cout << ((mean < duration) ? "-" : "+");
    //    uint64_t diff = (mean < duration) ? duration - mean : mean - duration;
    //    float percentile = (float)diff / (float)mean * 100;
    //    printf("%5.2f%%", percentile);
  }
  std::cout << std::endl;
}

static void print_loop(const std::string& current_indent,
                       const htf::Thread* thread,
                       const htf::Token token,
                       const htf::LoopOccurence* loopOccurence) {
  _print_timestamp(loopOccurence->timestamp);
  _print_duration(loopOccurence->duration);
  _print_indent(current_indent);

  if (!per_thread)
    std::cout << thread->getName() << "\t";

  if (show_structure) {
    std::cout << "Loop ";
  }

  auto* loop = loopOccurence->loop;

  thread->printToken(token);
  std::cout << "\t" << loopOccurence->nb_iterations << " * ";
  thread->printToken(loop->repeated_token);
  std::cout << std::endl;
}

static void print_token(const htf::Thread* thread,
                        const htf::Token* t,
                        const htf::Occurence* e,
                        int depth = 0,
                        int last_one = 0,
                        const htf::LoopOccurence* containing_loop = nullptr) {
  htf_log(htf::DebugLevel::Verbose, "Reading repeated_token(%x.%x) for thread %s\n", t->type, t->id, thread->getName());
  // Prints the structure of the sequences and the loops
  std::string current_indent;
  if (show_structure && depth >= 1) {
    structure_indent[depth - 1] = (last_one ? "╰" : "├");
    DOFOR(i, depth) {
      current_indent += structure_indent[i];
    }
    if (t->type != htf::TypeEvent) {
      current_indent += (containing_loop && !explore_loop_sequences) ? "─" : "┬";
    } else {
      current_indent += "─";
    }
    structure_indent[depth - 1] = last_one ? " " : "│";
  }

  // Prints the repeated_token we first started with
  switch (t->type) {
  case htf::TypeInvalid:
    htf_error("Type is invalid\n");
    break;
  case htf::TypeEvent:
    print_event(current_indent, thread, *t, &e->event_occurence);
    break;
  case htf::TypeSequence: {
    if (show_structure)
      print_sequence(current_indent, thread, *t, &e->sequence_occurence, containing_loop);
    break;
  }
  case htf::TypeLoop: {
    if (show_structure)
      print_loop(current_indent, thread, *t, &e->loop_occurence);
    break;
  }
  }
}

static void display_sequence(htf::ThreadReader* reader,
                             htf::Token token,
                             htf::SequenceOccurence* occurence,
                             int depth) {
  if (occurence) {
    load_savestate(reader, occurence->savestate);
    reader->enterBlock(token);
  }
  auto current_level = reader->readCurrentLevel();
  for (const auto& tokenOccurence : current_level) {
    print_token(reader->thread_trace, tokenOccurence.token, tokenOccurence.occurence, depth,
                &tokenOccurence == &current_level.back());
    if (tokenOccurence.token->type == htf::TypeSequence) {
      display_sequence(reader, *tokenOccurence.token, &tokenOccurence.occurence->sequence_occurence, depth + 1);
    }
    if (tokenOccurence.token->type == htf::TypeLoop) {
      htf::LoopOccurence loop = tokenOccurence.occurence->loop_occurence;
      // The printing of loops is a bit convoluted, because there's no right way to do it
      // Here, we offer four ways to print loops:
      //    - Print only the Sequence inside once, with its mean/median duration
      //    - Print only the Sequence inside once, and also print what's inside of it, with their mean/median duration
      //    - Print the Sequence inside as much time as needed, but don't unroll it
      //    - Print the Sequence inside as much time as needed, and unroll it.
      // So we basically need two booleans: unroll_loops, which we use to determine if we need to print each Sequence
      // inside the loop, and explore_loop_sequences, which we use to determine if we need to print the inside of
      // each Sequence.
      // We'll do each option one after another
      if (!unroll_loops && !explore_loop_sequences) {
        loop.loop_summary = htf::SequenceOccurence();
        loop.loop_summary.sequence = loop.full_loop[0].sequence;
        for (uint j = 0; j < loop.nb_iterations; j++) {
          loop.loop_summary.duration += loop.full_loop[j].duration / loop.nb_iterations;
        }
        // Don't do this at home kids
        print_token(reader->thread_trace, &loop.loop->repeated_token, (htf::Occurence*)&loop.loop_summary, depth + 1,
                    true, &loop);
      }
      if (!unroll_loops && explore_loop_sequences) {
        htf_error("Not implemented yet ! Sorry\n");
      }
      if (unroll_loops) {
        for (uint j = 0; j < loop.nb_iterations; j++) {
          htf::SequenceOccurence* seq = &loop.full_loop[j];
          print_token(reader->thread_trace, &loop.loop->repeated_token, (htf::Occurence*)seq, depth + 1,
                      j == loop.nb_iterations - 1, &loop);
          if (explore_loop_sequences) {
            display_sequence(reader, loop.loop->repeated_token, seq, depth + 2);
          }
        }
      }
    }
  }
  if (occurence) {
    occurence->full_sequence = current_level.data();
  }
  reader->leaveBlock();
}

/* Print all the events of a thread */
static void print_thread(htf::Archive& trace, htf::Thread* thread) {
  printf("Reading events for thread %u (%s):\n", thread->id, thread->getName());
  _print_timestamp_header();
  _print_duration_header();

  printf("Event\n");

  int reader_options = htf::ThreadReaderOptions::None;
  if (show_structure)
    reader_options |= htf::ThreadReaderOptions::ShowStructure;
  if (!store_timestamps || trace.store_timestamps == 0)
    reader_options |= htf::ThreadReaderOptions::NoTimestamps;

  auto* reader = new htf::ThreadReader(&trace, thread->id, reader_options);

  display_sequence(reader, htf::Token(htf::TypeSequence, 0), nullptr, 0);
}

/**
 * Compare the timestamps of the current token on each thread and select the smallest timestamp.
 * @returns Tuple containing the ThreadId and a TokenOccurence.
 *          You are responsible for the memory of the TokenOccurence.
 */
static std::tuple<htf::ThreadId, htf::TokenOccurence> getNextToken(std::vector<htf::ThreadReader>& threadReaders) {
  // Find the earliest threadReader
  htf::ThreadReader* earliestReader = nullptr;
  for (auto& reader : threadReaders) {
    // Check if reader has finished reading its trace
    if (reader.current_frame < 0)
      continue;
    if (!earliestReader || earliestReader->referential_timestamp > reader.referential_timestamp) {
      earliestReader = &reader;
    }
  }

  // If no reader was available
  if (!earliestReader) {
    return {HTF_THREAD_ID_INVALID, {nullptr, nullptr}};
  }

  // Grab the interesting information
  auto& token = earliestReader->getCurToken();
  auto& occurrence = earliestReader->getOccurence(token, earliestReader->tokenCount[token]);
  auto threadId = earliestReader->thread_trace->id;

  // Update the reader
  earliestReader->updateReadCurToken();
  if (token.type == htf::TypeEvent)
    earliestReader->moveToNextToken();

  return {threadId, {&token, &occurrence}};
}

/** Print all the events of all the threads sorted by timestamp*/
void printTrace(htf::Archive& trace) {
  auto readers = std::vector<htf::ThreadReader>();
  int reader_options = htf::ThreadReaderOptions::None;
  //  if (show_structure)
  //    reader_options |= htf::ThreadReaderOptions::ShowStructure;

  for (int i = 0; i < trace.nb_threads; i++) {
    readers.emplace_back(&trace, trace.threads[i]->id, reader_options);
  }

  _print_timestamp_header();
  _print_duration_header();
  printf("Event\n");

  htf::ThreadId threadId;
  htf::TokenOccurence tokenOccurence;
  std::tie(threadId, tokenOccurence) = getNextToken(readers);
  while (threadId != HTF_THREAD_ID_INVALID) {
    auto currentReader = std::find_if(readers.begin(), readers.end(), [&threadId](const htf::ThreadReader& reader) {
      return reader.thread_trace->id == threadId;
    });
    print_token(currentReader->thread_trace, tokenOccurence.token, tokenOccurence.occurence);
    std::tie(threadId, tokenOccurence) = getNextToken(readers);
  }
}

void usage(const char* prog_name) {
  printf("Usage: %s [OPTION] trace_file\n", prog_name);
  printf("\t-T          Print events per thread\n");
  printf("\t-S          Structure mode\n");
  printf("\t-v          Verbose mode\n");
  printf("\t-d          Print duration of events\n");
  printf("\t-t          Don't print timestamps\n");
  printf("\t-u          Unroll loops\n");
  printf("\t-e          Explore sequences inside of loops\n");
  printf("\t-?  -h      Display this help and exit\n");
}

int main(int argc, char** argv) {
  int nb_opts = 0;
  char* trace_name = NULL;

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-v")) {
      htf_debug_level_set(htf::DebugLevel::Debug);
      nb_opts++;
    } else if (!strcmp(argv[i], "-T")) {
      per_thread = true;
      nb_opts++;
    } else if (!strcmp(argv[i], "-S")) {
      show_structure = true;
      nb_opts++;
    } else if (!strcmp(argv[i], "-d")) {
      print_duration = true;
      nb_opts++;
    } else if (!strcmp(argv[i], "-t")) {
      print_timestamp = false;
      nb_opts++;
    } else if (!strcmp(argv[i], "-u")) {
      unroll_loops = true;
      nb_opts++;
    } else if (!strcmp(argv[i], "-e")) {
      explore_loop_sequences = true;
      nb_opts++;
    } else if (!strcmp(argv[i], "--no-timestamps")) {
      setenv("STORE_TIMESTAMPS", "FALSE", 0);
      print_timestamp = true;
      store_timestamps = 0;
      nb_opts++;
    } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-?")) {
      usage(argv[0]);
      return EXIT_SUCCESS;
    } else {
      /* Unknown parameter name. It's probably the trace's path name. We can stop
       * parsing the parameter list.
       */
      break;
    }
  }

  if (!show_structure) {
    unroll_loops = true;
  }

  trace_name = argv[nb_opts + 1];
  if (trace_name == NULL) {
    usage(argv[0]);
    return EXIT_SUCCESS;
  }

  auto trace = htf::Archive();
  htf_read_archive(&trace, trace_name);
  store_timestamps = trace.store_timestamps;

  if (per_thread) {
    for (int i = 0; i < trace.nb_threads; i++) {
      printf("\n");
      print_thread(trace, trace.threads[i]);
    }
  } else {
    printTrace(trace);
  }

  return EXIT_SUCCESS;
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
