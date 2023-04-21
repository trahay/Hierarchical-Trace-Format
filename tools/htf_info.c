#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

#include "htf.h"
#include "htf_read.h"
#include "htf_archive.h"

void print_sequence(struct htf_thread* t, struct htf_sequence *s, int id) {
  printf("S#%x\t{", id);
  for(unsigned i=0; i<s->size; i++) {
    htf_token_t* token = &s->token[i];
    if(HTF_TOKEN_TYPE(*token) == HTF_TYPE_LOOP) {
      struct htf_loop* l = htf_get_loop(t, HTF_LOOP_ID(token->id));
      printf("%d*", l->nb_iterations);
      token = &l->token;
    }
    printf("%c%x",  HTF_TOKEN_TYPE_C(*token), HTF_TOKEN_ID(*token));
    if(i<s->size - 1)
      printf(", ");
  }
  printf("}\n");
}

void info_event(struct htf_thread *t, struct htf_event_summary *e) {
  htf_print_event(t, &e->event);
  printf("\t{.nb_timestamps: %d, .nb_allocated_timestamps: %d}\n", e->nb_timestamps, e->nb_allocated_timestamps);
}


void info_sequence(struct htf_sequence *s) {
  printf("\t{.size: %d, .allocated: %d}\n", s->size, s->allocated);
}

void info_loop(struct htf_loop *l) {
  printf("\t{.nb_iterations: %d, .token: %x.%x}\n",
	 l->nb_iterations,
	 HTF_TOKEN_TYPE(l->token),
	 HTF_TOKEN_ID(l->token));
}

void info_thread(struct htf_thread *t) {
  printf("Thread %x {.archive: %x}\n", t->id, t->archive->id);
  printf("\tEvents {.nb_events: %d, .nb_allocated_events: %d}\n", t->nb_events, t->nb_allocated_events);
  for(unsigned i=0; i<t->nb_events; i++) {
    printf("\t\t");
    info_event(t, &t->events[i]);
  } 

  printf("\tSequences {.nb_sequences: %d, .nb_allocated_sequences: %d}\n", t->nb_sequences, t->nb_allocated_sequences);
  for(unsigned i=0; i<t->nb_sequences; i++) {
    printf("\t\t");
    print_sequence(t, &t->sequences[i], i);
  } 

  printf("\tLoops {.nb_loops: %d, .nb_allocated_loops: %d}\n", t->nb_loops, t->nb_allocated_loops);
  for(unsigned i=0; i<t->nb_loops; i++) {
    printf("\t\t");
    info_loop(&t->loops[i]);
  } 
}

void info_archive(struct htf_archive *archive) {
  printf("Archive %x:\n", archive->id);
  printf("\tdir_name:   %s\n", archive->dir_name);
  printf("\ttrace_name: %s\n", archive->trace_name);
  printf("\tfullpath:   %s\n", archive->fullpath);
  printf("\n");
  printf("\tglobal_archive: %x\n", archive->global_archive ? (int)archive->global_archive->id : -1);

  printf("\tStrings {.nb_strings: %d, .nb_allocated_strings: %d} :\n",
	 archive->definitions.nb_strings, archive->definitions.nb_allocated_strings);
  for(int i=0; i<archive->definitions.nb_strings; i++) {
    printf("\t\t%x: '%s'\n", archive->definitions.strings[i].string_ref, archive->definitions.strings[i].str);
  }

  printf("\tRegions {.nb_regions: %d, .nb_allocated_regions: %d} :\n",
	 archive->definitions.nb_regions, archive->definitions.nb_allocated_regions);
  for(int i=0; i<archive->definitions.nb_regions; i++) {
    printf("\t\t%x: %x ('%s')\n", archive->definitions.regions[i].region_ref, archive->definitions.regions[i].string_ref, htf_archive_get_string(archive, archive->definitions.regions[i].string_ref)->str);
  }

  printf("\tLocation_groups {.nb_lg: %d, .nb_allocated_lg: %d}:\n",
	 archive->nb_location_groups, archive->nb_allocated_location_groups);
  for(int i=0; i<archive->nb_location_groups; i++) {
    printf("\t\t%x: %x ('%s'), parent: %x\n", archive->location_groups[i].id,
	   archive->location_groups[i].name,
	   htf_archive_get_string(archive, archive->location_groups[i].name)->str,
	   archive->location_groups[i].parent);
  }

  printf("\tLocations {.nb_loc: %d, .nb_allocated_loc: %d}:\n",
	 archive->nb_locations, archive->nb_allocated_locations);
  for(int i=0; i<archive->nb_locations; i++) {
    printf("\t\t%x: %x ('%s'), parent: %x\n", archive->locations[i].id,
	   archive->locations[i].name,
	   htf_archive_get_string(archive, archive->locations[i].name)->str,
	   archive->locations[i].parent);
  }

  printf("\tThreads {.nb_threads: %d, .nb_allocated_threads: %d}:\n",
	 archive->nb_threads, archive->nb_allocated_threads);

  if(archive->threads) {
    for(int i=0; i<archive->nb_threads; i++) {
      if(archive->threads[i]) {
	printf("\t\t%x: {.archive=%x, .nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n",
	       archive->threads[i]->id,
	       archive->threads[i]->archive->id,
	       archive->threads[i]->nb_events,
	       archive->threads[i]->nb_sequences,
	       archive->threads[i]->nb_loops);
      }
    }
  }

  printf("\tArchives {.nb_archives: %d}\n",
	 archive->nb_archives);
}

void info_trace(struct htf_archive *trace) {
  info_archive(trace);
  for(int i=0; i<trace->nb_archives; i++) {
    info_archive(trace->archive_list[i]);
  }

  for(int i=0; i<trace->nb_threads; i++) {
    info_thread(trace->threads[i]);
  }
}

void usage(const char *prog_name) {
  printf("Usage: %s [OPTION] trace_file\n", prog_name);
  printf("\t-v          Verbose mode\n");
  printf("\t-?  -h      Display this help and exit\n");
}

int main(int argc, char**argv) {
  int nb_opts = 0;
  char* trace_name = NULL;

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-v")) {
      htf_debug_level_set(htf_dbg_lvl_debug);
      nb_opts++;
    } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-?") ) {
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

  struct htf_archive trace;
  htf_read_archive(&trace, trace_name);
  info_trace(&trace);

  return EXIT_SUCCESS;
}
