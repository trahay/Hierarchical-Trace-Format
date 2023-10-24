#pragma once

#include <stdint.h>
#include "htf.h"

#ifdef __cplusplus
namespace htf {
#endif

/** @brief Value container for an attributes.
 */
typedef union AttributeValue {
  /** @brief Arbitrary value of type uint8_t */
  uint8_t uint8;
  /** @brief Arbitrary value of type uint16_t */
  uint16_t uint16;
  /** @brief Arbitrary value of type uint32_t */
  uint32_t uint32;
  /** @brief Arbitrary value of type uint64_t */
  uint64_t uint64;
  /** @brief Arbitrary value of type int8_t */
  int8_t int8;
  /** @brief Arbitrary value of type int16_t */
  int16_t int16;
  /** @brief Arbitrary value of type int32_t */
  int32_t int32;
  /** @brief Arbitrary value of type int64_t */
  int64_t int64;
  /** @brief Arbitrary value of type float */
  float float32;
  /** @brief Arbitrary value of type double */
  double float64;
  /** @brief References a @eref{String} definition */
  StringRef string_ref;
  /** @brief References a @eref{Attribute} definition */
  AttributeRef attribute_ref;
  /** @brief References a @eref{Location} definition */
  Ref location_ref;
  /** @brief References a @eref{Region} definition */
  RegionRef region_ref;
  /** @brief References a @eref{Group} definition */
  Ref group_ref;
  /** @brief References a @eref{MetricClass}, or a @eref{MetricInstance} definition */
  Ref metric_ref;
  /** @brief References a @eref{Comm}, or a @eref{InterComm} definition */
  Ref comm_ref;
  /** @brief References a @eref{Parameter} definition */
  Ref parameter_ref;
  /** @brief References a @eref{RmaWin} definition */
  Ref rma_win_ref;
  /** @brief References a @eref{SourceCodeLocation} definition */
  Ref source_code_location_ref;
  /** @brief References a @eref{CallingContext} definition */
  Ref calling_context_ref;
  /** @brief References a @eref{InterruptGenerator} definition */
  Ref interrupt_generator_ref;
  /** @brief References a @eref{IoRegularFile}, or a @eref{IoDirectory} definition */
  Ref io_file_ref;
  /** @brief References a @eref{IoHandle} definition */
  Ref io_handle_ref;
  /** @brief References a @eref{LocationGroup} definition */
  Ref location_group_ref;
} AttributeValue;

/** @brief HTF basic data types. */
enum AttributeType {
  /** @brief Undefined type.
   *
   *  Type category: None
   */
  HTF_TYPE_NONE = 0,

  /** @brief Unsigned 8-bit integer.
   *
   *  Type category: Integer
   */
  HTF_TYPE_UINT8 = 1,

  /** @brief Unsigned 16-bit integer
   *
   *  Type category: Integer
   */
  HTF_TYPE_UINT16 = 2,

  /** @brief Unsigned 32-bit integer
   *
   *  Type category: Integer
   */
  HTF_TYPE_UINT32 = 3,

  /** @brief Unsigned 64-bit integer
   *
   *  Type category: Integer
   */
  HTF_TYPE_UINT64 = 4,

  /** @brief Signed 8-bit integer
   *
   *  Type category: Integer
   */
  HTF_TYPE_INT8 = 5,

  /** @brief Signed 16-bit integer
   *
   *  Type category: Integer
   */
  HTF_TYPE_INT16 = 6,

  /** @brief Signed 32-bit integer
   *
   *  Type category: Integer
   */
  HTF_TYPE_INT32 = 7,

  /** @brief Signed 64-bit integer
   *
   *  Type category: Integer
   */
  HTF_TYPE_INT64 = 8,

  /** @brief 32-bit floating point value
   *
   *  Type category: Floating point
   */
  HTF_TYPE_FLOAT = 9,

  /** @brief 64-bit floating point value
   *
   *  Type category: Floating point
   */
  HTF_TYPE_DOUBLE = 10,

