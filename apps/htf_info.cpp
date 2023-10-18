/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */
#include <cstdlib>
#include <cstring>

#include "htf/htf.h"
#include "htf/htf_archive.h"
#include "htf/htf_read.h"
#include "htf/htf_storage.h"

using namespace htf;
void print_sequence(Thread* t, Sequence* s) {
  printf("{");
  for (unsigned i = 0; i < s->size(); i++) {
    Token token = s->tokens[i];
    if (token.type == HTF_TYPE_LOOP) {
      Loop* l = t->getLoop(token);
      token = l->repeated_token;
    }
    printf("%c%x", HTF_TOKEN_TYPE_C(token), token.id);
    if (i < s->size() - 1)
      printf(", ");
  }
  printf("}\n");
}

void info_event(Thread* t, EventSummary* e) {
  htf_print_event(t, &e->event);
  printf("\t{.nb_events: %zu}\n", e->durations->size);
}

void info_sequence(Sequence* s) {
  printf("{.size: %zu}\n", htf_sequence_get_size(s));
}

void info_loop(Loop* l) {
  printf("{.nb_loops: %zu, .repeated_token: %c.%x, .nb_iterations: ", l->nb_iterations.size(),
         HTF_TOKEN_TYPE_C(l->repeated_token), l->repeated_token.id);
  printf("[");
  for (auto& i : l->nb_iterations) {
    printf("%u", i);
    if (&i != &l->nb_iterations.back()) {
      printf(", ");
    }
  }
  printf("]}\n");
}

void info_thread(Thread* t) {
  printf("Thread %x {.archive: %x}\n", t->id, t->archive->id);
  printf("\tEvents {.nb_events: %d, .nb_allocated_events: %d}\n", t->nb_events, t->nb_allocated_events);
  for (unsigned i = 0; i < t->nb_events; i++) {
    printf("\t\tE%x\t", i);
    info_event(t, &t->events[i]);
  }

  printf("\tSequences {.nb_sequences: %d, .nb_allocated_sequences: %d}\n", t->nb_sequences, t->nb_allocated_sequences);
  for (unsigned i = 0; i < t->nb_sequences; i++) {
    printf("\t\tS%x\t", i);
    print_sequence(t, t->sequences[i]);
  }

  printf("\tLoops {.nb_loops: %d, .nb_allocated_loops: %d}\n", t->nb_loops, t->nb_allocated_loops);
  for (unsigned i = 0; i < t->nb_loops; i++) {
    printf("\t\tL%x\t", i);
    info_loop(&t->loops[i]);
  }
}

void info_archive(Archive* archive) {
  printf("Archive %x:\n", archive->id);
  printf("\tdir_name:   %s\n", archive->dir_name);
  printf("\ttrace_name: %s\n", archive->trace_name);
  printf("\tfullpath:   %s\n", archive->fullpath);
  printf("\n");
  printf("\tglobal_archive: %x\n", archive->global_archive ? (int)archive->global_archive->id : -1);

  printf("\tStrings {.nb_strings: %zu } :\n", archive->definitions->strings.size());
  for (auto& string : archive->definitions->strings) {
    printf("\t\t%x: '%s'\n", string.string_ref, string.str);
  }

  printf("\tRegions {.nb_regions: %zu } :\n", archive->definitions->regions.size());
  for (int i = 0; i < archive->definitions->regions.size(); i++) {
    printf("\t\t%x: %x ('%s')\n", archive->definitions->regions[i].region_ref,
           archive->definitions->regions[i].string_ref,
           archive->getString(archive->definitions->regions[i].string_ref)->str);
  }

  printf("\tLocation_groups {.nb_lg: %zu }:\n", archive->location_groups.size());
  for (int i = 0; i < archive->location_groups.size(); i++) {
    printf("\t\t%x: %x ('%s'), parent: %x\n", archive->location_groups[i].id, archive->location_groups[i].name,
           archive->getString(archive->location_groups[i].name)->str, archive->location_groups[i].parent);
  }

  printf("\tLocations {.nb_loc: %zu }:\n", archive->locations.size());
  for (int i = 0; i < archive->locations.size(); i++) {
    printf("\t\t%x: %x ('%s'), parent: %x\n", archive->locations[i].id, archive->locations[i].name,
           archive->getString(archive->locations[i].name)->str, archive->locations[i].parent);
  }

  printf("\tThreads {.nb_threads: %d, .nb_allocated_threads: %d}:\n", archive->nb_threads,
         archive->nb_allocated_threads);

  if (archive->threads) {
    for (int i = 0; i < archive->nb_threads; i++) {
      if (archive->threads[i]) {
        printf("\t\t%x: {.archive=%x, .nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n", archive->threads[i]->id,
               archive->threads[i]->archive->id, archive->threads[i]->nb_events, archive->threads[i]->nb_sequences,
               archive->threads[i]->nb_loops);
      }
    }
  }

  printf("\tArchives {.nb_archives: %d}\n", archive->nb_archives);
}

void info_trace(Archive* trace) {
  info_archive(trace);
  for (int i = 0; i < trace->nb_archives; i++) {
    info_archive(trace->archive_list[i]);
  }

  for (int i = 0; i < trace->nb_threads; i++) {
    info_thread(trace->threads[i]);
  }
}

void usage(const char* prog_name) {
  printf("Usage: %s [OPTION] trace_file\n", prog_name);
  printf("\t-v          Verbose mode\n");
  printf("\t-?  -h      Display this help and exit\n");
}

int main(int argc, char** argv) {
  int nb_opts = 0;
  char* trace_name = NULL;

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-v")) {
      htf_debug_level_set(Debug);
      nb_opts++;
    } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-?")) {
      usage(argv[0]);
      return EXIT_SUCCESS;
    } else {
      /* Unknown parameter name. It's probably the program name. We can stop
       * parsing the parameter list.
       */
      break;
    }
  }

  trace_name = argv[nb_opts + 1];
  if (trace_name == NULL) {
    usage(argv[0]);
    return EXIT_SUCCESS;
  }

  Archive trace = Archive();
  htf_read_archive(&trace, trace_name);
  info_trace(&trace);

  return EXIT_SUCCESS;
}

/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
