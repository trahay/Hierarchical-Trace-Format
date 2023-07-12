#include <stdio.h>
#include <stdlib.h>

#include "OTF2_EvtReader.h"
#include "htf.h"
#include "otf2.h"

OTF2_ErrorCode OTF2_EvtReader_GetLocationID(const OTF2_EvtReader* reader, OTF2_LocationRef* location) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReader_GetPos(OTF2_EvtReader* reader, uint64_t* position) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReader_SetCallbacks(OTF2_EvtReader* reader,
                                           const OTF2_EvtReaderCallbacks* callbacks,
                                           void* userData) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReader_Seek(OTF2_EvtReader* reader, uint64_t position) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReader_ReadEvents(OTF2_EvtReader* reader, uint64_t recordsToRead, uint64_t* recordsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReader_TimeStampRewrite(OTF2_EvtReader* reader, OTF2_TimeStamp time) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReader_ReadEventsBackward(OTF2_EvtReader* reader,
                                                 uint64_t recordsToRead,
                                                 uint64_t* recordsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReader_ApplyMappingTables(OTF2_EvtReader* reader, bool action) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_EvtReader_ApplyClockOffsets(OTF2_EvtReader* reader, bool action) {
  NOT_IMPLEMENTED;
}