  /** @brief Mapping of @eref{String} identifiers.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_STRING = 11,

  /** @brief Mapping of @eref{Attribute} identifiers.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_ATTRIBUTE = 12,

  /** @brief Mapping of @eref{Location} identifiers.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_LOCATION = 13,

  /** @brief Mapping of @eref{Region} identifiers.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_REGION = 14,

  /** @brief Mapping of @eref{Group} identifiers.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_GROUP = 15,

  /** @brief Mapping of @eref{Metric} identifiers.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_METRIC = 16,

  /** @brief Mapping of @eref{Comm} identifiers.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_COMM = 17,

  /** @brief Mapping of @eref{Parameter} identifiers.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_PARAMETER = 18,

  /** @brief Mapping of @eref{RmaWin} identifiers.
   *
   *  @since Version 1.2.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_RMA_WIN = 19,

  /** @brief Mapping of @eref{SourceCodeLocation} identifiers.
   *
   *  @since Version 1.5.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_SOURCE_CODE_LOCATION = 20,

  /** @brief Mapping of @eref{CallingContext} identifiers.
   *
   *  @since Version 1.5.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_CALLING_CONTEXT = 21,

  /** @brief Mapping of @eref{InterruptGenerator} identifiers.
   *
   *  @since Version 1.5.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_INTERRUPT_GENERATOR = 22,

  /** @brief Mapping of @eref{IoFile} identifiers.
   *
   *  @since Version 2.1.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_IO_FILE = 23,

  /** @brief Mapping of @eref{IoHandle} identifiers.
   *
   *  @since Version 2.1.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_IO_HANDLE = 24,

  /** @brief Mapping of @eref{LocationGroup} identifiers.
   *
   *  @since Version 3.0.
   *
   *  Type category: Definition reference
   */
  HTF_TYPE_LOCATION_GROUP = 25
};

#define NB_ATTRIBUTE_MAX 128
#define ATTRIBUTE_MAX_BUFFER_SIZE (sizeof(HTF(AttributeList)))
#define ATTRIBUTE_HEADER_SIZE (sizeof(uint16_t) + sizeof(HTF(AttributeRef)))

typedef struct AttributeData {
  uint16_t struct_size;
  AttributeRef ref;
  union AttributeValue value;
} __attribute__((packed)) AttributeData;

#define ATTRIBUTE_LIST_HEADER_SIZE (sizeof(int) + sizeof(uint16_t) + sizeof(uint8_t))
typedef struct AttributeList {
  int index;
  uint16_t struct_size;
  uint8_t nb_values;
  AttributeData attributes[NB_ATTRIBUTE_MAX];
} __attribute__((packed)) AttributeList;

static inline size_t get_value_size(htf_type_t t) {
  union AttributeValue u;
  switch (t) {
  case HTF_TYPE_NONE:
    return 0;
    break;
  case HTF_TYPE_UINT8:
    return sizeof(u.uint8);
    break;
  case HTF_TYPE_UINT16:
    return sizeof(u.uint16);
    break;
  case HTF_TYPE_UINT32:
    return sizeof(u.uint32);
    break;
  case HTF_TYPE_UINT64:
    return sizeof(u.uint64);
    break;
  case HTF_TYPE_INT8:
    return sizeof(u.int8);
    break;
  case HTF_TYPE_INT16:
    return sizeof(u.int16);
    break;
  case HTF_TYPE_INT32:
    return sizeof(u.int32);
    break;
  case HTF_TYPE_INT64:
    return sizeof(u.int64);
    break;
  case HTF_TYPE_FLOAT:
    return sizeof(u.float32);
    break;
  case HTF_TYPE_DOUBLE:
    return sizeof(u.float64);
    break;
  case HTF_TYPE_STRING:
    return sizeof(u.string_ref);
    break;
  case HTF_TYPE_ATTRIBUTE:
    return sizeof(u.attribute_ref);
    break;
  case HTF_TYPE_LOCATION:
    return sizeof(u.location_ref);
    break;
  case HTF_TYPE_REGION:
    return sizeof(u.region_ref);
    break;
  case HTF_TYPE_GROUP:
    return sizeof(u.group_ref);
    break;
  case HTF_TYPE_METRIC:
    return sizeof(u.metric_ref);
    break;
  case HTF_TYPE_COMM:
    return sizeof(u.comm_ref);
    break;
  case HTF_TYPE_PARAMETER:
    return sizeof(u.parameter_ref);
    break;
  case HTF_TYPE_RMA_WIN:
    return sizeof(u.rma_win_ref);
    break;
  case HTF_TYPE_SOURCE_CODE_LOCATION:
    return sizeof(u.source_code_location_ref);
    break;
  case HTF_TYPE_CALLING_CONTEXT:
    return sizeof(u.calling_context_ref);
    break;
  case HTF_TYPE_INTERRUPT_GENERATOR:
    return sizeof(u.interrupt_generator_ref);
    break;
  case HTF_TYPE_IO_FILE:
    return sizeof(u.io_file_ref);
    break;
  case HTF_TYPE_IO_HANDLE:
    return sizeof(u.io_handle_ref);
    break;
  case HTF_TYPE_LOCATION_GROUP:
    return sizeof(u.location_group_ref);
    break;
  }
  return 0;
}
#ifdef __cplusplus
}; /* namespace htf */
#endif

