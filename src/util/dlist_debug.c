/**
 * @file
 * @brief Paranoia checks of doubly-linked lists
 *
 * @date 05.12.2013
 * @author Eldar Abusalimov
 */

#include <assert.h>
#include <util/dlist.h>

void __dlist_debug_check(const struct dlist_head *head) {
	const struct dlist_head *p = head->prev;
	const struct dlist_head *n = head->next;
	uintptr_t poison = head->poison;

	assert((!poison || (void *) ~poison == head) &&
		n->prev == head &&
		p->next == head,
			"\n"
			"head: %p, poison: %p\n"
			"prev: %p, n: %p, p: %p\n"
			"next: %p, n: %p, p: %p\n",
			head, (void *) poison,
			p, p->next, p->prev,
			n, n->next, n->prev);
}

