/**
 * @file
 * @brief Implementation of methods in util/set.h
 *
 * @date Oct 1, 2011
 * @author Avdyukhin Dmitry
 */

#ifndef UTIL_SET_IMPL_C_
#define UTIL_SET_IMPL_C_

#include <stddef.h>
#include <assert.h>
#include <util/set.h>

inline struct set *set_init(struct set *set) {
	assert(set != NULL);
	set->root = NULL;
	return set;
}

inline struct set_link *set_link_init(struct set_link * link) {
	assert(link != NULL);
	link->left = link->right = link->par = NULL;
	return link;
}

inline int set_empty(struct set *set) {
	assert(set != NULL);
	return set->root == NULL;
}

inline struct set_link *set_find_link(struct set *set,
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

inline int set_add_link(struct set *set,
		struct set_link *link, set_comparator_t compare) {
	struct set_link *cur, **pos;
	int comp_res;
	assert(set != NULL);
	assert(link != NULL);
	set_link_init(link);
	if (set->root == NULL) {
		set->root = link;
		return 1;
	}
	pos = &set->root;
	while (*pos) {
		cur = *pos;
		comp_res = compare(link, cur);
		if (comp_res == 0) {
//			link->left = cur->left;
//			link->right = cur->right;
//			link->par = cur->par;
//			*pos = link;
//			set_link_init(cur);
			return 0;
		} else {
			if (comp_res < 0) {
				pos = &cur->left;
			} else {
				pos = &cur->right;
			}
		}
	}
	link->par = cur;
	*pos = link;
	return 1;
}

inline int set_remove_link(struct set *set,
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
	while (*del_pos != NULL) {
		comp_res = compare(link, *del_pos);
		if (comp_res == 0) {
			break;
		} else {
			if (comp_res < 0) {
				del_pos = &(*del_pos)->left;
			} else {
				del_pos = &(*del_pos)->right;
			}
		}
	}
	deleted = *del_pos;
	if (deleted == NULL) {
		return 0;
	}

	if (deleted->left == NULL || deleted->right == NULL) {
		/* There is less then two children of deleted node */
		*del_pos = (deleted->left != NULL) ? deleted->left : deleted->right;
		if (*del_pos != NULL) {
			(*del_pos)->par = deleted->par;
		}
	} else {
		/* Give element from the right subtree */
		other_pos = &deleted->right;
		while ((*other_pos)->left != NULL) {
			other_pos = &(*other_pos)->left;
		}
		other = *other_pos;
		*other_pos = other->right;
		if (other->right != NULL) {
			if (other->par != deleted) {
				other->right->par = other->par;
			}
		}
		other->left = deleted->left;
		if (other->left != NULL) {
			other->left->par = other;
		}
		other->right = deleted->right;
		if (other->right != NULL) {
			other->right->par = other;
		}
		other->par = deleted->par;
		*del_pos = other;
	}
	set_link_init(deleted);
	return 1;
}

/* Operations on subtree, represented with their root nodes. */

inline struct set_link *__set_min_link(struct set_link *root) {
	struct set_link *result;
	result = root;
	if (result == NULL) {
		return NULL;
	}
	while (result->left) {
		result = result->left;
	}
	return result;
}

inline struct set_link *set_min_link(struct set *set) {
	assert(set != NULL);
	return __set_min_link(set->root);
}

inline struct set_link *__set_max_link(struct set_link *root) {
	struct set_link *result;
	result = root;
	if (result == NULL) {
		return NULL;
	}
	while (result->right) {
		result = result->right;
	}
	return result;
}

inline struct set_link *set_max_link(struct set *set) {
	assert(set != NULL);
	return __set_max_link(set->root);
}

inline struct set_link *set_next_link(struct set_link *link) {
	struct set_link *par;
	assert(link != NULL);
	if (link->right != NULL) {
		/* Search for the most left element in right subtree */
		return __set_min_link(link->right);
	} else {
		/* Search for the first parent, placed righter than its son. */
		par = link->par;
		while (par && par->right == link) {
			link = par;
			par = par->par;
		}
		return par;
	}
}

inline struct set_link *set_prev_link(struct set_link *link) {
	struct set_link *par;
	assert(link != NULL);
	if (link->left != NULL) {
		/* Search for the most left element in right subtree */
		return __set_max_link(link->left);
	} else {
		/* Search for the first parent, placed righter than its son. */
		par = link->par;
		while (par && par->left == link) {
			link = par;
			par = par->par;
		}
		return par;
	}
}

#endif /* UTIL_SET_IMPL_C_ */
