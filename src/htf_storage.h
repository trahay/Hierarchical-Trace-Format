#ifndef HTF_STORAGE_H
#define HTF_STORAGE_H

#include "htf.h"

void htf_storage_init(struct htf_archive* archive);
void htf_storage_finalize_thread(struct htf_thread* thread);
void htf_storage_finalize(struct htf_archive* archive);
void htf_storage_finalize_global(struct htf_archive* archive);


char* htf_archive_fullpath(char* dir_name, char* trace_name);
#define ZSTD_COMPRESSION_LEVEL 1
#endif	/* HTF_STORAGE_H */
