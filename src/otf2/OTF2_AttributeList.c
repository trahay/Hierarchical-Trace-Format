#include <stdio.h>
#include <stdlib.h>

#include "OTF2_AttributeList.h"
#include "htf.h"
#include "otf2.h"

#define NB_ATTRIBUTE_MAX 128
#define ATTRIBUTE_MAX_BUFFER_SIZE (NB_ATTRIBUTE_MAX * sizeof(OTF2_AttributeValue))

struct OTF2_AttributeList_struct {
  OTF2_AttributeValue values[NB_ATTRIBUTE_MAX];
  OTF2_AttributeRef   refs[NB_ATTRIBUTE_MAX];
  OTF2_Type types[NB_ATTRIBUTE_MAX];
  int nb_values;

  uint8_t data_buffer[ATTRIBUTE_MAX_BUFFER_SIZE];
  size_t offset;
};

static inline size_t get_value_size(OTF2_Type t) {
  union OTF2_AttributeValue_union u;
  switch(t) {
  case OTF2_TYPE_NONE: return 0; break;
  case OTF2_TYPE_UINT8: return sizeof(u.uint8); break;
  case OTF2_TYPE_UINT16: return sizeof(u.uint16); break;
  case OTF2_TYPE_UINT32: return sizeof(u.uint32); break;
  case OTF2_TYPE_UINT64: return sizeof(u.uint64); break;
  case OTF2_TYPE_INT8: return sizeof(u.int8); break;
  case OTF2_TYPE_INT16: return sizeof(u.int16); break;
  case OTF2_TYPE_INT32: return sizeof(u.int32); break;
  case OTF2_TYPE_INT64: return sizeof(u.int64); break;
  case OTF2_TYPE_FLOAT: return sizeof(u.float32); break;
  case OTF2_TYPE_DOUBLE: return sizeof(u.float64); break;
  case OTF2_TYPE_STRING: return sizeof(u.stringRef); break;
  case OTF2_TYPE_ATTRIBUTE: return sizeof(u.attributeRef); break;
  case OTF2_TYPE_LOCATION: return sizeof(u.locationRef); break;
  case OTF2_TYPE_REGION: return sizeof(u.regionRef); break;
  case OTF2_TYPE_GROUP: return sizeof(u.groupRef); break;
  case OTF2_TYPE_METRIC: return sizeof(u.metricRef); break;
  case OTF2_TYPE_COMM: return sizeof(u.commRef); break;
  case OTF2_TYPE_PARAMETER: return sizeof(u.parameterRef); break;
  case OTF2_TYPE_RMA_WIN: return sizeof(u.rmaWinRef); break;
  case OTF2_TYPE_SOURCE_CODE_LOCATION: return sizeof(u.sourceCodeLocationRef); break;
  case OTF2_TYPE_CALLING_CONTEXT: return sizeof(u.callingContextRef); break;
  case OTF2_TYPE_INTERRUPT_GENERATOR: return sizeof(u.interruptGeneratorRef); break;
  case OTF2_TYPE_IO_FILE: return sizeof(u.ioFileRef); break;
  case OTF2_TYPE_IO_HANDLE: return sizeof(u.ioHandleRef); break;
  case OTF2_TYPE_LOCATION_GROUP: return sizeof(u.locationGroupRef); break;
  }
  return 0;
}

static inline void push_data(struct OTF2_AttributeList_struct *l, void* data, size_t data_size) {
  htf_assert(l->offset + data_size <= ATTRIBUTE_MAX_BUFFER_SIZE);
  memcpy(&l->data_buffer[l->offset], data, data_size);
  l->offset += data_size;
}

static inline void pop_data(struct OTF2_AttributeList_struct *l, void* data, size_t data_size, size_t *current_offset) {
  htf_assert((*current_offset) + data_size <= ATTRIBUTE_MAX_BUFFER_SIZE);
  htf_assert((*current_offset) + data_size <= l->offset);

  memcpy(data, &l->types[(*current_offset)], data_size);
  *current_offset += data_size;
}

OTF2_AttributeList* OTF2_AttributeList_New(void) {
  struct OTF2_AttributeList_struct* list = malloc(sizeof(struct OTF2_AttributeList_struct));
  list->nb_values = 0;
  list->offset = 0;
  return list;
}

