#include "htf.h"
#include "htf_archive.h"
#include "htf_attribute.h"
#include "htf_read.h"

#include <inttypes.h>

#define UNUSED __attribute__((unused))

void htf_print_attribute_ref(struct htf_thread* thread,
			     htf_attribute_ref_t ref) {
  struct htf_attribute* attr = htf_archive_get_attribute(thread->archive,
							 ref);
  if(attr) {
    struct htf_string* attr_string = htf_archive_get_string(thread->archive,
							    attr->name);
    if(attr_string) {
      printf("\"%s\" <%d>", attr_string->str, ref);
      return;
    }
  }

  printf("INVALID <%d>", ref);
}

static enum htf_type_enum _guess_attribute_size(struct htf_attribute_data *attr) {
  uint16_t data_size = attr->struct_size - ATTRIBUTE_HEADER_SIZE;
  switch(data_size) {
  case 1: return HTF_TYPE_UINT8;
  case 2: return HTF_TYPE_UINT16;
  case 4: return HTF_TYPE_UINT32;
  case 8: return HTF_TYPE_UINT64;
  default: return HTF_TYPE_NONE;
  }
}

static void _htf_print_string(struct htf_thread* thread, htf_string_ref_t string_ref) {
  struct htf_string* str = htf_archive_get_string(thread->archive, string_ref);
  if(str)
    printf("%s <%d>", str->str, string_ref);
  else
    printf("INVALID_STRING <%d>", string_ref);
}

static void _htf_print_attribute_ref(struct htf_thread* thread, htf_attribute_ref_t attribute_ref) {
  struct htf_attribute* attr = htf_archive_get_attribute(thread->archive, attribute_ref);
  if(attr)
    printf("attribute <%d>", attribute_ref);
  else
    printf("INVALID_ATTRIBUTE <%d>", attribute_ref);
}

static void _htf_print_location(struct htf_thread* thread, htf_ref_t location_ref) {
  struct htf_location* attr = htf_archive_get_location(thread->archive, location_ref);
  if(attr)
    printf("location <%d>", location_ref);
  else
    printf("INVALID_LOCATION <%d>", location_ref);
}

static void _htf_print_region(struct htf_thread* thread, htf_region_ref_t region_ref) {
  struct htf_region* attr = htf_archive_get_region(thread->archive, region_ref);
  if(attr)
    printf("region <%d>", region_ref);
  else
    printf("INVALID_REGION <%d>", region_ref);
}


static void _htf_print_group(struct htf_thread* thread UNUSED, htf_ref_t group_ref) {
  printf("group <%d>", group_ref);
}


static void _htf_print_metric(struct htf_thread* thread UNUSED, htf_ref_t metric_ref) {
  printf("metric <%d>", metric_ref);
}


static void _htf_print_comm(struct htf_thread* thread UNUSED, htf_ref_t comm_ref) {
  printf("comm <%d>", comm_ref);
}


static void _htf_print_parameter(struct htf_thread* thread UNUSED, htf_ref_t parameter_ref) {
  printf("parameter <%d>", parameter_ref);
}


static void _htf_print_rma_win(struct htf_thread* thread UNUSED, htf_ref_t rma_win_ref) {
  printf("rma_win <%d>", rma_win_ref);
}


static void _htf_print_source_code_location(struct htf_thread* thread UNUSED, htf_ref_t source_code_location_ref) {
  printf("source_code_location <%d>", source_code_location_ref);
}


static void _htf_print_calling_context(struct htf_thread* thread UNUSED, htf_ref_t calling_context_ref) {
  printf("calling_context <%d>", calling_context_ref);
}


static void _htf_print_interrupt_generator(struct htf_thread* thread UNUSED, htf_ref_t interrupt_generator_ref) {
  printf("interrupt_generator <%d>", interrupt_generator_ref);
}


static void _htf_print_io_file(struct htf_thread* thread UNUSED, htf_ref_t io_file_ref) {
  printf("io_file <%d>", io_file_ref);
}


static void _htf_print_io_handle(struct htf_thread* thread UNUSED, htf_ref_t io_handle_ref) {
  printf("io_handle <%d>", io_handle_ref);
}


static void _htf_print_location_group(struct htf_thread* thread UNUSED, htf_ref_t location_group_ref) {
  printf("location_group <%d>", location_group_ref);
}

