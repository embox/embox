/**
 * @file
 * @brief TODO documentation for slist.h -- Eldar Abusalimov
 *
 * @date 16.04.11
 * @author Eldar Abusalimov
 */

#ifndef UTIL_SLIST_H_
#define UTIL_SLIST_H_

#include __impl_x(util/slist_impl.h)

struct slist;

struct slist_link;

#define SLIST_INIT(slist) \
	  __SLIST_INIT(slist)

#define SLIST_LINK_INIT(link) \
	  __SLIST_LINK_INIT(link)

extern struct slist *slist_init(struct slist *list);
extern struct slist_link *slist_link_init(struct slist_link *link);

extern int slist_empty(struct slist *list);

#define slist_alone(element, link_member) \
	  __slist_alone(element, link_member)

#define slist_foreach(element, slist, link_member) \
	  __slist_foreach(element, slist, link_member)

#define slist_first(slist, element_type, link_member) \
	  __slist_first(slist, element_type, link_member)

#define slist_add_first(element, slist, link_member) \
	  __slist_add_first(element, slist, link_member)

#define slist_insert_after(element, after_element, link_member) \
	  __slist_insert_after(element, after_element, link_member)

#define slist_remove_first(slist, element_type, link_member) \
	  __slist_remove_first(slist, element_type, link_member)

#define slist_link_element(link, element_type, link_member) \
	  __slist_link_element(link, element_type, link_member)

extern int slist_alone_link(struct slist_link *link);

#define slist_foreach_link(link, slist) \
	  __slist_foreach_link(link, slist)

extern struct slist_link *slist_first_link(struct slist *list);

extern void slist_add_first_link(struct slist_link *new_link,
		struct slist *list);

extern void slist_insert_after_link(struct slist_link *new_link,
		struct slist_link *link);

extern struct slist_link *slist_remove_first_link(struct slist *list);

#endif /* UTIL_SLIST_H_ */
