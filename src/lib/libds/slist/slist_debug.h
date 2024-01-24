/**
 * @file
 * @brief Debug implementation of single linked list utility.
 *
 * @date 17.04.11
 * @author Eldar Abusalimov
 */

#ifndef UTIL_SLIST_DEBUG_H_
#define UTIL_SLIST_DEBUG_H_

#include <assert.h>
#include <stddef.h>

struct slist;
struct slist_link;
struct __slist_link;

struct slist {
	unsigned int poison;
	int offset;
	struct __slist_link l;
};

struct slist_link {
	unsigned int poison;
	struct slist *slist;
	struct __slist_link l;
};

#define __SLIST_POISON       0x0f00
#define __SLIST_LINK_POISON  0x0b0b

#define __SLIST_INIT(slist) \
	{                                        \
		.poison = __SLIST_POISON,             \
		.offset = -1,                        \
		.l = __SLIST_LINK_INIT__(&(slist)->l), \
	}

#define __SLIST_LINK_INIT(link) \
	{                                        \
		.poison = __SLIST_LINK_POISON,        \
		.slist = NULL,                        \
		.l = __SLIST_LINK_INIT__(&(link)->l), \
	}

#define __slist_check(expr) \
	({ \
		typeof(expr) __slist_expr = (expr); \
		assert(__slist_expr != NULL);       \
		__slist_expr;                       \
	})

#endif /* UTIL_SLIST_DEBUG_H_ */
