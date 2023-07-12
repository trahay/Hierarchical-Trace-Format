#include <stdio.h>
#include <stdlib.h>

#include "OTF2_MarkerWriter.h"
#include "htf.h"
#include "otf2.h"

OTF2_ErrorCode OTF2_MarkerWriter_WriteDefMarker(OTF2_MarkerWriter* writerHandle,
                                                OTF2_MarkerRef self,
                                                const char* markerGroup,
                                                const char* markerCategory,
                                                OTF2_MarkerSeverity severity) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_MarkerWriter_WriteMarker(OTF2_MarkerWriter* writerHandle,
                                             OTF2_TimeStamp timestamp,
                                             OTF2_TimeStamp duration,
                                             OTF2_MarkerRef marker,
                                             OTF2_MarkerScope scope,
                                             uint64_t scopeRef,
                                             const char* text) {
  NOT_IMPLEMENTED;
}
