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

/** Left rotation of tree node. */
static void tree_set_left_rotate(struct tree_set *tree, struct tree_set_link *link) {
	struct tree_set_link *par;
	struct tree_set_link *other = link->right;
	assert(link != NULL);
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
	struct tree_set_link *other = link->left;
	assert(link != NULL);
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
				}
				par->color = TREE_SET_BLACK;
				par2->color = TREE_SET_RED;
				tree_set_right_rotate(tree, par2);
			}
		} else {
			other = par2->left;
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
				}
				par->color = TREE_SET_BLACK;
				par2->color = TREE_SET_RED;
				tree_set_left_rotate(tree, par2);
			}
		}
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
			link->left = cur->left;
			link->right = cur->right;
			link->par = cur->par;
			if (link->right != NULL) {
				link->right->par = link;
			}
			if (link->left != NULL) {
				link->left->par = link;
			}
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

/** Delete node on selected position */
static int tree_set_remove_found_link(struct tree_set *tree,
				struct tree_set_link **del_pos) {

	struct tree_set_link *deleted;

	/* position, where node can be inserted to be deleted easily */
	struct tree_set_link **other_pos;

	/* element, what possible take deleted one's position */
	struct tree_set_link *other;

	/* Real representation of null referense. */
	struct tree_set_link nil;

	/* Temp color for detecting if we need balancing. */
	enum tree_set_color temp_color;

	/* Where start balancing. */
	struct tree_set_link *start;

	deleted = *del_pos;
	if (deleted == NULL) {
		return 0;
	}
	start = NULL;
	nil.par = NULL;

	if (deleted->left == NULL || deleted->right == NULL) {
		/* There is less then two children of deleted node */
		*del_pos = (deleted->left != NULL) ? deleted->left : deleted->right;
		temp_color = deleted->color;
		if (*del_pos != NULL) {
			(*del_pos)->par = deleted->par;
		} else {
			nil.color = TREE_SET_BLACK;
			nil.par = deleted->par;
			*del_pos = &nil;
		}
		start = *del_pos;
	} else {
		/* Give element from the right subtree */
		other_pos = &deleted->right;
		while ((*other_pos)->left != NULL) {
			other_pos = &(*other_pos)->left;
		}
		other = *other_pos;
		*other_pos = (other->right != NULL) ? other->right : &nil;
		if (other->right != NULL) {
			if (other->par != deleted) {
				other->right->par = other->par;
			}
		} else {
			nil.color = TREE_SET_BLACK;
			if (other->par != deleted) {
				nil.par = other->par;
			} else {
				nil.par = other;
			}
		}
		start = *other_pos;
		other->left = deleted->left;
		if (other->left != NULL) {
			other->left->par = other;
		}
		other->right = deleted->right;
		if (other->right != NULL) {
			other->right->par = other;
		}
		other->par = deleted->par;
		temp_color = other->color;
		other->color = deleted->color;
		*del_pos = other;
	}
	if (temp_color == TREE_SET_BLACK) {
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
	tree_set_link_init(deleted);
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

/* Operations on subtree, represented with their root nodes. */

struct tree_set_link *__tree_set_min_link(struct tree_set_link *root) {
	struct tree_set_link *result;
	result = root;
	if (result == NULL) {
		return NULL;
	}
	while (result->left) {
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
	while (result->right) {
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
		/* Search for the first parent, placed righter than its son. */
		par = link->par;
		while (par && par->right == link) {
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

