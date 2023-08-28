#include "htf.h"
#include "htf_attribute.h"
#include "htf_read.h"

void htf_print_attribute_value(struct htf_thread* thread,
			       struct htf_attribute_data *attr) {
  printf("%x: %x", attr->ref, attr->value.uint32);
}

void htf_print_event_attributes(struct htf_thread* thread,
				struct htf_event_occurence* e) {
  struct htf_attribute_list* l = e->attributes;
  if(l == NULL)
    return;
  printf("Attributes (%u) { ", l->nb_values);
  uint16_t pos = 0;
  for(int i=0; i<l->nb_values; i++) {
    struct htf_attribute_data attr;
    htf_attribute_list_pop_data(l, &attr, &pos);
    htf_assert(ATTRIBUTE_LIST_HEADER_SIZE + pos <= l->struct_size);

    htf_print_attribute_value(thread,&attr);
  }
  printf("}");
}

