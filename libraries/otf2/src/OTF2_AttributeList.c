#include <stdio.h>
#include <stdlib.h>

#include "htf/htf.h"
#include "htf/htf_attribute.h"
#include "otf2/OTF2_AttributeList.h"
#include "otf2/otf2.h"

OTF2_AttributeList* OTF2_AttributeList_New(void) {
  OTF2_AttributeList* list = malloc(sizeof(OTF2_AttributeList));
  htf_attribute_list_init(list);
  return list;
}

OTF2_ErrorCode OTF2_AttributeList_Delete(OTF2_AttributeList* list) {
  htf_attribute_list_finalize(list);
  free(list);
  return OTF2_SUCCESS;
 
}


OTF2_ErrorCode OTF2_AttributeList_AddAttribute(OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_Type type,
                                               OTF2_AttributeValue attributeValue) {
  htf_type_t t = OTF2_HTF_TYPE(type);
  AttributeValue v = OTF2_HTF_ATTRIBUTE_VALUE(attributeValue, t);
  return htf_attribute_list_add_attribute(attributeList, attribute, get_value_size(t), v);
}

OTF2_ErrorCode OTF2_AttributeList_AddUint8(OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           uint8_t uint8Value) {
  AttributeValue u;
  u.uint8 = uint8Value;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.uint8), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddUint16(OTF2_AttributeList* attributeList,
                                            OTF2_AttributeRef attribute,
                                            uint16_t uint16Value) {
  AttributeValue u;
  u.uint16 = uint16Value;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.uint16), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddUint32(OTF2_AttributeList* attributeList,
                                            OTF2_AttributeRef attribute,
                                            uint32_t uint32Value) {
  AttributeValue u;
  u.uint32 = uint32Value;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.uint32), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddUint64(OTF2_AttributeList* attributeList,
                                            OTF2_AttributeRef attribute,
                                            uint64_t uint64Value) {
  AttributeValue u;
  u.uint64 = uint64Value;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.uint64), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddInt8(OTF2_AttributeList* attributeList,
                                          OTF2_AttributeRef attribute,
                                          int8_t int8Value) {
  AttributeValue u;
  u.int8 = int8Value;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.int8), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddInt16(OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           int16_t int16Value) {
  AttributeValue u;
  u.int16 = int16Value;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.int16), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddInt32(OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           int32_t int32Value) {
  AttributeValue u;
  u.int32 = int32Value;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.int32), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddInt64(OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           int64_t int64Value) {
  AttributeValue u;
  u.int64 = int64Value;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.int64), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddFloat(OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           float float32Value) {
  AttributeValue u;
  u.float32 = float32Value;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.float32), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddDouble(OTF2_AttributeList* attributeList,
                                            OTF2_AttributeRef attribute,
                                            double float64Value) {
  AttributeValue u;
  u.float64 = float64Value;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.float64), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddStringRef(OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_StringRef stringRef) {
  AttributeValue u;
  u.string_ref = stringRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.string_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddAttributeRef(OTF2_AttributeList* attributeList,
                                                  OTF2_AttributeRef attribute,
                                                  OTF2_AttributeRef attributeRef) {
  AttributeValue u;
  u.attribute_ref = attributeRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.attribute_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddLocationRef(OTF2_AttributeList* attributeList,
                                                 OTF2_AttributeRef attribute,
                                                 OTF2_LocationRef locationRef) {
  AttributeValue u;
  u.location_ref = locationRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.location_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddRegionRef(OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_RegionRef regionRef) {
  AttributeValue u;
  u.region_ref = regionRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.region_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddGroupRef(OTF2_AttributeList* attributeList,
                                              OTF2_AttributeRef attribute,
                                              OTF2_GroupRef groupRef) {
  AttributeValue u;
  u.group_ref = groupRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.group_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddMetricRef(OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_MetricRef metricRef) {
  AttributeValue u;
  u.metric_ref = metricRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.metric_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddCommRef(OTF2_AttributeList* attributeList,
                                             OTF2_AttributeRef attribute,
                                             OTF2_CommRef commRef) {
  AttributeValue u;
  u.comm_ref = commRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.comm_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddParameterRef(OTF2_AttributeList* attributeList,
                                                  OTF2_AttributeRef attribute,
                                                  OTF2_ParameterRef parameterRef) {
  AttributeValue u;
  u.parameter_ref = parameterRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.parameter_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddRmaWinRef(OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_RmaWinRef rmaWinRef) {
  AttributeValue u;
  u.rma_win_ref = rmaWinRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.rma_win_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddSourceCodeLocationRef(OTF2_AttributeList* attributeList,
                                                           OTF2_AttributeRef attribute,
                                                           OTF2_SourceCodeLocationRef sourceCodeLocationRef) {
  AttributeValue u;
  u.source_code_location_ref = sourceCodeLocationRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.source_code_location_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddCallingContextRef(OTF2_AttributeList* attributeList,
                                                       OTF2_AttributeRef attribute,
                                                       OTF2_CallingContextRef callingContextRef) {
  AttributeValue u;
  u.calling_context_ref = callingContextRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.calling_context_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddInterruptGeneratorRef(OTF2_AttributeList* attributeList,
                                                           OTF2_AttributeRef attribute,
                                                           OTF2_InterruptGeneratorRef interruptGeneratorRef) {
  AttributeValue u;
  u.interrupt_generator_ref = interruptGeneratorRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.interrupt_generator_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddIoFileRef(OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_IoFileRef ioFileRef) {
  AttributeValue u;
  u.io_file_ref =  ioFileRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.io_file_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddIoHandleRef(OTF2_AttributeList* attributeList,
                                                 OTF2_AttributeRef attribute,
                                                 OTF2_IoHandleRef ioHandleRef) {
  AttributeValue u;
  u.io_handle_ref = ioHandleRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.io_handle_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_AddLocationGroupRef(OTF2_AttributeList* attributeList,
                                                      OTF2_AttributeRef attribute,
                                                      OTF2_LocationGroupRef locationGroupRef) {
  AttributeValue u;
  u.location_group_ref = locationGroupRef;
  return htf_attribute_list_add_attribute(attributeList, attribute, sizeof(u.location_group_ref), u);
}

OTF2_ErrorCode OTF2_AttributeList_RemoveAttribute(OTF2_AttributeList* attributeList, OTF2_AttributeRef attribute) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_RemoveAllAttributes(OTF2_AttributeList* attributeList) {
  NOT_IMPLEMENTED;
}

bool OTF2_AttributeList_TestAttributeByID(const OTF2_AttributeList* attributeList, OTF2_AttributeRef attribute) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetAttributeByID(const OTF2_AttributeList* attributeList,
                                                   OTF2_AttributeRef attribute,
                                                   OTF2_Type* type,
                                                   OTF2_AttributeValue* attributeValue) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetUint8(const OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           uint8_t* uint8Value) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetUint16(const OTF2_AttributeList* attributeList,
                                            OTF2_AttributeRef attribute,
                                            uint16_t* uint16Value) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetUint32(const OTF2_AttributeList* attributeList,
                                            OTF2_AttributeRef attribute,
                                            uint32_t* uint32Value) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetUint64(const OTF2_AttributeList* attributeList,
                                            OTF2_AttributeRef attribute,
                                            uint64_t* uint64Value) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetInt8(const OTF2_AttributeList* attributeList,
                                          OTF2_AttributeRef attribute,
                                          int8_t* int8Value) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetInt16(const OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           int16_t* int16Value) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetInt32(const OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           int32_t* int32Value) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetInt64(const OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           int64_t* int64Value) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetFloat(const OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           float* float32Value) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetDouble(const OTF2_AttributeList* attributeList,
                                            OTF2_AttributeRef attribute,
                                            double* float64Value) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetStringRef(const OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_StringRef* stringRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetAttributeRef(const OTF2_AttributeList* attributeList,
                                                  OTF2_AttributeRef attribute,
                                                  OTF2_AttributeRef* attributeRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetLocationRef(const OTF2_AttributeList* attributeList,
                                                 OTF2_AttributeRef attribute,
                                                 OTF2_LocationRef* locationRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetRegionRef(const OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_RegionRef* regionRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetGroupRef(const OTF2_AttributeList* attributeList,
                                              OTF2_AttributeRef attribute,
                                              OTF2_GroupRef* groupRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetMetricRef(const OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_MetricRef* metricRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetCommRef(const OTF2_AttributeList* attributeList,
                                             OTF2_AttributeRef attribute,
                                             OTF2_CommRef* commRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetParameterRef(const OTF2_AttributeList* attributeList,
                                                  OTF2_AttributeRef attribute,
                                                  OTF2_ParameterRef* parameterRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetRmaWinRef(const OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_RmaWinRef* rmaWinRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetSourceCodeLocationRef(const OTF2_AttributeList* attributeList,
                                                           OTF2_AttributeRef attribute,
                                                           OTF2_SourceCodeLocationRef* sourceCodeLocationRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetCallingContextRef(const OTF2_AttributeList* attributeList,
                                                       OTF2_AttributeRef attribute,
                                                       OTF2_CallingContextRef* callingContextRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetInterruptGeneratorRef(const OTF2_AttributeList* attributeList,
                                                           OTF2_AttributeRef attribute,
                                                           OTF2_InterruptGeneratorRef* interruptGeneratorRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetIoFileRef(const OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_IoFileRef* ioFileRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetIoHandleRef(const OTF2_AttributeList* attributeList,
                                                 OTF2_AttributeRef attribute,
                                                 OTF2_IoHandleRef* ioHandleRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetLocationGroupRef(const OTF2_AttributeList* attributeList,
                                                      OTF2_AttributeRef attribute,
                                                      OTF2_LocationGroupRef* locationGroupRef) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_GetAttributeByIndex(const OTF2_AttributeList* attributeList,
                                                      uint32_t index,
                                                      OTF2_AttributeRef* attribute,
                                                      OTF2_Type* type,
                                                      OTF2_AttributeValue* attributeValue) {
  NOT_IMPLEMENTED;
}

OTF2_ErrorCode OTF2_AttributeList_PopAttribute(OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef* attribute,
                                               OTF2_Type* type,
                                               OTF2_AttributeValue* attributeValue) {
  NOT_IMPLEMENTED;
}

uint32_t OTF2_AttributeList_GetNumberOfElements(const OTF2_AttributeList* attributeList) {
  NOT_IMPLEMENTED;
}
