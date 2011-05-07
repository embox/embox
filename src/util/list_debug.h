/**
 * @file
 * @brief Debug implementation of doubly-linked list utility.
 *
 * @date 26.03.11
 * @author Eldar Abusalimov
 */

#ifndef UTIL_LIST_DEBUG_H_
#define UTIL_LIST_DEBUG_H_

#include <assert.h>
#include <stddef.h>

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

#endif /* UTIL_LIST_DEBUG_H_ */
