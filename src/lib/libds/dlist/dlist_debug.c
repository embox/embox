/**
 * @file
 * @brief Paranoia checks of doubly-linked lists
 *
 * @date 05.12.2013
 * @author Eldar Abusalimov
 */
#include <inttypes.h>
#include <assert.h>

#include <util/dlist.h>

#if DLIST_DEBUG_VERSION
void __dlist_debug_check(const struct dlist_head *head) {
#ifndef NDEBUG
	const struct dlist_head *p = head->prev;
	const struct dlist_head *n = head->next;
	uintptr_t poison = head->poison;

	assertf(((!poison || (void *) ~poison == head) &&
			n->prev == head &&
			p->next == head),
			"\n"
			"head: %p, poison: %p, ~poison: %p,\n"
			"n: %p, n->prev: %p,\n"
			"p: %p, p->next: %p\n",
			head, (void *)poison, (void *) ~poison,
			n, n->prev,
			p, p->next);
#endif
}
#endif
