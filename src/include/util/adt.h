/**
 * @file
 * @brief TODO documentation for adt.h -- Eldar Abusalimov
 *
 * @date Sep 21, 2011
 * @author Eldar Abusalimov
 */

#ifndef UTIL_ADT_H_
#define UTIL_ADT_H_

#include <util/member.h>

#define adt_link_t(element_type, link_member) \
	union __attribute__ ((packed)) { \
		struct __attribute__ ((packed)) {                       \
			char __offset[offsetof(element_type, link_member)]; \
			member_typeof(element_type, link_member) type;      \
		} link[0];                                              \
		struct __attribute__ ((packed)) {                       \
			typeof(element_type) type;                          \
		} element[0];                                           \
	}

#define adt_to_link(element_ptr, adt_link) \
	(__adt_typeof_link(adt_link) *) \
		member_in((typeof(adt_link) *) (element_ptr), link[0].type)

#define adt_from_link(link_ptr, adt_link) \
	 (__adt_typeof_element(adt_link) *) \
	 	 member_out((__adt_typeof_link(adt_link) *) (link_ptr), \
	 			 typeof(adt_link), link[0].type)

#define __adt_typeof_link(adt_link) \
	member_typeof(adt_link, link[0].type)
#define __adt_typeof_element(adt_link) \
	member_typeof(adt_link, element[0].type)

#endif /* UTIL_ADT_H_ */
