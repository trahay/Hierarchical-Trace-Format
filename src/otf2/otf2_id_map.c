#include <stdlib.h>
#include <stdio.h>

#include "htf.h"
#include "otf2.h"
#include "OTF2_IdMap.h"

OTF2_IdMap*
OTF2_IdMap_Create( OTF2_IdMapMode mode,
                   uint64_t       capacity ) {
  NOT_IMPLEMENTED;
}

OTF2_IdMap*
OTF2_IdMap_CreateFromUint64Array( uint64_t        length,
                                  const uint64_t* mappings,
                                  bool            optimizeSize ) {
  NOT_IMPLEMENTED;
}

OTF2_IdMap*
OTF2_IdMap_CreateFromUint32Array( uint64_t        length,
                                  const uint32_t* mappings,
                                  bool            optimizeSize ) {
  NOT_IMPLEMENTED;
}

void
OTF2_IdMap_Free( OTF2_IdMap* instance ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_IdMap_GetSize( const OTF2_IdMap* instance,
                    uint64_t*         size ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_IdMap_GetMode( const OTF2_IdMap* instance,
                    OTF2_IdMapMode*   mode ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_IdMap_Clear( OTF2_IdMap* instance ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_IdMap_AddIdPair( OTF2_IdMap* instance,
                      uint64_t    localId,
                      uint64_t    globalId ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_IdMap_GetGlobalId( const OTF2_IdMap* instance,
                        uint64_t          localId,
                        uint64_t*         globalId ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_IdMap_GetGlobalIdSave( const OTF2_IdMap* instance,
                            uint64_t          localId,
                            uint64_t*         globalId ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_IdMap_Traverse( const OTF2_IdMap*           instance,
                     OTF2_IdMap_TraverseCallback callback,
                     void*                       userData ) {
  NOT_IMPLEMENTED;
}


