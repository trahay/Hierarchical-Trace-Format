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


void info_archive(struct htf_archive *archive) {
  printf("Archive %x:\n", archive->id);
  printf("\tdir_name:   %s\n", archive->dir_name);
  printf("\ttrace_name: %s\n", archive->trace_name);
  printf("\tfullpath:   %s\n", archive->fullpath);
  printf("\n");
  printf("\tglobal_archive: %x\n", archive->global_archive ? archive->global_archive->id : -1);

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

  for(int i=0; i<archive->nb_threads; i++) {
    printf("\t\t%x: {.archive=%x, .nb_events=%d, .nb_sequences=%d, .nb_loops=%d}\n",
	   archive->threads[i]->id,
	   archive->threads[i]->archive->id,
	   archive->threads[i]->nb_events,
	   archive->threads[i]->nb_sequences,
	   archive->threads[i]->nb_loops);
  }

  printf("\tArchives {.nb_archives: %d}\n",
	 archive->nb_archives);
}

void info_trace(struct htf_archive *trace) {
  info_archive(trace);
  for(int i=0; i<trace->nb_archives; i++) {
  info_archive(trace->archive_list[i]);
  }
}

void usage(const char *prog_name) {
  printf("Usage: %s [OPTION] trace_file\n", prog_name);
  printf("\t-T          Print events per thread\n");
  printf("\t-v          Verbose mode\n");
  printf("\t-?  -h      Display this help and exit\n");
}

int main(int argc, char**argv) {
  int per_thread = 0;
  int nb_opts = 0;
  char* trace_name = NULL;

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-v")) {
      htf_debug_level_set(htf_dbg_lvl_debug);
      nb_opts++;
    } else if (!strcmp(argv[i], "-T")) {
      per_thread = 1;
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
