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

#define adt_element_t(element_type, link_member) \
	union __attribute__ ((packed)) { \
		typeof(element_type) element;                           \
		struct __attribute__ ((packed)) {                       \
			char __offset[offsetof(element_type, link_member)]; \
			member_typeof(element_type, link_member) m_link;    \
		} /* unnamed */;                                        \
	}

#define adt_to_link(element_ptr, adt) \
	member_in((typeof(adt) *) (element_ptr), m_link)

#define adt_from_link(link_ptr, adt) \
	 &(member_out((link_ptr), typeof(adt), m_link)->element)

#endif /* UTIL_ADT_H_ */
