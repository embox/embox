/**
 * @file
 * @brief Working with tree_set example
 *
 * @date 12.11.2011
 * @author Dmitry Avdyukhin
 */


#include <util/tree_set.h>

#include <framework/example/self.h>
#include <stdio.h>
EMBOX_EXAMPLE(tree_set_example);

/*
 * Important parts for tree_set:
 *   1) Structure with some data and tree_set_link.
 *      The last will be used for storing struct instanse in set.
 *   2) Compare of tree_set_link function (is needed for tree construction).
 */

/** Structure, allowing to store in numbers in set. */
struct int_tree_set_element {
	int data;
	struct tree_set_link link;
};

/** Returns int value corresponding to specified link in set. */
static int get_tree_set_int_val(struct tree_set_link *link) {
	return tree_set_element(link, struct int_tree_set_element, link)->data;
}

/** Comparison of links is comparison of corresponding integer values. */
static int tree_set_int_compare(struct tree_set_link *first, struct tree_set_link *second) {
	return get_tree_set_int_val(first) - get_tree_set_int_val(second);
}

/*
 * Some way to allocate elements for set.
 */

/* Pool of elements. */
static struct int_tree_set_element elements[100];
static int elem_cnt = 0;

/* Returns pointer to newly allocated element from pool. */
static struct tree_set_link *new_link(int val) {
	elements[elem_cnt].data = val;
	return &elements[elem_cnt++].link;
}

/** Dispose function - nothing to do. */
static void dispose(struct tree_set_link *link) { /* Nothing */ }

/** Maps tree_set_link to integer value. */
static int link2int(struct tree_set_link *link) {
	return tree_set_element(link, struct int_tree_set_element, link)->data;
}

/** Example. How to use tree_set. */
static int tree_set_example(int argc, char **argv) {

	/* Set itself. */
	struct tree_set set_real;

	/* It's more comfortable to have a pointer to set. */
	struct tree_set *set = &set_real;

	/* Used in different foreach loops. */
	struct tree_set_link *link;

	/* Also used in different foreach loops. */
	struct int_tree_set_element *elem;

	/* Init working with set. */
	printf("Set is empty\n");
	tree_set_init(set);

	printf("Add to set numbers: 1341, 22141, 11, 325, -50\n");
	tree_set_add_link(set, new_link(1341), tree_set_int_compare);
	tree_set_add_link(set, new_link(22141), tree_set_int_compare);
	tree_set_add_link(set, new_link(11), tree_set_int_compare);
	tree_set_add_link(set, new_link(325), tree_set_int_compare);
	tree_set_add_link(set, new_link(-50), tree_set_int_compare);

	printf("Return if set is empty (must return false): ");
	printf("%d\n", tree_set_empty(set));

	/* Minimum and maximum elements in tree. */
	printf("Minimum in set (must be -50): %d\n",
			link2int(tree_set_min_link(set)));
	printf("Maximum in set (must be 22141): %d\n",
			link2int(tree_set_max_link(set)));

	/* Foreach loops. */

	/* Foreach on links (without casting to structure with data field). */
	printf("Print set in format (link address, corresponding value).\n)");
	printf("  Second elements of pairs must be in ascending order.\n");
	tree_set_foreach_link(link, set) {
		/* Print address of link. */
		printf("(%d ", (int)link);
		/* Casting out allows to access data field. */
		printf("%d) ", link2int(link));
	}
	printf("\n");

	/* Foreach in back order. */
	printf("Print set in format (link address, corresponding value).\n)");
	printf("  Second elements of pairs must be in decending order.\n");
	tree_set_foreach_back_link(link, set) {
		printf("(%d ", (int)link);
		printf("%d) ", link2int(link));
	}
	printf("\n");

	/* Foreach with casting out. */
	printf("Print set data in ascending order.\n");
	tree_set_foreach(elem, set, link) {
		printf ("%d ", elem->data);
	}
	printf("\n");

	printf("Print set data in descending order.\n");
	tree_set_foreach_back(elem, set, link) {
		printf ("%d ", elem->data);
	}
	printf("\n");

	/* Delete elements from set. */

	printf("Delete (existing) element -50 from set.\n");
	tree_set_remove_link(set, new_link(-50), tree_set_int_compare);

	printf("Now set consists of:\n");
	tree_set_foreach(elem, set, link) {
		printf ("%d ", elem->data);
	}
	printf("\n");

	printf("Delete (not-existing) element 10 from set.\n");
	tree_set_remove_link(set, new_link(10), tree_set_int_compare);
	printf("Nothing have changed:\n");
	tree_set_foreach(elem, set, link) {
		printf ("%d ", elem->data);
	}
	printf("\n");

	/* Delete all elements from set. */
	printf("Clear set\n");
	tree_set_clear(set, dispose);

	printf("Is it empty: %d\n", tree_set_empty(set));

	return 0;
}
