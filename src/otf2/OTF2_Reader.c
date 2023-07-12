#include <stdio.h>
#include <stdlib.h>

#include "OTF2_Reader.h"
#include "htf.h"
#include "otf2.h"

OTF2_Reader* OTF2_Reader_Open(const char* anchorFilePath) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_Close(OTF2_Reader* reader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_SetHint(OTF2_Reader* reader, OTF2_Hint hint, void* value) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_SetCollectiveCallbacks(OTF2_Reader* reader,
                                                  const OTF2_CollectiveCallbacks* collectiveCallbacks,
                                                  void* collectiveData,
                                                  OTF2_CollectiveContext* globalCommContext,
                                                  OTF2_CollectiveContext* localCommContext) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_SetSerialCollectiveCallbacks(OTF2_Reader* reader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_SetLockingCallbacks(OTF2_Reader* reader,
                                               const OTF2_LockingCallbacks* lockingCallbacks,
                                               void* lockingData) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_RegisterEvtCallbacks(OTF2_Reader* reader,
                                                OTF2_EvtReader* evtReader,
                                                const OTF2_EvtReaderCallbacks* callbacks,
                                                void* userData) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_RegisterGlobalEvtCallbacks(OTF2_Reader* reader,
                                                      OTF2_GlobalEvtReader* evtReader,
                                                      const OTF2_GlobalEvtReaderCallbacks* callbacks,
                                                      void* userData) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_RegisterDefCallbacks(OTF2_Reader* reader,
                                                OTF2_DefReader* defReader,
                                                const OTF2_DefReaderCallbacks* callbacks,
                                                void* userData) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_RegisterGlobalDefCallbacks(OTF2_Reader* reader,
                                                      OTF2_GlobalDefReader* defReader,
                                                      const OTF2_GlobalDefReaderCallbacks* callbacks,
                                                      void* userData) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_RegisterSnapCallbacks(OTF2_Reader* reader,
                                                 OTF2_SnapReader* snapReader,
                                                 const OTF2_SnapReaderCallbacks* callbacks,
                                                 void* userData) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_RegisterGlobalSnapCallbacks(OTF2_Reader* reader,
                                                       OTF2_GlobalSnapReader* evtReader,
                                                       const OTF2_GlobalSnapReaderCallbacks* callbacks,
                                                       void* userData) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_RegisterMarkerCallbacks(OTF2_Reader* reader,
                                                   OTF2_MarkerReader* markerReader,
                                                   const OTF2_MarkerReaderCallbacks* callbacks,
                                                   void* userData) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadLocalEvents(OTF2_Reader* reader,
                                           OTF2_EvtReader* evtReader,
                                           uint64_t eventsToRead,
                                           uint64_t* eventsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadAllLocalEvents(OTF2_Reader* reader, OTF2_EvtReader* evtReader, uint64_t* eventsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadLocalEventsBackward(OTF2_Reader* reader,
                                                   OTF2_EvtReader* evtReader,
                                                   uint64_t eventsToRead,
                                                   uint64_t* eventsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadGlobalEvent(OTF2_Reader* reader, OTF2_GlobalEvtReader* evtReader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_HasGlobalEvent(OTF2_Reader* reader, OTF2_GlobalEvtReader* evtReader, int* flag) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadGlobalEvents(OTF2_Reader* reader,
                                            OTF2_GlobalEvtReader* evtReader,
                                            uint64_t eventsToRead,
                                            uint64_t* eventsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadAllGlobalEvents(OTF2_Reader* reader,
                                               OTF2_GlobalEvtReader* evtReader,
                                               uint64_t* eventsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadLocalDefinitions(OTF2_Reader* reader,
                                                OTF2_DefReader* defReader,
                                                uint64_t definitionsToRead,
                                                uint64_t* definitionsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadAllLocalDefinitions(OTF2_Reader* reader,
                                                   OTF2_DefReader* defReader,
                                                   uint64_t* definitionsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadGlobalDefinitions(OTF2_Reader* reader,
                                                 OTF2_GlobalDefReader* defReader,
                                                 uint64_t definitionsToRead,
                                                 uint64_t* definitionsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadAllGlobalDefinitions(OTF2_Reader* reader,
                                                    OTF2_GlobalDefReader* defReader,
                                                    uint64_t* definitionsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadLocalSnapshots(OTF2_Reader* reader,
                                              OTF2_SnapReader* snapReader,
                                              uint64_t recordsToRead,
                                              uint64_t* recordsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadAllLocalSnapshots(OTF2_Reader* reader,
                                                 OTF2_SnapReader* snapReader,
                                                 uint64_t* recordsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadGlobalSnapshots(OTF2_Reader* reader,
                                               OTF2_GlobalSnapReader* snapReader,
                                               uint64_t recordsToRead,
                                               uint64_t* recordsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadAllGlobalSnapshots(OTF2_Reader* reader,
                                                  OTF2_GlobalSnapReader* snapReader,
                                                  uint64_t* recordsRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadMarkers(OTF2_Reader* reader,
                                       OTF2_MarkerReader* markerReader,
                                       uint64_t markersToRead,
                                       uint64_t* markersRead) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_ReadAllMarkers(OTF2_Reader* reader, OTF2_MarkerReader* markerReader, uint64_t* markersRead) {
  NOT_IMPLEMENTED;
}

OTF2_EvtReader* OTF2_Reader_GetEvtReader(OTF2_Reader* reader, OTF2_LocationRef location) {
  NOT_IMPLEMENTED;
}

OTF2_GlobalEvtReader* OTF2_Reader_GetGlobalEvtReader(OTF2_Reader* reader) {
  NOT_IMPLEMENTED;
}

OTF2_DefReader* OTF2_Reader_GetDefReader(OTF2_Reader* reader, OTF2_LocationRef location) {
  NOT_IMPLEMENTED;
}

OTF2_GlobalDefReader* OTF2_Reader_GetGlobalDefReader(OTF2_Reader* reader) {
  NOT_IMPLEMENTED;
}

OTF2_SnapReader* OTF2_Reader_GetSnapReader(OTF2_Reader* reader, OTF2_LocationRef location) {
  NOT_IMPLEMENTED;
}

OTF2_GlobalSnapReader* OTF2_Reader_GetGlobalSnapReader(OTF2_Reader* reader) {
  NOT_IMPLEMENTED;
}

OTF2_ThumbReader* OTF2_Reader_GetThumbReader(OTF2_Reader* reader, uint32_t number) {
  NOT_IMPLEMENTED;
}

OTF2_MarkerReader* OTF2_Reader_GetMarkerReader(OTF2_Reader* reader) {
  NOT_IMPLEMENTED;
}

OTF2_MarkerWriter* OTF2_Reader_GetMarkerWriter(OTF2_Reader* reader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_CloseEvtReader(OTF2_Reader* reader, OTF2_EvtReader* evtReader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_CloseGlobalEvtReader(OTF2_Reader* reader, OTF2_GlobalEvtReader* globalEvtReader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_CloseDefReader(OTF2_Reader* reader, OTF2_DefReader* defReader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_CloseGlobalDefReader(OTF2_Reader* reader, OTF2_GlobalDefReader* globalDefReader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_CloseSnapReader(OTF2_Reader* reader, OTF2_SnapReader* snapReader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_CloseGlobalSnapReader(OTF2_Reader* reader, OTF2_GlobalSnapReader* globalSnapReader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_CloseThumbReader(OTF2_Reader* reader, OTF2_ThumbReader* thumbReader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_CloseMarkerReader(OTF2_Reader* reader, OTF2_MarkerReader* markerReader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_CloseMarkerWriter(OTF2_Reader* reader, OTF2_MarkerWriter* markerWriter) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetVersion(OTF2_Reader* reader, uint8_t* major, uint8_t* minor, uint8_t* bugfix) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetChunkSize(OTF2_Reader* reader,
                                        uint64_t* chunkSizeEvents,
                                        uint64_t* chunkSizeDefinitions) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetFileSubstrate(OTF2_Reader* reader, OTF2_FileSubstrate* substrate) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetCompression(OTF2_Reader* reader, OTF2_Compression* compression) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetNumberOfLocations(OTF2_Reader* reader, uint64_t* numberOfLocations) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetNumberOfGlobalDefinitions(OTF2_Reader* reader, uint64_t* numberOfDefinitions) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetMachineName(OTF2_Reader* reader, char** machineName) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetCreator(OTF2_Reader* reader, char** creator) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetDescription(OTF2_Reader* reader, char** description) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetPropertyNames(OTF2_Reader* reader, uint32_t* numberOfProperties, char*** names) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetProperty(OTF2_Reader* reader, const char* name, char** value) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetBoolProperty(OTF2_Reader* reader, const char* name, bool* value) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetTraceId(OTF2_Reader* reader, uint64_t* id) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetNumberOfSnapshots(OTF2_Reader* reader, uint32_t* number) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_GetNumberOfThumbnails(OTF2_Reader* reader, uint32_t* number) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_OpenEvtFiles(OTF2_Reader* reader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_CloseEvtFiles(OTF2_Reader* reader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_OpenDefFiles(OTF2_Reader* reader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_CloseDefFiles(OTF2_Reader* reader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_OpenSnapFiles(OTF2_Reader* reader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_CloseSnapFiles(OTF2_Reader* reader) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_SelectLocation(OTF2_Reader* reader, OTF2_LocationRef location) {
  NOT_IMPLEMENTED;
}

/** @brief Determines if this reader is the primary archive which handles the
 *  global archive state (anchor file, global definitions, marker, ...).
 *
 *  @param reader       Reader handle.
 *  @param[out] result  Storage for the result.
 *
 *  @return @eref{OTF2_SUCCESS} if successful, an error code if an error occurs.
 */
OTF2_ErrorCode OTF2_Reader_IsPrimary(OTF2_Reader* reader, bool* result) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_Reader_IsMaster(OTF2_Reader* reader, bool* result) {
  NOT_IMPLEMENTED;
}
