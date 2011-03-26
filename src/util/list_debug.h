/**
 * @file
 * @brief Debug implementation of linked list utility.
 *
 * @date Mar 26, 2011
 * @author Eldar Abusalimov
 */

#ifndef UTIL_LIST_DEBUG_H_
#define UTIL_LIST_DEBUG_H_

#include <assert.h>
#include <stddef.h>

#include <util/structof.h>

struct list;
struct list_link;
struct __list_link;

struct list {
	unsigned int poison;
	int offset;
	struct __list_link l;
};

struct list_link {
	unsigned int poison;
	struct list *list;
	struct __list_link l;
};

#define __LIST_POISON       0x0f00
#define __LIST_LINK_POISON  0x0b0b

#define __LIST_INIT(list) \
	{                                        \
		.poison = __LIST_POISON,             \
		.offset = -1,                        \
		.l = __LIST_LINK_INIT__(&(list)->l), \
	}

#define __LIST_LINK_INIT(link) \
	{                                        \
		.poison = __LIST_LINK_POISON,        \
		.list = NULL,                        \
		.l = __LIST_LINK_INIT__(&(link)->l), \
	}

#define __list_check(expr) \
	__extension__ ({                       \
		typeof(expr) __list_expr = (expr); \
		assert(__list_expr != NULL);       \
		__list_expr;                       \
	})

/* Most of macros are defined through a corresponding _link method. */

#define __list_link_element(link, type, m_link) \
	structof(__list_check(link), type, m_link)

#define __list_alone(element, m_link) \
	list_alone_link(&__list_check(element)->m_link)

#define __list_first(list, type, m_link) \
	__list_link_element_check(list_first_link(list), type, m_link)

#define __list_last(list, type, m_link) \
	__list_link_element_check(list_last_link(list), type, m_link)

#define __list_link_element_check(link, type, m_link) \
	__extension__ ({ \
		struct list_link *__list_link__ = (link); \
		__list_link__ ? list_link_element(__list_link__, type, m_link) : NULL;\
	})

#define __list_add_first(element, list, m_link) \
	list_add_first_link(&__list_check(element)->m_link, list)

#define __list_add_last(element, list, m_link) \
	list_add_last_link(&__list_check(element)->m_link, list)

#define __list_insert_before(element, list_element, m_link) \
	list_insert_before_link(&__list_check(element)->m_link, &(list_element)->m_link)

#define __list_insert_after(element, list_element, m_link) \
	list_insert_after_link(&__list_check(element)->m_link, &(list_element)->m_link)

#define __list_remove_first(list, type, m_link) \
	__list_link_element_check(list_remove_first_link(list), type, m_link)

#define __list_remove_last(list, type, m_link) \
	__list_link_element_check(list_remove_last_link(list), type, m_link)

#define __list_remove(element, m_link) \
	list_remove_link(&__list_check(element)->m_link)

#endif /* UTIL_LIST_DEBUG_H_ */
