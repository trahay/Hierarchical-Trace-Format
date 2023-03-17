#include <stdlib.h>
#include <stdio.h>

#include "htf.h"
#include "otf2.h"
#include "OTF2_EvtWriter.h"

OTF2_ErrorCode
OTF2_EvtWriter_GetLocationID( const OTF2_EvtWriter* writer,
                              OTF2_LocationRef*     locationID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_SetUserData( OTF2_EvtWriter* writer,
                            void*           userData ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_GetUserData( const OTF2_EvtWriter* writer,
                            void**                userData ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_SetLocationID( OTF2_EvtWriter*  writer,
                              OTF2_LocationRef location ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_GetNumberOfEvents( OTF2_EvtWriter* writer,
                                  uint64_t*       numberOfEvents ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_BufferFlush( OTF2_EvtWriter*     writer,
                            OTF2_AttributeList* attributeList,
                            OTF2_TimeStamp      time,
                            OTF2_TimeStamp      stopTime ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_MeasurementOnOff( OTF2_EvtWriter*      writer,
                                 OTF2_AttributeList*  attributeList,
                                 OTF2_TimeStamp       time,
                                 OTF2_MeasurementMode measurementMode ) {
   NOT_IMPLEMENTED;
     }

OTF2_ErrorCode
OTF2_EvtWriter_Enter( OTF2_EvtWriter*     writer,
                      OTF2_AttributeList* attributeList,
                      OTF2_TimeStamp      time,
                      OTF2_RegionRef      region ) {

  printf("enter(%p {.locationRef=%x, .writer=%p}, %d)\n", writer, writer->locationRef, writer->thread_writer, region);
  htf_assert(writer->locationRef < 10);
  htf_record_enter(writer->thread_writer,
		   NULL, // TO_BE_IMPLEMENTED
		   time,
		   region);

  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_EvtWriter_Leave( OTF2_EvtWriter*     writer,
                      OTF2_AttributeList* attributeList,
                      OTF2_TimeStamp      time,
                      OTF2_RegionRef      region ) {
  printf("leave(%p {.locationRef=%x, .writer=%p}, %d)\n", writer, writer->locationRef, writer->thread_writer, region);
 htf_assert(writer->locationRef < 10);
 htf_record_leave(writer->thread_writer,
		   NULL, // TO_BE_IMPLEMENTED
		   time,
		   region);
  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_EvtWriter_MpiSend( OTF2_EvtWriter*     writer,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time,
                        uint32_t            receiver,
                        OTF2_CommRef        communicator,
                        uint32_t            msgTag,
                        uint64_t            msgLength ) {
   NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_MpiIsend( OTF2_EvtWriter*     writer,
                         OTF2_AttributeList* attributeList,
                         OTF2_TimeStamp      time,
                         uint32_t            receiver,
                         OTF2_CommRef        communicator,
                         uint32_t            msgTag,
                         uint64_t            msgLength,
                         uint64_t            requestID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_MpiIsendComplete( OTF2_EvtWriter*     writer,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 uint64_t            requestID ) {
  NOT_IMPLEMENTED;
}


OTF2_ErrorCode
OTF2_EvtWriter_MpiIrecvRequest( OTF2_EvtWriter*     writer,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                uint64_t            requestID ) {
   NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_MpiRecv( OTF2_EvtWriter*     writer,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time,
                        uint32_t            sender,
                        OTF2_CommRef        communicator,
                        uint32_t            msgTag,
                        uint64_t            msgLength ) {
   NOT_IMPLEMENTED;
}
OTF2_ErrorCode
OTF2_EvtWriter_MpiIrecv( OTF2_EvtWriter*     writer,
                         OTF2_AttributeList* attributeList,
                         OTF2_TimeStamp      time,
                         uint32_t            sender,
                         OTF2_CommRef        communicator,
                         uint32_t            msgTag,
                         uint64_t            msgLength,
                         uint64_t            requestID ) {
   NOT_IMPLEMENTED;
     }

OTF2_ErrorCode
OTF2_EvtWriter_MpiRequestTest( OTF2_EvtWriter*     writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               uint64_t            requestID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_MpiRequestCancelled( OTF2_EvtWriter*     writer,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_TimeStamp      time,
                                    uint64_t            requestID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_MpiCollectiveBegin( OTF2_EvtWriter*     writer,
                                   OTF2_AttributeList* attributeList,
                                   OTF2_TimeStamp      time ) {
   NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_MpiCollectiveEnd( OTF2_EvtWriter*     writer,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 OTF2_CollectiveOp   collectiveOp,
                                 OTF2_CommRef        communicator,
                                 uint32_t            root,
                                 uint64_t            sizeSent,
                                 uint64_t            sizeReceived ) {
   NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_OmpFork( OTF2_EvtWriter*     writer,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time,
                        uint32_t            numberOfRequestedThreads ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_OmpJoin( OTF2_EvtWriter*     writer,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_OmpAcquireLock( OTF2_EvtWriter*     writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               uint32_t            lockID,
                               uint32_t            acquisitionOrder ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_OmpReleaseLock( OTF2_EvtWriter*     writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               uint32_t            lockID,
                               uint32_t            acquisitionOrder ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_OmpTaskCreate( OTF2_EvtWriter*     writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              uint64_t            taskID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_OmpTaskSwitch( OTF2_EvtWriter*     writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              uint64_t            taskID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_OmpTaskComplete( OTF2_EvtWriter*     writer,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                uint64_t            taskID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_Metric( OTF2_EvtWriter*         writer,
                       OTF2_AttributeList*     attributeList,
                       OTF2_TimeStamp          time,
                       OTF2_MetricRef          metric,
                       uint8_t                 numberOfMetrics,
                       const OTF2_Type*        typeIDs,
                       const OTF2_MetricValue* metricValues ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ParameterString( OTF2_EvtWriter*     writer,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                OTF2_ParameterRef   parameter,
                                OTF2_StringRef      string ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ParameterInt( OTF2_EvtWriter*     writer,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_ParameterRef   parameter,
                             int64_t             value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ParameterUnsignedInt( OTF2_EvtWriter*     writer,
                                     OTF2_AttributeList* attributeList,
                                     OTF2_TimeStamp      time,
                                     OTF2_ParameterRef   parameter,
                                     uint64_t            value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaWinCreate( OTF2_EvtWriter*     writer,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_RmaWinRef      win ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaWinDestroy( OTF2_EvtWriter*     writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_RmaWinRef      win ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaCollectiveBegin( OTF2_EvtWriter*     writer,
                                   OTF2_AttributeList* attributeList,
                                   OTF2_TimeStamp      time ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaCollectiveEnd( OTF2_EvtWriter*     writer,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 OTF2_CollectiveOp   collectiveOp,
                                 OTF2_RmaSyncLevel   syncLevel,
                                 OTF2_RmaWinRef      win,
                                 uint32_t            root,
                                 uint64_t            bytesSent,
                                 uint64_t            bytesReceived ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaGroupSync( OTF2_EvtWriter*     writer,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_RmaSyncLevel   syncLevel,
                             OTF2_RmaWinRef      win,
                             OTF2_GroupRef       group ) {
  NOT_IMPLEMENTED;
}


OTF2_ErrorCode
OTF2_EvtWriter_RmaRequestLock( OTF2_EvtWriter*     writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               OTF2_RmaWinRef      win,
                               uint32_t            remote,
                               uint64_t            lockId,
                               OTF2_LockType       lockType ) {
   NOT_IMPLEMENTED;
}
 
OTF2_ErrorCode
OTF2_EvtWriter_RmaAcquireLock( OTF2_EvtWriter*     writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               OTF2_RmaWinRef      win,
                               uint32_t            remote,
                               uint64_t            lockId,
                               OTF2_LockType       lockType ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaTryLock( OTF2_EvtWriter*     writer,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           OTF2_RmaWinRef      win,
                           uint32_t            remote,
                           uint64_t            lockId,
                           OTF2_LockType       lockType ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaReleaseLock( OTF2_EvtWriter*     writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               OTF2_RmaWinRef      win,
                               uint32_t            remote,
                               uint64_t            lockId ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaSync( OTF2_EvtWriter*     writer,
                        OTF2_AttributeList* attributeList,
                        OTF2_TimeStamp      time,
                        OTF2_RmaWinRef      win,
                        uint32_t            remote,
                        OTF2_RmaSyncType    syncType ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaWaitChange( OTF2_EvtWriter*     writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_RmaWinRef      win ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaPut( OTF2_EvtWriter*     writer,
                       OTF2_AttributeList* attributeList,
                       OTF2_TimeStamp      time,
                       OTF2_RmaWinRef      win,
                       uint32_t            remote,
                       uint64_t            bytes,
                       uint64_t            matchingId ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaGet( OTF2_EvtWriter*     writer,
                       OTF2_AttributeList* attributeList,
                       OTF2_TimeStamp      time,
                       OTF2_RmaWinRef      win,
                       uint32_t            remote,
                       uint64_t            bytes,
                       uint64_t            matchingId ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaAtomic( OTF2_EvtWriter*     writer,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      time,
                          OTF2_RmaWinRef      win,
                          uint32_t            remote,
                          OTF2_RmaAtomicType  type,
                          uint64_t            bytesSent,
                          uint64_t            bytesReceived,
                          uint64_t            matchingId ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaOpCompleteBlocking( OTF2_EvtWriter*     writer,
                                      OTF2_AttributeList* attributeList,
                                      OTF2_TimeStamp      time,
                                      OTF2_RmaWinRef      win,
                                      uint64_t            matchingId ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaOpCompleteNonBlocking( OTF2_EvtWriter*     writer,
                                         OTF2_AttributeList* attributeList,
                                         OTF2_TimeStamp      time,
                                         OTF2_RmaWinRef      win,
                                         uint64_t            matchingId ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaOpTest( OTF2_EvtWriter*     writer,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      time,
                          OTF2_RmaWinRef      win,
                          uint64_t            matchingId ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_RmaOpCompleteRemote( OTF2_EvtWriter*     writer,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_TimeStamp      time,
                                    OTF2_RmaWinRef      win,
                                    uint64_t            matchingId ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ThreadFork( OTF2_EvtWriter*     writer,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           OTF2_Paradigm       model,
                           uint32_t            numberOfRequestedThreads ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ThreadJoin( OTF2_EvtWriter*     writer,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           OTF2_Paradigm       model ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ThreadTeamBegin( OTF2_EvtWriter*     writer,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                OTF2_CommRef        threadTeam ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ThreadTeamEnd( OTF2_EvtWriter*     writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_CommRef        threadTeam ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ThreadAcquireLock( OTF2_EvtWriter*     writer,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      time,
                                  OTF2_Paradigm       model,
                                  uint32_t            lockID,
                                  uint32_t            acquisitionOrder ) {
   NOT_IMPLEMENTED;
}


OTF2_ErrorCode
OTF2_EvtWriter_ThreadReleaseLock( OTF2_EvtWriter*     writer,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      time,
                                  OTF2_Paradigm       model,
                                  uint32_t            lockID,
                                  uint32_t            acquisitionOrder ) {
   NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ThreadTaskCreate( OTF2_EvtWriter*     writer,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 OTF2_CommRef        threadTeam,
                                 uint32_t            creatingThread,
                                 uint32_t            generationNumber ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ThreadTaskSwitch( OTF2_EvtWriter*     writer,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      time,
                                 OTF2_CommRef        threadTeam,
                                 uint32_t            creatingThread,
                                 uint32_t            generationNumber ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ThreadTaskComplete( OTF2_EvtWriter*     writer,
                                   OTF2_AttributeList* attributeList,
                                   OTF2_TimeStamp      time,
                                   OTF2_CommRef        threadTeam,
                                   uint32_t            creatingThread,
                                   uint32_t            generationNumber ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ThreadCreate( OTF2_EvtWriter*     writer,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_CommRef        threadContingent,
                             uint64_t            sequenceCount ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ThreadBegin( OTF2_EvtWriter*     writer,
                            OTF2_AttributeList* attributeList,
                            OTF2_TimeStamp      time,
                            OTF2_CommRef        threadContingent,
                            uint64_t            sequenceCount ) {
  TO_BE_IMPLEMENTED;
  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_EvtWriter_ThreadWait( OTF2_EvtWriter*     writer,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           OTF2_CommRef        threadContingent,
                           uint64_t            sequenceCount ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ThreadEnd( OTF2_EvtWriter*     writer,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      time,
                          OTF2_CommRef        threadContingent,
                          uint64_t            sequenceCount ) {
  TO_BE_IMPLEMENTED;
  return OTF2_SUCCESS;
}

OTF2_ErrorCode
OTF2_EvtWriter_CallingContextEnter( OTF2_EvtWriter*        writer,
                                    OTF2_AttributeList*    attributeList,
                                    OTF2_TimeStamp         time,
                                    OTF2_CallingContextRef callingContext,
                                    uint32_t               unwindDistance ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_CallingContextLeave( OTF2_EvtWriter*        writer,
                                    OTF2_AttributeList*    attributeList,
                                    OTF2_TimeStamp         time,
                                    OTF2_CallingContextRef callingContext ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_CallingContextSample( OTF2_EvtWriter*            writer,
                                     OTF2_AttributeList*        attributeList,
                                     OTF2_TimeStamp             time,
                                     OTF2_CallingContextRef     callingContext,
                                     uint32_t                   unwindDistance,
                                     OTF2_InterruptGeneratorRef interruptGenerator ) {
   NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_IoCreateHandle( OTF2_EvtWriter*     writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      time,
                               OTF2_IoHandleRef    handle,
                               OTF2_IoAccessMode   mode,
                               OTF2_IoCreationFlag creationFlags,
                               OTF2_IoStatusFlag   statusFlags ) {
   NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_IoDestroyHandle( OTF2_EvtWriter*     writer,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                OTF2_IoHandleRef    handle ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_IoDuplicateHandle( OTF2_EvtWriter*     writer,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      time,
                                  OTF2_IoHandleRef    oldHandle,
                                  OTF2_IoHandleRef    newHandle,
                                  OTF2_IoStatusFlag   statusFlags ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_IoSeek( OTF2_EvtWriter*     writer,
                       OTF2_AttributeList* attributeList,
                       OTF2_TimeStamp      time,
                       OTF2_IoHandleRef    handle,
                       int64_t             offsetRequest,
                       OTF2_IoSeekOption   whence,
                       uint64_t            offsetResult ) {
   NOT_IMPLEMENTED;
     }

OTF2_ErrorCode
OTF2_EvtWriter_IoChangeStatusFlags( OTF2_EvtWriter*     writer,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_TimeStamp      time,
                                    OTF2_IoHandleRef    handle,
                                    OTF2_IoStatusFlag   statusFlags ) {
   NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_IoDeleteFile( OTF2_EvtWriter*     writer,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      time,
                             OTF2_IoParadigmRef  ioParadigm,
                             OTF2_IoFileRef      file ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_IoOperationBegin( OTF2_EvtWriter*      writer,
                                 OTF2_AttributeList*  attributeList,
                                 OTF2_TimeStamp       time,
                                 OTF2_IoHandleRef     handle,
                                 OTF2_IoOperationMode mode,
                                 OTF2_IoOperationFlag operationFlags,
                                 uint64_t             bytesRequest,
                                 uint64_t             matchingId ) {
   NOT_IMPLEMENTED;
     }

OTF2_ErrorCode
OTF2_EvtWriter_IoOperationTest( OTF2_EvtWriter*     writer,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      time,
                                OTF2_IoHandleRef    handle,
                                uint64_t            matchingId ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_IoOperationIssued( OTF2_EvtWriter*     writer,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      time,
                                  OTF2_IoHandleRef    handle,
                                  uint64_t            matchingId ) {
   NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_IoOperationComplete( OTF2_EvtWriter*     writer,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_TimeStamp      time,
                                    OTF2_IoHandleRef    handle,
                                    uint64_t            bytesResult,
                                    uint64_t            matchingId ) {
   NOT_IMPLEMENTED;
}


OTF2_ErrorCode
OTF2_EvtWriter_IoOperationCancelled( OTF2_EvtWriter*     writer,
                                     OTF2_AttributeList* attributeList,
                                     OTF2_TimeStamp      time,
                                     OTF2_IoHandleRef    handle,
                                     uint64_t            matchingId ) {
   NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_IoAcquireLock( OTF2_EvtWriter*     writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_IoHandleRef    handle,
                              OTF2_LockType       lockType ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_IoReleaseLock( OTF2_EvtWriter*     writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      time,
                              OTF2_IoHandleRef    handle,
                              OTF2_LockType       lockType ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_IoTryLock( OTF2_EvtWriter*     writer,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      time,
                          OTF2_IoHandleRef    handle,
                          OTF2_LockType       lockType ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ProgramBegin( OTF2_EvtWriter*       writer,
                             OTF2_AttributeList*   attributeList,
                             OTF2_TimeStamp        time,
                             OTF2_StringRef        programName,
                             uint32_t              numberOfArguments,
                             const OTF2_StringRef* programArguments ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ProgramEnd( OTF2_EvtWriter*     writer,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           int64_t             exitStatus ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_NonBlockingCollectiveRequest( OTF2_EvtWriter*     writer,
                                             OTF2_AttributeList* attributeList,
                                             OTF2_TimeStamp      time,
                                             uint64_t            requestID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_NonBlockingCollectiveComplete( OTF2_EvtWriter*     writer,
                                              OTF2_AttributeList* attributeList,
                                              OTF2_TimeStamp      time,
                                              OTF2_CollectiveOp   collectiveOp,
                                              OTF2_CommRef        communicator,
                                              uint32_t            root,
                                              uint64_t            sizeSent,
                                              uint64_t            sizeReceived,
                                              uint64_t            requestID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_CommCreate( OTF2_EvtWriter*     writer,
                           OTF2_AttributeList* attributeList,
                           OTF2_TimeStamp      time,
                           OTF2_CommRef        communicator ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_CommDestroy( OTF2_EvtWriter*     writer,
                            OTF2_AttributeList* attributeList,
                            OTF2_TimeStamp      time,
                            OTF2_CommRef        communicator ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_StoreRewindPoint( OTF2_EvtWriter* writer,
                                 uint32_t        rewindId ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_Rewind( OTF2_EvtWriter* writer,
                       uint32_t        rewindId ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_EvtWriter_ClearRewindPoint( OTF2_EvtWriter* writer,
                                 uint32_t        rewindId ) {
  NOT_IMPLEMENTED;
}
