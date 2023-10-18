#include <stdio.h>
#include <stdlib.h>

#include "htf/htf.h"
#include "otf2/OTF2_GlobalSnapReader.h"
#include "otf2/otf2.h"

OTF2_ErrorCode OTF2_GlobalSnapReader_SetCallbacks(OTF2_GlobalSnapReader* reader,
                                                  const OTF2_GlobalSnapReaderCallbacks* callbacks,
                                                  void* userData) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_GlobalSnapReader_ReadSnapshots(OTF2_GlobalSnapReader* reader,
                                                   uint64_t recordsToRead,
                                                   uint64_t* recordsRead) {
  NOT_IMPLEMENTED;
}
