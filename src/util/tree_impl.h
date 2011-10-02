/**
 * @file
 * @brief Implementation of methods in util/tree.h
 *
 * @date Oct 1, 2011
 * @author Avdyukhin Dmitry
 */

#ifndef UTIL_TREE_IMPL_H_
#define UTIL_TREE_IMPL_H_

static inline struct tree *tree_init(struct tree *tree) {
	assert(tree != NULL);
	tree->root = NULL;
	return tree;
}

static inline struct tree_link *tree_link_init(struct tree_link * link) {
	assert(link != NULL);
	link->left = link->right = NULL;
	return link;
}

static inline bool tree_empty(struct tree *tree) {
	assert(tree != NULL);
	return tree->root == NULL;
}

static inline struct tree_link *tree_find_link(struct tree *tree,
		struct tree_link *link, tree_comparator_t compare) {
	struct tree_link * cur;
	int comp_res;
	assert(tree != NULL);
	assert(link != NULL);

	cur = tree->root;
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

static inline bool tree_add_link(struct tree *tree,
		struct tree_link *link, tree_comparator_t compare) {
	struct tree_link **cur;
	struct tree_link *other;
	int comp_res;
	assert(tree != NULL);
	assert(link != NULL);

	cur = &tree->root;
	tree_link_init(link);
	while (*cur) {
		comp_res = compare(link, *cur);
		if (comp_res == 0) {
			other = *cur;
			link->left = other->left;
			link->right = other->right;
			*cur = link;
			tree_link_init(other);
			return false;
		} else if (comp_res < 0) {
			cur = &(*cur)->left;
		} else {
			cur = &(*cur)->right;
		}
	}
	*cur = link;
	return true;
}

static inline bool tree_remove_link(struct tree *tree,
		struct tree_link *link, tree_comparator_t compare) {
	struct tree_link **del_pos;
	struct tree_link *deleted;

	/* position, where node can be inserted to be deleted easily */
	struct tree_link **other_pos;
	/* element, what possible take deleted one's position */
	struct tree_link *other;

	int comp_res;
	assert(tree != NULL);
	assert(link != NULL);

	del_pos = &tree->root;
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
		return false;
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
	tree_link_init(deleted);
	return true;
}

static inline struct tree_link *tree_min_link(struct tree *tree) {
	struct tree_link *result;
	assert(tree != NULL);

	result = tree->root;
	if (!result) {
		return NULL;
	}
	while (result->left) {
		result = result->left;
	}
	return result;
}

static inline struct tree_link *tree_max_link(struct tree *tree) {
	struct tree_link *result;
	assert(tree != NULL);

	result = tree->root;
	if (!result) {
		return NULL;
	}
	while (result->right) {
		result = result->right;
	}
	return result;
}

#endif /* UTIL_TREE_IMPL_H_ */
