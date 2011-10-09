/**
 * @file
 * @brief Implementation of methods in util/set.h
 *
 * @date Oct 1, 2011
 * @author Avdyukhin Dmitry
 */

#ifndef UTIL_SET_IMPL_H_
#define UTIL_SET_IMPL_H_

#include <stddef.h>
#include <assert.h>

struct set_link {
	struct set_link *left;
	struct set_link *right;
};

struct set {
	struct set_link *root;
};

typedef int (*set_comparator_t)(struct set_link *first, struct set_link *second);

static inline struct set *set_init(struct set *set) {
	assert(set != NULL);
	set->root = NULL;
	return set;
}

static inline struct set_link *set_link_init(struct set_link * link) {
	assert(link != NULL);
	link->left = link->right = NULL;
	return link;
}

static inline int set_empty(struct set *set) {
	assert(set != NULL);
	return set->root == NULL;
}

static inline struct set_link *set_find_link(struct set *set,
		struct set_link *link, set_comparator_t compare) {
	struct set_link * cur;
	int comp_res;
	assert(set != NULL);
	assert(link != NULL);

	cur = set->root;
	while (cur) {
		comp_res = compare(link, cur);
		if (comp_res == 0) {
			return cur;
		} else if (comp_res < 0) {
			cur = cur->left;
		} else {
			cur = cur->right;
		}
	}
	return NULL;
}

static inline int set_add_link(struct set *set,
		struct set_link *link, set_comparator_t compare) {
	struct set_link **cur;
	struct set_link *other;
	int comp_res;
	assert(set != NULL);
	assert(link != NULL);

	cur = &set->root;
	set_link_init(link);
	while (*cur) {
		comp_res = compare(link, *cur);
		if (comp_res == 0) {
			other = *cur;
			link->left = other->left;
			link->right = other->right;
			*cur = link;
			set_link_init(other);
			return 0;
		} else if (comp_res < 0) {
			cur = &(*cur)->left;
		} else {
			cur = &(*cur)->right;
		}
	}
	*cur = link;
	return 1;
}

static inline int set_remove_link(struct set *set,
		struct set_link *link, set_comparator_t compare) {
	struct set_link **del_pos;
	struct set_link *deleted;

	/* position, where node can be inserted to be deleted easily */
	struct set_link **other_pos;
	/* element, what possible take deleted one's position */
	struct set_link *other;

	int comp_res;
	assert(set != NULL);
	assert(link != NULL);

	del_pos = &set->root;
	while (*del_pos) {
		comp_res = compare(link, *del_pos);
		if (comp_res == 0) {
			break;
		} else
		if (comp_res < 0) {
			del_pos = &(*del_pos)->left;
		} else {
			del_pos = &(*del_pos)->right;
		}
	}
	deleted = *del_pos;
	if (deleted == NULL) {
		return 0;
	}

	if (!deleted->left || !deleted->right) {
		/* There is less then two children of deleted node */
		*del_pos = deleted->left ? deleted->left : deleted->right;
	} else {
		/* Give element from the right subtree */
		other_pos = &deleted->right;
		while ((*other_pos)->left) {
			other_pos = &(*other_pos)->left;
		}
		other = *other_pos;
		*other_pos = other->right;
		other->left = deleted->left;
		other->right = deleted->right;
		*del_pos = other;
	}
	set_link_init(deleted);
	return 1;
}

static inline struct set_link *set_min_link(struct set *set) {
	struct set_link *result;
	assert(set != NULL);

	result = set->root;
	if (!result) {
		return NULL;
	}
	while (result->left) {
		result = result->left;
	}
	return result;
}

static inline struct set_link *set_max_link(struct set *set) {
	struct set_link *result;
	assert(set != NULL);

	result = set->root;
	if (!result) {
		return NULL;
	}
	while (result->right) {
		result = result->right;
	}
	return result;
}

#endif /* UTIL_SET_IMPL_H_ */
