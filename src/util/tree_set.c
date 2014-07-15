/**
 * @file
 * @brief Implementation of methods in util/tree_set.h
 *
 * @date Oct 1, 2011
 * @author Avdyukhin Dmitry
 */

#include <stddef.h>
#include <assert.h>
#include <util/tree_set.h>

struct tree_set *tree_set_init(struct tree_set *tree_set) {
	assert(tree_set != NULL);
	tree_set->root = NULL;
	return tree_set;
}

struct tree_set_link *tree_set_link_init(struct tree_set_link * link) {
	assert(link != NULL);
	link->left = link->right = link->par = NULL;
	link->color = TREE_SET_NONE;
	return link;
}

int tree_set_empty(struct tree_set *tree_set) {
	assert(tree_set != NULL);
	return tree_set->root == NULL;
}

struct tree_set_link *tree_set_find_link(struct tree_set *tree_set,
		struct tree_set_link *link, tree_set_comparator_t compare) {
	struct tree_set_link * cur;
	int comp_res;
	assert(tree_set != NULL);
	assert(link != NULL);

	cur = tree_set->root;
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

struct tree_set_link *tree_set_lower_bound(struct tree_set *tree_set,
		struct tree_set_link *link, tree_set_comparator_t compare) {
	struct tree_set_link *cur;
	/* We will store current result here. */
	struct tree_set_link *res = NULL;
	int comp_res;
	assert(tree_set != NULL);
	assert(link != NULL);

	cur = tree_set->root;
	while (cur) {
		comp_res = compare(link, cur);
		if (comp_res >= 0) {
			res = cur;
			cur = cur->left;
		} else {
			cur = cur->right;
		}
	}
	return res;
}

struct tree_set_link *tree_set_upper_bound(struct tree_set *tree_set,
		struct tree_set_link *link, tree_set_comparator_t compare) {
	struct tree_set_link *cur;
	/* We will store current result here. */
	struct tree_set_link *res = NULL;
	int comp_res;
	assert(tree_set != NULL);
	assert(link != NULL);

	cur = tree_set->root;
	while (cur) {
		comp_res = compare(link, cur);
		if (comp_res > 0) {
			res = cur;
			cur = cur->left;
		} else {
			cur = cur->right;
		}
	}
	return res;
}

/**
 * Copy tree pointers and color from one link to another
 *         (except pointer from parent node to specified).
 * @param src - Source node of links.
 * @param dest - Destination node.
 */
static void tree_set_copy_links(struct tree_set_link *src, struct tree_set_link *dest) {
	assert(src != NULL);
	assert(dest != NULL);

	dest->left = src->left;
	dest->right = src->right;
	dest->par = src->par;
	dest->color = src->color;
	if (dest->right != NULL) {
		dest->right->par = dest;
	}
	if (dest->left != NULL) {
		dest->left->par = dest;
	}
}

/** Left rotation of tree node. */
static void tree_set_left_rotate(struct tree_set *tree, struct tree_set_link *link) {
	struct tree_set_link *par;
	struct tree_set_link *other;
	assert(link != NULL);
	other = link->right;
	assert(other != NULL);
	par = link->par;
	link->right = other->left;
	if (other->left != NULL) {
		other->left->par = link;
	}
	other->left = link;
	other->par = par;
	link->par = other;
	if (par != NULL) {
		if (link == par->left) {
			par->left = other;
		} else {
			par->right = other;
		}
	} else {
		tree->root = other;
	}
}

/** Right rotation of tree node. */
static void tree_set_right_rotate(struct tree_set *tree, struct tree_set_link *link) {
	struct tree_set_link *par;
	struct tree_set_link *other;
	assert(link != NULL);
	other = link->left;
	assert(other != NULL);
	par = link->par;
	link->left = other->right;
	if (other->right != NULL) {
		other->right->par = link;
	}
	other->right = link;
	other->par = par;
	link->par = other;
	if (par != NULL) {
		if (link == par->left) {
			par->left = other;
		} else {
			par->right = other;
		}
	} else {
		tree->root = other;
	}
}

/** Balance tree after insertion during walking up a tree. */
static void tree_set_insert_balance(struct tree_set *tree, struct tree_set_link *link) {
	struct tree_set_link *par2, *other, *par;
	assert(tree != NULL);
	assert(link != NULL);
	par = link->par;
	while (link != tree->root && par->color == TREE_SET_RED) {
		par2 = par->par;
		if (par == par2->left) {
			other = par2->right;
			assert(other);
			if (other->color == TREE_SET_RED) {
				par->color = TREE_SET_BLACK;
				other->color = TREE_SET_BLACK;
				par2->color = TREE_SET_RED;
				link = par2;
			} else {
				if (link == par->right) {
					link = par;
					tree_set_left_rotate(tree, par);
					par = link->par;
					par2 = par->par;
				}
				par->color = TREE_SET_BLACK;
				par2->color = TREE_SET_RED;
				tree_set_right_rotate(tree, par2);
			}
		} else {
			other = par2->left;
			assert(other);
			if (other->color == TREE_SET_RED) {
				par->color = TREE_SET_BLACK;
				other->color = TREE_SET_BLACK;
				par2->color = TREE_SET_RED;
				link = par2;
			} else {
				if (link == par->left) {
					link = par;
					tree_set_right_rotate(tree, par);
					par = link->par;
					par2 = par->par;
				}
				par->color = TREE_SET_BLACK;
				par2->color = TREE_SET_RED;
				tree_set_left_rotate(tree, par2);
			}
		}
		par = link->par;
	}
	tree->root->color = TREE_SET_BLACK;
}

int tree_set_add_link(struct tree_set *tree_set,
		struct tree_set_link *link, tree_set_comparator_t compare) {
	struct tree_set_link *cur, **pos;
	int comp_res;
	assert(tree_set != NULL);
	assert(link != NULL);
	tree_set_link_init(link);
	if (tree_set->root == NULL) {
		tree_set->root = link;
		link->color = TREE_SET_BLACK;
		return 1;
	}
	pos = &tree_set->root;
	cur = NULL;
	while (*pos) {
		cur = *pos;
		comp_res = compare(link, cur);
		if (comp_res == 0) {
			tree_set_copy_links(cur, link);
			*pos = link;
			tree_set_link_init(cur);
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
	link->color = TREE_SET_RED;
	*pos = link;
	tree_set_insert_balance(tree_set, link);
	return 1;
}

void tree_set_delete_balance(struct tree_set* tree, struct tree_set_link *link) {
	struct tree_set_link *other, *par;
	assert(link != NULL);
	assert(tree != NULL);
	while (link != tree->root && link->color == TREE_SET_BLACK) {
		par = link->par;
		if (link == par->left) {
			/* assert(other != NULL); */
			other = par->right;
			if (other->color == TREE_SET_RED) {
				other->color = TREE_SET_BLACK;
				par->color = TREE_SET_RED;
				tree_set_left_rotate(tree, par);
				par = link->par;
				other = par->right;
			}
			if ((other->left == NULL || other->left->color == TREE_SET_BLACK)
			    	&& (other->right == NULL || other->right->color == TREE_SET_BLACK)) {
				other->color = TREE_SET_RED;
				link = par;
			} else {
				if (other->right == NULL || other->right->color == TREE_SET_BLACK) {
					other->left->color = TREE_SET_BLACK;
					other->color = TREE_SET_RED;
					tree_set_right_rotate(tree, other);
					other = par->right;
				}
				other->color = par->color;
				par->color = TREE_SET_BLACK;
				other->right->color = TREE_SET_BLACK;
				tree_set_left_rotate(tree, par);
				link = tree->root;
			}
		} else {
			/* assert(other != NULL); */
			other = par->left;
			if (other->color == TREE_SET_RED) {
				other->color = TREE_SET_BLACK;
				par->color = TREE_SET_RED;
				tree_set_right_rotate(tree, par);
				par = link->par;
				other = par->left;
			}
			if ((other->right == NULL || other->right->color == TREE_SET_BLACK)
			    	&& (other->left == NULL || other->left->color == TREE_SET_BLACK)) {
				other->color = TREE_SET_RED;
				link = par;
			} else {
				if (other->left == NULL || other->left->color == TREE_SET_BLACK) {
					other->right->color = TREE_SET_BLACK;
					other->color = TREE_SET_RED;
					tree_set_left_rotate(tree, other);
					other = par->left;
				}
				other->color = par->color;
				par->color = TREE_SET_BLACK;
				other->left->color = TREE_SET_BLACK;
				tree_set_right_rotate(tree, par);
				link = tree->root;
			}
		}
	}
	link->color = TREE_SET_BLACK;
}

/**
 * Replace found link with other one and remove from tree.
 * @param del_pos - Position, where we delete node from.
 * @param pnil - Pointer to nil of tree.
 * Returns postition to start balancing from (or NULL if wi don't need it).
 */
static inline struct tree_set_link *tree_set_replace_link(
		struct tree_set_link **del_pos, struct tree_set_link *pnil) {

	struct tree_set_link *deleted;

	/* Position, where node can be inserted to be deleted easily */
	struct tree_set_link **other_pos;

	/* Element, what possible take deleted one's position */
	struct tree_set_link *other;

	/* Start of balancing. */
	struct tree_set_link *res;

	deleted = *del_pos;

	if (deleted->left == NULL || deleted->right == NULL) {
		/* There is less then two children of deleted node */
		*del_pos = (deleted->left != NULL) ? deleted->left : deleted->right;
		if (*del_pos != NULL) {
			(*del_pos)->par = deleted->par;
		} else {
			pnil->color = TREE_SET_BLACK;
			pnil->par = deleted->par;
			*del_pos = pnil;
		}
		res = (deleted->color == TREE_SET_BLACK) ? (*del_pos) : NULL;
	} else {
		/* Give element from the right subtree */
		other_pos = &deleted->right;
		while ((*other_pos)->left != NULL) {
			other_pos = &(*other_pos)->left;
		}
		other = *other_pos;
		*other_pos = (other->right != NULL) ? other->right : pnil;
		if (other->right != NULL) {
			if (other->par != deleted) {
				other->right->par = other->par;
			}
		} else {
			pnil->color = TREE_SET_BLACK;
			if (other->par != deleted) {
				pnil->par = other->par;
			} else {
				pnil->par = other;
			}
		}
		res = (other->color == TREE_SET_BLACK) ? (*other_pos) : NULL;
		tree_set_copy_links(deleted, other);
		*del_pos = other;
	}
	tree_set_link_init(deleted);
	return res;
}

/** Delete node on selected position */
static int tree_set_remove_found_link(struct tree_set *tree,
				struct tree_set_link **del_pos) {

	/* Real representation of null referense. */
	struct tree_set_link nil;

	/* Where start balancing. */
	struct tree_set_link *start;

	nil.par = NULL;
	start = tree_set_replace_link(del_pos, &nil);
	if (start != NULL) {
		tree_set_delete_balance(tree, start);
	}
	if (nil.par != NULL) {
		if (nil.par->left == &nil) {
			nil.par->left = NULL;
		} else {
			nil.par->right = NULL;
		}
		nil.par = NULL;
	} else {
		if (tree->root == &nil) {
			tree->root = NULL;
		}
	}
	return 1;
}

int tree_set_remove_link(struct tree_set *tree_set,
		struct tree_set_link *link, tree_set_comparator_t compare) {
	struct tree_set_link **del_pos;
	int comp_res;
	assert(tree_set != NULL);
	assert(link != NULL);

	del_pos = &tree_set->root;
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
	if (*del_pos == NULL) return 0;
	return tree_set_remove_found_link(tree_set, del_pos);
}

/** Clear subtree, corresponding to the node. */
static void tree_set_clear_link(struct tree_set_link *link, tree_set_dispose_t dispose) {
	if (link != NULL) {
		tree_set_clear_link(link->left, dispose);
		tree_set_clear_link(link->right, dispose);
		dispose(link);
	}
}

void tree_set_clear(struct tree_set *set, tree_set_dispose_t dispose) {
	assert(set != NULL);
	tree_set_clear_link(set->root, dispose);
	set->root = NULL;
}

/* Operations on subtree, represented with their root nodes. */

struct tree_set_link *__tree_set_min_link(struct tree_set_link *root) {
	struct tree_set_link *result;
	if (root == NULL) {
		return NULL;
	}
	result = root;
	while (result->left != NULL) {
		result = result->left;
	}
	return result;
}

struct tree_set_link *tree_set_min_link(struct tree_set *tree_set) {
	assert(tree_set != NULL);
	return __tree_set_min_link(tree_set->root);
}

struct tree_set_link *__tree_set_max_link(struct tree_set_link *root) {
	struct tree_set_link *result;
	result = root;
	if (result == NULL) {
		return NULL;
	}
	while (result->right != NULL) {
		result = result->right;
	}
	return result;
}

struct tree_set_link *tree_set_max_link(struct tree_set *tree_set) {
	assert(tree_set != NULL);
	return __tree_set_max_link(tree_set->root);
}

struct tree_set_link *tree_set_next_link(struct tree_set_link *link) {
	struct tree_set_link *par;
	assert(link != NULL);
	if (link->right != NULL) {
		/* Search for the most left element in right subtree */
		return __tree_set_min_link(link->right);
	} else {
		/* Search for the first parent, placed righter than it's son. */
		par = link->par;
		while (par != NULL && par->right == link) {
			link = par;
			par = par->par;
		}
		return par;
	}
}

struct tree_set_link *tree_set_prev_link(struct tree_set_link *link) {
	struct tree_set_link *par;
	assert(link != NULL);
	if (link->left != NULL) {
		/* Search for the most left element in right subtree */
		return __tree_set_max_link(link->left);
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