void _htf_print_attribute_value(struct htf_thread* thread,
				struct htf_attribute_data *attr,
				htf_type_t type) {
  switch(type) {
  case HTF_TYPE_NONE: printf("NONE"); break;
  case HTF_TYPE_UINT8: printf("%u", attr->value.uint8); break;
  case HTF_TYPE_UINT16: printf("%u", attr->value.uint16); break;
  case HTF_TYPE_UINT32: printf("%u", attr->value.uint32); break;
  case HTF_TYPE_UINT64: printf("%"PRIu64, attr->value.uint64); break;
  case HTF_TYPE_INT8: printf("%d", attr->value.int8); break;
  case HTF_TYPE_INT16: printf("%d", attr->value.int16); break;
  case HTF_TYPE_INT32: printf("%d", attr->value.int32); break;
  case HTF_TYPE_INT64: printf("%" PRId64, attr->value.int64); break;
  case HTF_TYPE_FLOAT: printf("%f", attr->value.float32); break;
  case HTF_TYPE_DOUBLE: printf("%lf", attr->value.float64); break;
  case HTF_TYPE_STRING: _htf_print_string(thread, attr->value.string_ref); break;
  case HTF_TYPE_ATTRIBUTE: _htf_print_attribute_ref(thread, attr->value.attribute_ref); break;
  case HTF_TYPE_LOCATION: _htf_print_location(thread, attr->value.location_ref); break;
  case HTF_TYPE_REGION: _htf_print_region(thread, attr->value.region_ref); break;
  case HTF_TYPE_GROUP:  _htf_print_group(thread, attr->value.group_ref); break;
  case HTF_TYPE_METRIC:  _htf_print_metric(thread, attr->value.metric_ref); break;
  case HTF_TYPE_COMM:  _htf_print_comm(thread, attr->value.comm_ref); break;
  case HTF_TYPE_PARAMETER:  _htf_print_parameter(thread, attr->value.parameter_ref); break;
  case HTF_TYPE_RMA_WIN:  _htf_print_rma_win(thread, attr->value.rma_win_ref); break;
  case HTF_TYPE_SOURCE_CODE_LOCATION:  _htf_print_source_code_location(thread, attr->value.source_code_location_ref); break;
  case HTF_TYPE_CALLING_CONTEXT:  _htf_print_calling_context(thread, attr->value.calling_context_ref); break;
  case HTF_TYPE_INTERRUPT_GENERATOR:  _htf_print_interrupt_generator(thread, attr->value.interrupt_generator_ref); break;
  case HTF_TYPE_IO_FILE:  _htf_print_io_file(thread, attr->value.io_file_ref); break;
  case HTF_TYPE_IO_HANDLE:  _htf_print_io_handle(thread, attr->value.io_handle_ref); break;
  case HTF_TYPE_LOCATION_GROUP:  _htf_print_location_group(thread, attr->value.location_group_ref); break;
  }
}

void _htf_print_attribute(struct htf_thread* thread,
			  struct htf_attribute_data *attr) {
  char* invalid_attr_string = "INVALID";
  char* attr_string = invalid_attr_string;
  enum htf_type_enum type = _guess_attribute_size(attr);

  struct htf_attribute* a = htf_archive_get_attribute(thread->archive,
  						      attr->ref);
  if(a) {
    struct htf_string* str = htf_archive_get_string(thread->archive,
						    a->name);
    if(str) {
      attr_string = str->str;
    }

    type = a->type;
  }

  printf("%s <%d>: ", attr_string, attr->ref);
  _htf_print_attribute_value(thread, attr, type);
}

void htf_print_attribute_list(struct htf_thread* thread,
			      struct htf_attribute_list* l) {
  if(l == NULL)
    return;
  printf(" { ");
  uint16_t pos = 0;
  for(int i=0; i<l->nb_values; i++) {
    struct htf_attribute_data attr;
    htf_attribute_list_pop_data(l, &attr, &pos);
    htf_assert(ATTRIBUTE_LIST_HEADER_SIZE + pos <= l->struct_size);

    if(i>0) printf(", ");
    _htf_print_attribute(thread,&attr);
  }
  printf("}");
}

void htf_print_event_attributes(struct htf_thread* thread,
				struct htf_event_occurence* e) {
  htf_print_attribute_list(thread, e->attributes);
}


/* -*-
   mode: c;
   c-file-style: "k&r";
   c-basic-offset 2;
   tab-width 2 ;
   indent-tabs-mode nil
   -*- */
