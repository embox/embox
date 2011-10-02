/**
 * @file
 * @brief Implementation of methods in util/tree.h
 *
 * @date Oct 1, 2011
 * @author Avdyukhin Dmitry
 */

#ifndef UTIL_TREE_IMPL_H_
#define UTIL_TREE_IMPL_H_

inline pnode tree_find_link(struct tree* tree, pnode link) {
	pnode cur;
	int comp_res;
	assert(tree != NULL);
	assert(link != NULL);
	cur = tree->root;
	while (cur) {
		comp_res = tree->comparator(link, cur);
		if (comp_res == 0) {
			return cur;
		} else
		if (comp_res < 0) {
			cur = cur->left;
		} else {
			cur = cur->right;
		}
	}
	return NULL;
}

inline bool tree_add_link(struct tree* tree, pnode link) {
	pnode* cur;
	pnode other;
	int comp_res;
	assert(tree != NULL);
	assert(link != NULL);
	cur = &tree->root;
	tree_link_init(link);
	while (*cur) {
		comp_res = tree->comparator(link, *cur);
		if (comp_res == 0) {
			other = *cur;
			link->left = other->left;
			link->right = other->right;
			*cur = link;
			tree_link_init(other);
			return false;
		} else
		if (comp_res < 0) {
			cur = &(*cur)->left;
		} else {
			cur = &(*cur)->right;
		}
	}
	*cur = link;
	return true;
}

inline bool tree_remove_link(struct tree* tree, pnode link) {
	pnode* del_pos;
	pnode deleted;
	// position, where node can be inserted to be deleted easily
	pnode* other_pos;
	// element, what possible take deleted one's position
	pnode other;
	int comp_res;
	assert(tree != NULL);
	assert(link != NULL);
	del_pos = &tree->root;
	while (*del_pos) {
		comp_res = tree->comparator(link, *del_pos);
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
		*del_pos = deleted->left ? deleted->left : deleted->right;
	} else {
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

inline pnode tree_min_link(struct tree *tree) {
	pnode result;
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

inline pnode tree_max_link(struct tree *tree) {
	pnode result;
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