OTF2_ErrorCode OTF2_AttributeList_Delete(OTF2_AttributeList* attributeList) {
  free(attributeList);
  return OTF2_SUCCESS;
 
}

static OTF2_ErrorCode _OTF2_AttributeList_AddAttribute_generic(OTF2_AttributeList* attributeList,
							       OTF2_AttributeRef attribute,
							       size_t data_size,
							       OTF2_Type type,
							       OTF2_AttributeValue attributeValue) {
  if(attributeList->nb_values+1 >= NB_ATTRIBUTE_MAX) {
    htf_warn("[OTF2-HTF] too many attributes\n");
    return OTF2_ERROR_INVALID_ARGUMENT;
  }

  htf_assert(attributeList->offset + data_size <= ATTRIBUTE_MAX_BUFFER_SIZE);

  int i = attributeList->nb_values++;
  attributeList->values[i] = attributeValue;
  attributeList->refs[i] = attribute;
  attributeList->types[i] = type;
  
  push_data(attributeList, &attributeValue, data_size);
  return OTF2_SUCCESS;
}


OTF2_ErrorCode OTF2_AttributeList_AddAttribute(OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_Type type,
                                               OTF2_AttributeValue attributeValue) {
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, get_value_size(type), type, attributeValue);
}

OTF2_ErrorCode OTF2_AttributeList_AddUint8(OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           uint8_t uint8Value) {
  union OTF2_AttributeValue_union u;
  u.uint8 = uint8Value;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(uint8Value), OTF2_TYPE_UINT8, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddUint16(OTF2_AttributeList* attributeList,
                                            OTF2_AttributeRef attribute,
                                            uint16_t uint16Value) {
  union OTF2_AttributeValue_union u;
  u.uint16 = uint16Value;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(uint16Value), OTF2_TYPE_UINT16, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddUint32(OTF2_AttributeList* attributeList,
                                            OTF2_AttributeRef attribute,
                                            uint32_t uint32Value) {
  union OTF2_AttributeValue_union u;
  u.uint32 = uint32Value;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(uint32Value), OTF2_TYPE_UINT32, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddUint64(OTF2_AttributeList* attributeList,
                                            OTF2_AttributeRef attribute,
                                            uint64_t uint64Value) {
  union OTF2_AttributeValue_union u;
  u.uint64 = uint64Value;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(uint64Value), OTF2_TYPE_UINT64, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddInt8(OTF2_AttributeList* attributeList,
                                          OTF2_AttributeRef attribute,
                                          int8_t int8Value) {
  union OTF2_AttributeValue_union u;
  u.int8 = int8Value;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(int8Value), OTF2_TYPE_INT8, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddInt16(OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           int16_t int16Value) {
  union OTF2_AttributeValue_union u;
  u.int16 = int16Value;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(int16Value), OTF2_TYPE_INT16, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddInt32(OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           int32_t int32Value) {
  union OTF2_AttributeValue_union u;
  u.int32 = int32Value;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(int32Value), OTF2_TYPE_INT32, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddInt64(OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           int64_t int64Value) {
  union OTF2_AttributeValue_union u;
  u.int64 = int64Value;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(int64Value), OTF2_TYPE_INT64, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddFloat(OTF2_AttributeList* attributeList,
                                           OTF2_AttributeRef attribute,
                                           float float32Value) {
  union OTF2_AttributeValue_union u;
  u.float32 = float32Value;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(float32Value), OTF2_TYPE_FLOAT, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddDouble(OTF2_AttributeList* attributeList,
                                            OTF2_AttributeRef attribute,
                                            double float64Value) {
  union OTF2_AttributeValue_union u;
  u.float64 = float64Value;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(float64Value), OTF2_TYPE_DOUBLE, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddStringRef(OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_StringRef stringRef) {
  union OTF2_AttributeValue_union u;
  u.stringRef = stringRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(stringRef), OTF2_TYPE_STRING, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddAttributeRef(OTF2_AttributeList* attributeList,
                                                  OTF2_AttributeRef attribute,
                                                  OTF2_AttributeRef attributeRef) {
  union OTF2_AttributeValue_union u;
  u.attributeRef = attributeRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(attributeRef), OTF2_TYPE_ATTRIBUTE, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddLocationRef(OTF2_AttributeList* attributeList,
                                                 OTF2_AttributeRef attribute,
                                                 OTF2_LocationRef locationRef) {
  union OTF2_AttributeValue_union u;
  u.locationRef = locationRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(locationRef), OTF2_TYPE_LOCATION, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddRegionRef(OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_RegionRef regionRef) {
  union OTF2_AttributeValue_union u;
  u.regionRef = regionRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(regionRef), OTF2_TYPE_REGION, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddGroupRef(OTF2_AttributeList* attributeList,
                                              OTF2_AttributeRef attribute,
                                              OTF2_GroupRef groupRef) {
  union OTF2_AttributeValue_union u;
  u.groupRef = groupRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(groupRef), OTF2_TYPE_GROUP, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddMetricRef(OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_MetricRef metricRef) {
  union OTF2_AttributeValue_union u;
  u.metricRef = metricRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(metricRef), OTF2_TYPE_METRIC, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddCommRef(OTF2_AttributeList* attributeList,
                                             OTF2_AttributeRef attribute,
                                             OTF2_CommRef commRef) {
  union OTF2_AttributeValue_union u;
  u.commRef = commRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(commRef), OTF2_TYPE_COMM, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddParameterRef(OTF2_AttributeList* attributeList,
                                                  OTF2_AttributeRef attribute,
                                                  OTF2_ParameterRef parameterRef) {
  union OTF2_AttributeValue_union u;
  u.parameterRef = parameterRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(parameterRef), OTF2_TYPE_PARAMETER, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddRmaWinRef(OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_RmaWinRef rmaWinRef) {
  union OTF2_AttributeValue_union u;
  u.rmaWinRef = rmaWinRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(rmaWinRef), OTF2_TYPE_RMA_WIN, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddSourceCodeLocationRef(OTF2_AttributeList* attributeList,
                                                           OTF2_AttributeRef attribute,
                                                           OTF2_SourceCodeLocationRef sourceCodeLocationRef) {
  union OTF2_AttributeValue_union u;
  u.sourceCodeLocationRef = sourceCodeLocationRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(sourceCodeLocationRef), OTF2_TYPE_SOURCE_CODE_LOCATION, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddCallingContextRef(OTF2_AttributeList* attributeList,
                                                       OTF2_AttributeRef attribute,
                                                       OTF2_CallingContextRef callingContextRef) {
  union OTF2_AttributeValue_union u;
  u.callingContextRef = callingContextRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(callingContextRef), OTF2_TYPE_CALLING_CONTEXT, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddInterruptGeneratorRef(OTF2_AttributeList* attributeList,
                                                           OTF2_AttributeRef attribute,
                                                           OTF2_InterruptGeneratorRef interruptGeneratorRef) {
  union OTF2_AttributeValue_union u;
  u.interruptGeneratorRef = interruptGeneratorRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(interruptGeneratorRef), OTF2_TYPE_INTERRUPT_GENERATOR, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddIoFileRef(OTF2_AttributeList* attributeList,
                                               OTF2_AttributeRef attribute,
                                               OTF2_IoFileRef ioFileRef) {
  union OTF2_AttributeValue_union u;
  u.ioFileRef =  ioFileRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(ioFileRef), OTF2_TYPE_IO_FILE, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddIoHandleRef(OTF2_AttributeList* attributeList,
                                                 OTF2_AttributeRef attribute,
                                                 OTF2_IoHandleRef ioHandleRef) {
  union OTF2_AttributeValue_union u;
  u.ioHandleRef = ioHandleRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(ioHandleRef), OTF2_TYPE_IO_HANDLE, u);
}

OTF2_ErrorCode OTF2_AttributeList_AddLocationGroupRef(OTF2_AttributeList* attributeList,
                                                      OTF2_AttributeRef attribute,
                                                      OTF2_LocationGroupRef locationGroupRef) {
  union OTF2_AttributeValue_union u;
  u.locationGroupRef = locationGroupRef;
  return _OTF2_AttributeList_AddAttribute_generic(attributeList, attribute, sizeof(locationGroupRef), OTF2_TYPE_LOCATION_GROUP, u);
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
