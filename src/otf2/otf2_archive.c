#include <stdlib.h>
#include <stdio.h>

#include "htf.h"
#include "otf2.h"
#include "OTF2_Archive.h"

struct OTF2_Archive_struct {
  struct htf_trace trace;
};

OTF2_Archive*
OTF2_Archive_Open( const char*              archivePath,
                   const char*              archiveName,
                   const OTF2_FileMode      fileMode,
                   const uint64_t           chunkSizeEvents,
                   const uint64_t           chunkSizeDefs,
                   const OTF2_FileSubstrate fileSubstrate,
                   const OTF2_Compression   compression ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_Close( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SwitchFileMode( OTF2_Archive* archive,
                             OTF2_FileMode newFileMode ) {
    NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetDefChunkSize( OTF2_Archive* archive,
                              uint64_t      chunkSize ) {
    NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetMachineName( OTF2_Archive* archive,
                             const char*   machineName ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetDescription( OTF2_Archive* archive,
                             const char*   description ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetCreator( OTF2_Archive* archive,
                         const char*   creator ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetFlushCallbacks( OTF2_Archive*              archive,
                                const OTF2_FlushCallbacks* flushCallbacks,
                                void*                      flushData ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetMemoryCallbacks( OTF2_Archive*               archive,
                                 const OTF2_MemoryCallbacks* memoryCallbacks,
                                 void*                       memoryData ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetCollectiveCallbacks( OTF2_Archive*                   archive,
                                     const OTF2_CollectiveCallbacks* collectiveCallbacks,
                                     void*                           collectiveData,
                                     OTF2_CollectiveContext*         globalCommContext,
                                     OTF2_CollectiveContext*         localCommContext ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetSerialCollectiveCallbacks( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_IsPrimary( OTF2_Archive* archive,
                        bool*         result ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_IsMaster( OTF2_Archive* archive,
                       bool*         result ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetLockingCallbacks( OTF2_Archive*                archive,
                                  const OTF2_LockingCallbacks* lockingCallbacks,
                                  void*                        lockingData ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetHint( OTF2_Archive* archive,
                      OTF2_Hint     hint,
                      void*         value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetProperty( OTF2_Archive* archive,
                          const char*   name,
                          const char*   value,
                          bool          overwrite ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetBoolProperty( OTF2_Archive* archive,
                              const char*   name,
                              bool          value,
                              bool          overwrite ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetPropertyNames( OTF2_Archive* archive,
                               uint32_t*     numberOfProperties,
                               char***       names ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetProperty( OTF2_Archive* archive,
                          const char*   name,
                          char**        value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetTraceId( OTF2_Archive* archive,
                         uint64_t*     id ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetNumberOfLocations( OTF2_Archive* archive,
                                   uint64_t*     numberOfLocations ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetNumberOfGlobalDefinitions( OTF2_Archive* archive,
                                           uint64_t*     numberOfDefinitions ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetMachineName( OTF2_Archive* archive,
                             char**        machineName ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetDescription( OTF2_Archive* archive,
                             char**        description ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetCreator( OTF2_Archive* archive,
                         char**        creator ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetVersion( OTF2_Archive* archive,
                         uint8_t*      major,
                         uint8_t*      minor,
                         uint8_t*      bugfix ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetChunkSize( OTF2_Archive* archive,
                           uint64_t*     chunkSizeEvents,
                           uint64_t*     chunkSizeDefs ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetFileSubstrate( OTF2_Archive*       archive,
                               OTF2_FileSubstrate* substrate ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetCompression( OTF2_Archive*     archive,
                             OTF2_Compression* compression ) {
  NOT_IMPLEMENTED;
}

OTF2_EvtWriter*
OTF2_Archive_GetEvtWriter( OTF2_Archive*    archive,
                           OTF2_LocationRef location ) {
  NOT_IMPLEMENTED;
}

OTF2_DefWriter*
OTF2_Archive_GetDefWriter( OTF2_Archive*    archive,
                           OTF2_LocationRef location ) {
  NOT_IMPLEMENTED;
}

OTF2_GlobalDefWriter*
OTF2_Archive_GetGlobalDefWriter( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_SnapWriter*
OTF2_Archive_GetSnapWriter( OTF2_Archive*    archive,
                            OTF2_LocationRef location ) {
  NOT_IMPLEMENTED;
}

OTF2_ThumbWriter*
OTF2_Archive_GetThumbWriter( OTF2_Archive*      archive,
                             const char*        name,
                             const char*        description,
                             OTF2_ThumbnailType type,
                             uint32_t           numberOfSamples,
                             uint32_t           numberOfMetrics,
                             const uint64_t*    refsToDefs ) {
  NOT_IMPLEMENTED;
}

OTF2_MarkerWriter*
OTF2_Archive_GetMarkerWriter( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_EvtReader*
OTF2_Archive_GetEvtReader( OTF2_Archive*    archive,
                           OTF2_LocationRef location ) {
  NOT_IMPLEMENTED;
}

OTF2_GlobalEvtReader*
OTF2_Archive_GetGlobalEvtReader( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_DefReader*
OTF2_Archive_GetDefReader( OTF2_Archive*    archive,
                           OTF2_LocationRef location ) {
  NOT_IMPLEMENTED;
}

OTF2_GlobalDefReader*
OTF2_Archive_GetGlobalDefReader( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_SnapReader*
OTF2_Archive_GetSnapReader( OTF2_Archive*    archive,
                            OTF2_LocationRef location ) {
  NOT_IMPLEMENTED;
}

OTF2_GlobalSnapReader*
OTF2_Archive_GetGlobalSnapReader( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_ThumbReader*
OTF2_Archive_GetThumbReader( OTF2_Archive* archive,
                             uint32_t      number ) {
  NOT_IMPLEMENTED;
}

OTF2_MarkerReader*
OTF2_Archive_GetMarkerReader( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseEvtWriter( OTF2_Archive*   archive,
                             OTF2_EvtWriter* writer ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseDefWriter( OTF2_Archive*   archive,
                             OTF2_DefWriter* writer ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseMarkerWriter( OTF2_Archive*      archive,
                                OTF2_MarkerWriter* writer ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseSnapWriter( OTF2_Archive*    archive,
                              OTF2_SnapWriter* writer ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseGlobalDefWriter( OTF2_Archive*         archive,
                                   OTF2_GlobalDefWriter* writer ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseEvtReader( OTF2_Archive*   archive,
                             OTF2_EvtReader* reader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseThumbReader( OTF2_Archive*     archive,
                               OTF2_ThumbReader* reader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseGlobalEvtReader( OTF2_Archive*         archive,
                                   OTF2_GlobalEvtReader* globalEvtReader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseDefReader( OTF2_Archive*   archive,
                             OTF2_DefReader* reader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseGlobalDefReader( OTF2_Archive*         archive,
                                   OTF2_GlobalDefReader* globalDefReader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseSnapReader( OTF2_Archive*    archive,
                              OTF2_SnapReader* reader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseGlobalSnapReader( OTF2_Archive*          archive,
                                    OTF2_GlobalSnapReader* globalSnapReader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseMarkerReader( OTF2_Archive*      archive,
                                OTF2_MarkerReader* markerReader ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetNumberOfThumbnails( OTF2_Archive* archive,
                                    uint32_t*     number ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SetNumberOfSnapshots( OTF2_Archive* archive,
                                   uint32_t      number ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_GetNumberOfSnapshots( OTF2_Archive* archive,
                                   uint32_t*     number ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_OpenEvtFiles( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseEvtFiles( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_OpenDefFiles( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseDefFiles( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_OpenSnapFiles( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_CloseSnapFiles( OTF2_Archive* archive ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_Archive_SelectLocation( OTF2_Archive*    archive,
                             OTF2_LocationRef location ) {
  NOT_IMPLEMENTED;
}






