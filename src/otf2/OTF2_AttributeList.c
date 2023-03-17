#include <stdlib.h>
#include <stdio.h>

#include "htf.h"
#include "otf2.h"
#include "OTF2_AttributeList.h"

OTF2_AttributeList*
OTF2_AttributeList_New( void ) {
  TO_BE_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_Delete( OTF2_AttributeList* attributeList ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddAttribute( OTF2_AttributeList* attributeList,
                                 OTF2_AttributeRef   attribute,
                                 OTF2_Type           type,
                                 OTF2_AttributeValue attributeValue ) {
  TO_BE_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddUint8( OTF2_AttributeList* attributeList,
                             OTF2_AttributeRef   attribute,
                             uint8_t             uint8Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddUint16( OTF2_AttributeList* attributeList,
                              OTF2_AttributeRef   attribute,
                              uint16_t            uint16Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddUint32( OTF2_AttributeList* attributeList,
                              OTF2_AttributeRef   attribute,
                              uint32_t            uint32Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddUint64( OTF2_AttributeList* attributeList,
                              OTF2_AttributeRef   attribute,
                              uint64_t            uint64Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddInt8( OTF2_AttributeList* attributeList,
                            OTF2_AttributeRef   attribute,
                            int8_t              int8Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddInt16( OTF2_AttributeList* attributeList,
                             OTF2_AttributeRef   attribute,
                             int16_t             int16Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddInt32( OTF2_AttributeList* attributeList,
                             OTF2_AttributeRef   attribute,
                             int32_t             int32Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddInt64( OTF2_AttributeList* attributeList,
                             OTF2_AttributeRef   attribute,
                             int64_t             int64Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddFloat( OTF2_AttributeList* attributeList,
                             OTF2_AttributeRef   attribute,
                             float               float32Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddDouble( OTF2_AttributeList* attributeList,
                              OTF2_AttributeRef   attribute,
                              double              float64Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddStringRef( OTF2_AttributeList* attributeList,
                                 OTF2_AttributeRef   attribute,
                                 OTF2_StringRef      stringRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddAttributeRef( OTF2_AttributeList* attributeList,
                                    OTF2_AttributeRef   attribute,
                                    OTF2_AttributeRef   attributeRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddLocationRef( OTF2_AttributeList* attributeList,
                                   OTF2_AttributeRef   attribute,
                                   OTF2_LocationRef    locationRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddRegionRef( OTF2_AttributeList* attributeList,
                                 OTF2_AttributeRef   attribute,
                                 OTF2_RegionRef      regionRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddGroupRef( OTF2_AttributeList* attributeList,
                                OTF2_AttributeRef   attribute,
                                OTF2_GroupRef       groupRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddMetricRef( OTF2_AttributeList* attributeList,
                                 OTF2_AttributeRef   attribute,
                                 OTF2_MetricRef      metricRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddCommRef( OTF2_AttributeList* attributeList,
                               OTF2_AttributeRef   attribute,
                               OTF2_CommRef        commRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddParameterRef( OTF2_AttributeList* attributeList,
                                    OTF2_AttributeRef   attribute,
                                    OTF2_ParameterRef   parameterRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddRmaWinRef( OTF2_AttributeList* attributeList,
                                 OTF2_AttributeRef   attribute,
                                 OTF2_RmaWinRef      rmaWinRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddSourceCodeLocationRef( OTF2_AttributeList*        attributeList,
                                             OTF2_AttributeRef          attribute,
                                             OTF2_SourceCodeLocationRef sourceCodeLocationRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddCallingContextRef( OTF2_AttributeList*    attributeList,
                                         OTF2_AttributeRef      attribute,
                                         OTF2_CallingContextRef callingContextRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddInterruptGeneratorRef( OTF2_AttributeList*        attributeList,
                                             OTF2_AttributeRef          attribute,
                                             OTF2_InterruptGeneratorRef interruptGeneratorRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddIoFileRef( OTF2_AttributeList* attributeList,
                                 OTF2_AttributeRef   attribute,
                                 OTF2_IoFileRef      ioFileRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddIoHandleRef( OTF2_AttributeList* attributeList,
                                   OTF2_AttributeRef   attribute,
                                   OTF2_IoHandleRef    ioHandleRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_AddLocationGroupRef( OTF2_AttributeList*   attributeList,
                                        OTF2_AttributeRef     attribute,
                                        OTF2_LocationGroupRef locationGroupRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_RemoveAttribute( OTF2_AttributeList* attributeList,
                                    OTF2_AttributeRef   attribute ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_RemoveAllAttributes( OTF2_AttributeList* attributeList ) {
  NOT_IMPLEMENTED;
}

bool
OTF2_AttributeList_TestAttributeByID( const OTF2_AttributeList* attributeList,
                                      OTF2_AttributeRef         attribute ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetAttributeByID( const OTF2_AttributeList* attributeList,
                                     OTF2_AttributeRef         attribute,
                                     OTF2_Type*                type,
                                     OTF2_AttributeValue*      attributeValue ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetUint8( const OTF2_AttributeList* attributeList,
                             OTF2_AttributeRef         attribute,
                             uint8_t*                  uint8Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetUint16( const OTF2_AttributeList* attributeList,
                              OTF2_AttributeRef         attribute,
                              uint16_t*                 uint16Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetUint32( const OTF2_AttributeList* attributeList,
                              OTF2_AttributeRef         attribute,
                              uint32_t*                 uint32Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetUint64( const OTF2_AttributeList* attributeList,
                              OTF2_AttributeRef         attribute,
                              uint64_t*                 uint64Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetInt8( const OTF2_AttributeList* attributeList,
                            OTF2_AttributeRef         attribute,
                            int8_t*                   int8Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetInt16( const OTF2_AttributeList* attributeList,
                             OTF2_AttributeRef         attribute,
                             int16_t*                  int16Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetInt32( const OTF2_AttributeList* attributeList,
                             OTF2_AttributeRef         attribute,
                             int32_t*                  int32Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetInt64( const OTF2_AttributeList* attributeList,
                             OTF2_AttributeRef         attribute,
                             int64_t*                  int64Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetFloat( const OTF2_AttributeList* attributeList,
                             OTF2_AttributeRef         attribute,
                             float*                    float32Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetDouble( const OTF2_AttributeList* attributeList,
                              OTF2_AttributeRef         attribute,
                              double*                   float64Value ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetStringRef( const OTF2_AttributeList* attributeList,
                                 OTF2_AttributeRef         attribute,
                                 OTF2_StringRef*           stringRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetAttributeRef( const OTF2_AttributeList* attributeList,
                                    OTF2_AttributeRef         attribute,
                                    OTF2_AttributeRef*        attributeRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetLocationRef( const OTF2_AttributeList* attributeList,
                                   OTF2_AttributeRef         attribute,
                                   OTF2_LocationRef*         locationRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetRegionRef( const OTF2_AttributeList* attributeList,
                                 OTF2_AttributeRef         attribute,
                                 OTF2_RegionRef*           regionRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetGroupRef( const OTF2_AttributeList* attributeList,
                                OTF2_AttributeRef         attribute,
                                OTF2_GroupRef*            groupRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetMetricRef( const OTF2_AttributeList* attributeList,
                                 OTF2_AttributeRef         attribute,
                                 OTF2_MetricRef*           metricRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetCommRef( const OTF2_AttributeList* attributeList,
                               OTF2_AttributeRef         attribute,
                               OTF2_CommRef*             commRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetParameterRef( const OTF2_AttributeList* attributeList,
                                    OTF2_AttributeRef         attribute,
                                    OTF2_ParameterRef*        parameterRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetRmaWinRef( const OTF2_AttributeList* attributeList,
                                 OTF2_AttributeRef         attribute,
                                 OTF2_RmaWinRef*           rmaWinRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetSourceCodeLocationRef( const OTF2_AttributeList*   attributeList,
                                             OTF2_AttributeRef           attribute,
                                             OTF2_SourceCodeLocationRef* sourceCodeLocationRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetCallingContextRef( const OTF2_AttributeList* attributeList,
                                         OTF2_AttributeRef         attribute,
                                         OTF2_CallingContextRef*   callingContextRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetInterruptGeneratorRef( const OTF2_AttributeList*   attributeList,
                                             OTF2_AttributeRef           attribute,
                                             OTF2_InterruptGeneratorRef* interruptGeneratorRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetIoFileRef( const OTF2_AttributeList* attributeList,
                                 OTF2_AttributeRef         attribute,
                                 OTF2_IoFileRef*           ioFileRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetIoHandleRef( const OTF2_AttributeList* attributeList,
                                   OTF2_AttributeRef         attribute,
                                   OTF2_IoHandleRef*         ioHandleRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetLocationGroupRef( const OTF2_AttributeList* attributeList,
                                        OTF2_AttributeRef         attribute,
                                        OTF2_LocationGroupRef*    locationGroupRef ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_GetAttributeByIndex( const OTF2_AttributeList* attributeList,
                                        uint32_t                  index,
                                        OTF2_AttributeRef*        attribute,
                                        OTF2_Type*                type,
                                        OTF2_AttributeValue*      attributeValue ) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode
OTF2_AttributeList_PopAttribute( OTF2_AttributeList*  attributeList,
                                 OTF2_AttributeRef*   attribute,
                                 OTF2_Type*           type,
                                 OTF2_AttributeValue* attributeValue ) {
  NOT_IMPLEMENTED;
}

uint32_t
OTF2_AttributeList_GetNumberOfElements( const OTF2_AttributeList* attributeList ) {
  NOT_IMPLEMENTED;
}