static inline void htf_attribute_list_push_data(HTF(AttributeList) * l, HTF(AttributeData) * data) {
  uintptr_t offset = l->struct_size;
  htf_assert(offset + data->struct_size <= ATTRIBUTE_MAX_BUFFER_SIZE);
  uintptr_t addr = ((uintptr_t)l) + offset;
  memcpy((void*)addr, data, data->struct_size);
  l->struct_size += data->struct_size;
  l->nb_values++;
}

static inline void htf_attribute_list_pop_data(HTF(AttributeList) * l,
                                               HTF(AttributeData) * data,
                                               uint16_t* current_offset) {
  uintptr_t addr = ((uintptr_t)&l->attributes[0]) + (*current_offset);
  HTF(AttributeData)* attr_data = (HTF(AttributeData)*)addr;
  uint16_t struct_size = attr_data->struct_size;

  htf_assert(struct_size + (*current_offset) <= ATTRIBUTE_MAX_BUFFER_SIZE);
  htf_assert(struct_size + (*current_offset) <= l->struct_size);

  memcpy(data, attr_data, struct_size);
  data->struct_size = struct_size;
  *current_offset += struct_size;
}

static inline void htf_attribute_list_init(HTF(AttributeList) * l) {
  l->index = -1;
  l->nb_values = 0;
  l->struct_size = ATTRIBUTE_LIST_HEADER_SIZE;
}

static inline void htf_attribute_list_finalize(HTF(AttributeList) * l __attribute__((unused))) {}

static inline int htf_attribute_list_add_attribute(HTF(AttributeList) * list,
                                                   HTF(AttributeRef) attribute,
                                                   size_t data_size,
                                                   HTF(AttributeValue) value) {
  if (list->nb_values + 1 >= NB_ATTRIBUTE_MAX) {
    htf_warn("[HTF] too many attributes\n");
    return -1;
  }
  HTF(AttributeData) d;
  d.ref = attribute;
  d.value = value;
  d.struct_size = ATTRIBUTE_HEADER_SIZE + data_size;

  htf_attribute_list_push_data(list, &d);
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

extern void htf_print_attribute_value(HTF(Thread) * thread, HTF(AttributeData) * attr, HTF(htf_type_t) type);

extern void htf_print_event_attributes(HTF(Thread) * thread,
                                       struct C_CXX(htf_event_occurence, htf::EventOccurence) * e);

extern void htf_print_attribute_list(HTF(Thread) * thread, HTF(AttributeList) * l);

#ifdef __cplusplus
};
#endif
