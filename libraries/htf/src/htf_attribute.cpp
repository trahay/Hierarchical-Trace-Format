/*
 * Copyright (C) Telecom SudParis
 * See LICENSE in top-level directory.
 */

#include "htf/htf_attribute.h"
#include "htf/htf.h"
#include "htf/htf_archive.h"
#include "htf/htf_read.h"

#include <inttypes.h>

#define UNUSED __attribute__((unused))

namespace htf {
void Thread::printAttribute(AttributeRef ref) const {
  const Attribute* attr = archive->getAttribute(ref);
  if (attr) {
    const String* attr_string = archive->getString(attr->name);
    if (attr_string) {
      printf("\"%s\" <%d>", attr_string->str, ref);
      return;
    }
  }

  printf("INVALID <%d>", ref);
}

static enum AttributeType _guess_attribute_size(const AttributeData* attr) {
  uint16_t data_size = attr->struct_size - ATTRIBUTE_HEADER_SIZE;
  switch (data_size) {
  case 1:
    return HTF_TYPE_UINT8;
  case 2:
    return HTF_TYPE_UINT16;
  case 4:
    return HTF_TYPE_UINT32;
  case 8:
    return HTF_TYPE_UINT64;
  default:
    return HTF_TYPE_NONE;
  }
}

void Thread::printString(StringRef string_ref) const {
  auto* str = archive->getString(string_ref);
  if (str)
    printf("%s <%d>", str->str, string_ref);
  else
    printf("INVALID_STRING <%d>", string_ref);
}

void Thread::printAttributeRef(AttributeRef attribute_ref) const {
  auto* attr = archive->getAttribute(attribute_ref);
  if (attr)
    printf("attribute <%d>", attribute_ref);
  else
    printf("INVALID_ATTRIBUTE <%d>", attribute_ref);
}

void Thread::printLocation(Ref location_ref) const {
  auto* attr = archive->getLocation(location_ref);
  if (attr)
    printf("location <%d>", location_ref);
  else
    printf("INVALID_LOCATION <%d>", location_ref);
}

void Thread::printRegion(Ref region_ref) const {
  auto* attr = archive->getRegion(region_ref);
  if (attr)
    printf("region <%d>", region_ref);
  else
    printf("INVALID_REGION <%d>", region_ref);
}

static void _htf_print_group(Ref group_ref) {
  printf("group <%d>", group_ref);
}

static void _htf_print_metric(Ref metric_ref) {
  printf("metric <%d>", metric_ref);
}

static void _htf_print_comm(Ref comm_ref) {
  printf("comm <%d>", comm_ref);
}

static void _htf_print_parameter(Ref parameter_ref) {
  printf("parameter <%d>", parameter_ref);
}

static void _htf_print_rma_win(Ref rma_win_ref) {
  printf("rma_win <%d>", rma_win_ref);
}

static void _htf_print_source_code_location(Ref source_code_location_ref) {
  printf("source_code_location <%d>", source_code_location_ref);
}

static void _htf_print_calling_context(Ref calling_context_ref) {
  printf("calling_context <%d>", calling_context_ref);
}

static void _htf_print_interrupt_generator(Ref interrupt_generator_ref) {
  printf("interrupt_generator <%d>", interrupt_generator_ref);
}

static void _htf_print_io_file(Ref io_file_ref) {
  printf("io_file <%d>", io_file_ref);
}

static void _htf_print_io_handle(Ref io_handle_ref) {
  printf("io_handle <%d>", io_handle_ref);
}

static void _htf_print_location_group(Ref location_group_ref) {
  printf("location_group <%d>", location_group_ref);
}

void Thread::printAttributeValue(const struct AttributeData* attr, htf_type_t type) const {
  switch (type) {
  case HTF_TYPE_NONE:
    printf("NONE");
    break;
  case HTF_TYPE_UINT8:
    printf("%u", attr->value.uint8);
    break;
  case HTF_TYPE_UINT16:
    printf("%u", attr->value.uint16);
    break;
  case HTF_TYPE_UINT32:
    printf("%u", attr->value.uint32);
    break;
  case HTF_TYPE_UINT64:
    printf("%" PRIu64, attr->value.uint64);
    break;
  case HTF_TYPE_INT8:
    printf("%d", attr->value.int8);
    break;
  case HTF_TYPE_INT16:
    printf("%d", attr->value.int16);
    break;
  case HTF_TYPE_INT32:
    printf("%d", attr->value.int32);
    break;
  case HTF_TYPE_INT64:
    printf("%" PRId64, attr->value.int64);
    break;
  case HTF_TYPE_FLOAT:
    printf("%f", attr->value.float32);
    break;
  case HTF_TYPE_DOUBLE:
    printf("%lf", attr->value.float64);
    break;
  case HTF_TYPE_STRING:
    printString(attr->value.string_ref);
    break;
  case HTF_TYPE_ATTRIBUTE:
    printAttributeRef(attr->value.attribute_ref);
    break;
  case HTF_TYPE_LOCATION:
    printLocation(attr->value.location_ref);
    break;
  case HTF_TYPE_REGION:
    printRegion(attr->value.region_ref);
    break;
  case HTF_TYPE_GROUP:
    _htf_print_group(attr->value.group_ref);
    break;
  case HTF_TYPE_METRIC:
    _htf_print_metric(attr->value.metric_ref);
    break;
  case HTF_TYPE_COMM:
    _htf_print_comm(attr->value.comm_ref);
    break;
  case HTF_TYPE_PARAMETER:
    _htf_print_parameter(attr->value.parameter_ref);
    break;
  case HTF_TYPE_RMA_WIN:
    _htf_print_rma_win(attr->value.rma_win_ref);
    break;
  case HTF_TYPE_SOURCE_CODE_LOCATION:
    _htf_print_source_code_location(attr->value.source_code_location_ref);
    break;
  case HTF_TYPE_CALLING_CONTEXT:
    _htf_print_calling_context(attr->value.calling_context_ref);
    break;
  case HTF_TYPE_INTERRUPT_GENERATOR:
    _htf_print_interrupt_generator(attr->value.interrupt_generator_ref);
    break;
  case HTF_TYPE_IO_FILE:
    _htf_print_io_file(attr->value.io_file_ref);
    break;
  case HTF_TYPE_IO_HANDLE:
    _htf_print_io_handle(attr->value.io_handle_ref);
    break;
  case HTF_TYPE_LOCATION_GROUP:
    _htf_print_location_group(attr->value.location_group_ref);
    break;
  }
}

void Thread::printAttribute(const struct AttributeData* attr) const {
  const char* attr_string = "INVALID";
  enum AttributeType type = _guess_attribute_size(attr);

  auto* a = archive->getAttribute(attr->ref);
  if (a) {
    auto* str = archive->getString(a->name);
    if (str) {
      attr_string = str->str;
    }

    type = static_cast<AttributeType>(a->type);
  }

  printf("%s <%d>: ", attr_string, attr->ref);
  printAttributeValue(attr, type);
}

void Thread::printAttributeList(const AttributeList* attribute_list) const {
  if (attribute_list == nullptr)
    return;
  printf(" { ");
  uint16_t pos = 0;
  for (int i = 0; i < attribute_list->nb_values; i++) {
    AttributeData attr;
    htf_attribute_list_pop_data(attribute_list, &attr, &pos);
    htf_assert(ATTRIBUTE_LIST_HEADER_SIZE + pos <= attribute_list->struct_size);

    if (i > 0)
      printf(", ");
    printAttribute(&attr);
  }
  printf("}");
}

void Thread::printEventAttribute(const struct EventOccurence* e) const {
  printAttributeList(e->attributes);
}
}  // namespace htf

void htf_print_attribute_value(htf::Thread* thread, htf::AttributeData* attr, htf::htf_type_t type) {
  thread->printAttributeValue(attr, type);
};

void htf_print_event_attributes(htf::Thread* thread, htf::EventOccurence* e) {
  thread->printEventAttribute(e);
};

void htf_print_attribute_list(htf::Thread* thread, htf::AttributeList* l) {
  thread->printAttributeList(l);
};
/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
