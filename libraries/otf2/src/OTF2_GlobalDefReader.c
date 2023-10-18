#include <stdio.h>
#include <stdlib.h>

#include "htf/htf.h"
#include "otf2/OTF2_GlobalDefReader.h"
#include "otf2/otf2.h"

OTF2_ErrorCode OTF2_GlobalDefReader_SetCallbacks(OTF2_GlobalDefReader* reader,
                                                 const OTF2_GlobalDefReaderCallbacks* callbacks,
                                                 void* userData) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_GlobalDefReader_ReadDefinitions(OTF2_GlobalDefReader* reader,
                                                    uint64_t recordsToRead,
                                                    uint64_t* recordsRead) {
  NOT_IMPLEMENTED;
}
