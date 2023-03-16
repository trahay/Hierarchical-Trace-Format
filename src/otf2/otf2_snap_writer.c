#include <stdlib.h>
#include <stdio.h>

#include "htf.h"
#include "otf2.h"
#include "OTF2_SnapWriter.h"

OTF2_ErrorCode
OTF2_SnapWriter_GetLocationID( const OTF2_SnapWriter* writer,
                               OTF2_LocationRef*      locationID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_SnapshotStart( OTF2_SnapWriter*    writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      snapTime,
                               uint64_t            numberOfRecords ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_SnapshotEnd( OTF2_SnapWriter*    writer,
                             OTF2_AttributeList* attributeList,
                             OTF2_TimeStamp      snapTime,
                             uint64_t            contReadPos ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_MeasurementOnOff( OTF2_SnapWriter*     writer,
                                  OTF2_AttributeList*  attributeList,
                                  OTF2_TimeStamp       snapTime,
                                  OTF2_TimeStamp       origEventTime,
                                  OTF2_MeasurementMode measurementMode ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_Enter( OTF2_SnapWriter*    writer,
                       OTF2_AttributeList* attributeList,
                       OTF2_TimeStamp      snapTime,
                       OTF2_TimeStamp      origEventTime,
                       OTF2_RegionRef      region ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_MpiSend( OTF2_SnapWriter*    writer,
                         OTF2_AttributeList* attributeList,
                         OTF2_TimeStamp      snapTime,
                         OTF2_TimeStamp      origEventTime,
                         uint32_t            receiver,
                         OTF2_CommRef        communicator,
                         uint32_t            msgTag,
                         uint64_t            msgLength ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_MpiIsend( OTF2_SnapWriter*    writer,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      snapTime,
                          OTF2_TimeStamp      origEventTime,
                          uint32_t            receiver,
                          OTF2_CommRef        communicator,
                          uint32_t            msgTag,
                          uint64_t            msgLength,
                          uint64_t            requestID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_MpiIsendComplete( OTF2_SnapWriter*    writer,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      snapTime,
                                  OTF2_TimeStamp      origEventTime,
                                  uint64_t            requestID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_MpiRecv( OTF2_SnapWriter*    writer,
                         OTF2_AttributeList* attributeList,
                         OTF2_TimeStamp      snapTime,
                         OTF2_TimeStamp      origEventTime,
                         uint32_t            sender,
                         OTF2_CommRef        communicator,
                         uint32_t            msgTag,
                         uint64_t            msgLength ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_MpiIrecvRequest( OTF2_SnapWriter*    writer,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      snapTime,
                                 OTF2_TimeStamp      origEventTime,
                                 uint64_t            requestID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_MpiIrecv( OTF2_SnapWriter*    writer,
                          OTF2_AttributeList* attributeList,
                          OTF2_TimeStamp      snapTime,
                          OTF2_TimeStamp      origEventTime,
                          uint32_t            sender,
                          OTF2_CommRef        communicator,
                          uint32_t            msgTag,
                          uint64_t            msgLength,
                          uint64_t            requestID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_MpiCollectiveBegin( OTF2_SnapWriter*    writer,
                                    OTF2_AttributeList* attributeList,
                                    OTF2_TimeStamp      snapTime,
                                    OTF2_TimeStamp      origEventTime ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_MpiCollectiveEnd( OTF2_SnapWriter*    writer,
                                  OTF2_AttributeList* attributeList,
                                  OTF2_TimeStamp      snapTime,
                                  OTF2_TimeStamp      origEventTime,
                                  OTF2_CollectiveOp   collectiveOp,
                                  OTF2_CommRef        communicator,
                                  uint32_t            root,
                                  uint64_t            sizeSent,
                                  uint64_t            sizeReceived ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_OmpFork( OTF2_SnapWriter*    writer,
                         OTF2_AttributeList* attributeList,
                         OTF2_TimeStamp      snapTime,
                         OTF2_TimeStamp      origEventTime,
                         uint32_t            numberOfRequestedThreads ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_OmpAcquireLock( OTF2_SnapWriter*    writer,
                                OTF2_AttributeList* attributeList,
                                OTF2_TimeStamp      snapTime,
                                OTF2_TimeStamp      origEventTime,
                                uint32_t            lockID,
                                uint32_t            acquisitionOrder ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_OmpTaskCreate( OTF2_SnapWriter*    writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      snapTime,
                               OTF2_TimeStamp      origEventTime,
                               uint64_t            taskID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_OmpTaskSwitch( OTF2_SnapWriter*    writer,
                               OTF2_AttributeList* attributeList,
                               OTF2_TimeStamp      snapTime,
                               OTF2_TimeStamp      origEventTime,
                               uint64_t            taskID ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_Metric( OTF2_SnapWriter*        writer,
                        OTF2_AttributeList*     attributeList,
                        OTF2_TimeStamp          snapTime,
                        OTF2_TimeStamp          origEventTime,
                        OTF2_MetricRef          metric,
                        uint8_t                 numberOfMetrics,
                        const OTF2_Type*        typeIDs,
                        const OTF2_MetricValue* metricValues ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_ParameterString( OTF2_SnapWriter*    writer,
                                 OTF2_AttributeList* attributeList,
                                 OTF2_TimeStamp      snapTime,
                                 OTF2_TimeStamp      origEventTime,
                                 OTF2_ParameterRef   parameter,
                                 OTF2_StringRef      string ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_ParameterInt( OTF2_SnapWriter*    writer,
                              OTF2_AttributeList* attributeList,
                              OTF2_TimeStamp      snapTime,
                              OTF2_TimeStamp      origEventTime,
                              OTF2_ParameterRef   parameter,
                              int64_t             value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_SnapWriter_ParameterUnsignedInt( OTF2_SnapWriter*    writer,
                                      OTF2_AttributeList* attributeList,
                                      OTF2_TimeStamp      snapTime,
                                      OTF2_TimeStamp      origEventTime,
                                      OTF2_ParameterRef   parameter,
                                      uint64_t            value ) {
  NOT_IMPLEMENTED;
}


