/**
 * @file
 * @brief Test unit for util/tree_set.
 *
 * @date Oct 8, 2011
 * @author Avdyukhin Dmitry
 */

#include <embox/test.h>
#include <stdio.h>
#include <string.h>
#include <util/tree_set.h>
#include <stdlib.h>

EMBOX_TEST_SUITE("util/tree_set test");

static struct tree_set simple_tree_set;
static struct tree_set_link tree_set_links[3];
static int tree_set_address_comp(struct tree_set_link* first, struct tree_set_link* second) {
	return first - second;
}

TEST_CASE("Compilation test for tree_set") {
	struct tree_set_link *link;
	tree_set_init(&simple_tree_set);
	tree_set_min_link(&simple_tree_set);
	tree_set_add_link(&simple_tree_set, tree_set_links + 1, tree_set_address_comp);
	tree_set_add_link(&simple_tree_set, tree_set_links + 0, tree_set_address_comp);
	tree_set_add_link(&simple_tree_set, tree_set_links + 2, tree_set_address_comp);
	tree_set_foreach_link(link, &simple_tree_set) { }
}

struct int_tree_set_element {
	int data;
	struct tree_set_link link;
};

static int get_tree_set_int_val(struct tree_set_link *link) {
	assert(link);
	return tree_set_element(link, struct int_tree_set_element, link)->data;
}

static int tree_set_int_comp(struct tree_set_link *first, struct tree_set_link *second) {
	return get_tree_set_int_val(first) - get_tree_set_int_val(second);
}

#define ELEMENT_COUNT 100

/* Pool of elements. */
static struct int_tree_set_element elements[ELEMENT_COUNT << 1];
static int elem_cnt;

static struct tree_set set_obj;
/* Set itself. */
static struct tree_set *set = &set_obj;

/* Array, used to store the same elements than in the set in ascending order. */
static struct int_tree_set_element *ideal[ELEMENT_COUNT];
/* Count of elements in array. Must be equal to number of set elements. */
static int ideal_size;

/* Add element into array (ascending order must be saved). */
static void array_add(struct int_tree_set_element *elem) {
	int i, j, comp_res;
	for (i = 0; i < ideal_size; i++) {
		comp_res = elem->data - ideal[i]->data;
		if (comp_res == 0) {
			ideal[i] = elem;
			return;
		} else if (comp_res < 0) {
			break;
		}
	}
	for (j = ideal_size; j > i; j--) {
		ideal[j] = ideal[j-1];
	}
	ideal[i] = elem;
	ideal_size++;
}

/* Delete element from array with saving the order. */
static void array_del(struct int_tree_set_element * elem) {
	int i, j, comp_res;
	for (i = 0; i < ideal_size; i++) {
		comp_res = elem->data - ideal[i]->data;
		if (comp_res == 0) {
			break;
		} else if (comp_res < 0) {
			return;
		}
	}
	if (i == ideal_size) {
		return;
	}
	ideal_size--;
	for (j = i; j < ideal_size; j++) {
		ideal[j] = ideal[j+1];
	}
	ideal[ideal_size] = NULL;
}

/*
 * Check whether set and array contains the same elements.
 * Also check that both foreach loops (forward and back) are correct.
 */
static bool compare(void) {
	struct int_tree_set_element *elem;
	int i;

	/* Forward */
	i = 0;
	tree_set_foreach(elem, set, link) {
		if (elem->data != ideal[i]->data) {
			return false;
		}
		i++;
	}
	if (i != ideal_size) {
		return false;
	}

	/* Back */
	i = ideal_size-1;
	tree_set_foreach_back(elem, set, link) {
		if (elem->data != ideal[i]->data) {
			return false;
		}
		i--;
	}
	if (i != -1) {
		return false;
	}
	return true;
}

/* Return color of node (black if node is NULL). */
static enum tree_set_color get_color(struct tree_set_link *node) {
	return (node != NULL) ? node->color : TREE_SET_BLACK;
}

/* Check for subtree if children of red node are black. */
static bool check_rb_node(struct tree_set_link *node) {
	if (node == NULL) {
		return true;
	}
	if (node->color == TREE_SET_RED) {
		if (get_color(node->left) != TREE_SET_BLACK
				|| get_color(node->right) != TREE_SET_BLACK) {
			return false;
		}
	}
	return (check_rb_node(node->left) && check_rb_node(node->right));
}

/* Check for each red node in tree that all of it's children are black. */
static bool check_rb(struct tree_set *set) {
	return check_rb_node(set->root);
}

#if 0
static void print(void){
	int i;
	struct tree_set_link *link;
	struct int_tree_set_element *elem;
	printf ("\n");
	for (i = 0; i < ideal_size; i++) {
		printf ("%d ", ideal[i]->data);
	}
	printf ("\n");
	tree_set_foreach(link, elem, set, link) {
		printf ("%d ", elem->data);
	}
	printf ("\n");
}
#endif

#if 0
static void print_tree_set_link(struct tree_set_link *link, int shift) {
	int i;
	if (link != NULL) {
		print_tree_set_link(link->right, shift + 4);
		for (i = 0; i < shift; i++) {
			printf(" ");
		}
		printf ("%c", link->color == TREE_SET_RED ? 'R' : 'B');
		printf (", %d\n", get_tree_set_int_val(link));
		print_tree_set_link(link->left, shift + 4);
	}
}

/* Readable output of tree. */
static void print_tree_set(void) {
	printf("\n");
	print_tree_set_link(set->root, 0);
	printf("\n");
}
#endif

static void tree_set_reset_all(void) {
	elem_cnt = 0;
	tree_set_init(set);
	ideal_size = 0;
}

static struct int_tree_set_element *new_element(void) {
	return elements + elem_cnt++;
}

static void add(int num) {
	struct int_tree_set_element *elem = new_element();
	/* printf ("%d\n", num); */
	elem->data = num;
	tree_set_add_link(set, &elem->link, tree_set_int_comp);
	array_add(elem);
	/* print(); */
	test_assert(compare());
	test_assert(check_rb(set));
}

static void del(int num) {
	struct int_tree_set_element *elem = new_element();
	/* printf ("%d\n", num); */
	elem->data = num;
	tree_set_remove_link(set, &elem->link, tree_set_int_comp);
	array_del(elem);
	/* print(); */
	test_assert(compare());
	test_assert(check_rb(set));
}

TEST_CASE("Deletion from empty tree_set") {
	tree_set_reset_all();
	del(100);
}

TEST_CASE("Deletion of tree_set root") {
	add(100);
	del(100);
}

TEST_CASE("Add nodes without balancing") {
	add(2);
	add(1);
	add(3);
}

TEST_CASE("Add to red node (just recoloring)") {
	add(4);
}

#if 0
TEST_CASE("Add to red node (with one left rotate)") {
	add(5);
}

TEST_CASE("Add to red node (with one right rotate)") {
	add(0);
	add(-1);
}

TEST_CASE("Add with two left rotations") {
	tree_set_reset_all();
	add(0);
	add(1);
	add(2);
	add(3);
	add(4);
	add(5);
	add(6);
	add(7);
	/* print_tree_set(); */
}

TEST_CASE("Add with two right rotations") {
	tree_set_reset_all();
	add(7);
	add(6);
	add(5);
	add(4);
	add(3);
	add(2);
	add(1);
	add(0);
	/* print_tree_set(); */
}

static struct tree_set_link *get_link(int ind) {
	return &elements[ind].link;
}

TEST_CASE("Add test for all cases (left)") {
	int i;
	tree_set_reset_all();
	for (i = 0; i < 9; i++) {
		tree_set_link_init(&elements[i].link);
		elements[i].data = i;
		if (i != 2) {
			ideal[ideal_size++] = &elements[i];
		}
	}
	elem_cnt = 9;
	set->root = get_link(6);

	get_link(6)->left = get_link(1);
	get_link(6)->right = get_link(7);
	get_link(6)->color = TREE_SET_BLACK;

	get_link(1)->left = get_link(0);
	get_link(1)->right = get_link(4);
	get_link(1)->par = get_link(6);
	get_link(1)->color = TREE_SET_RED;

	get_link(0)->par = get_link(1);
	get_link(0)->color = TREE_SET_BLACK;

	get_link(4)->left = get_link(3);
	get_link(4)->right = get_link(5);
	get_link(4)->par = get_link(1);
	get_link(4)->color = TREE_SET_BLACK;

	get_link(3)->par = get_link(4);
	get_link(3)->color = TREE_SET_RED;

	get_link(5)->par = get_link(4);
	get_link(5)->color = TREE_SET_RED;

	get_link(7)->right = get_link(8);
	get_link(7)->par = get_link(6);
	get_link(7)->color = TREE_SET_BLACK;

	get_link(8)->par = get_link(7);
	get_link(8)->color = TREE_SET_RED;

	/* print_tree_set(); */
	add(2);
	/* print_tree_set(); */
}

TEST_CASE("Add test for all cases (right)") {
	int i;
	tree_set_reset_all();
	for (i = 0; i < 9; i++) {
		tree_set_link_init(&elements[i].link);
		elements[i].data = i;
		if (i != 6) {
			ideal[ideal_size++] = &elements[i];
		}
	}
	elem_cnt = 9;
	set->root = get_link(2);

	get_link(2)->right = get_link(7);
	get_link(2)->left = get_link(1);
	get_link(2)->color = TREE_SET_BLACK;

	get_link(7)->right = get_link(8);
	get_link(7)->left = get_link(4);
	get_link(7)->par = get_link(2);
	get_link(7)->color = TREE_SET_RED;

	get_link(8)->par = get_link(7);
	get_link(8)->color = TREE_SET_BLACK;

	get_link(4)->right = get_link(5);
	get_link(4)->left = get_link(3);
	get_link(4)->par = get_link(7);
	get_link(4)->color = TREE_SET_BLACK;

	get_link(5)->par = get_link(4);
	get_link(5)->color = TREE_SET_RED;

	get_link(3)->par = get_link(4);
	get_link(3)->color = TREE_SET_RED;

	get_link(1)->left = get_link(0);
	get_link(1)->par = get_link(2);
	get_link(1)->color = TREE_SET_BLACK;

	get_link(0)->par = get_link(1);
	get_link(0)->color = TREE_SET_RED;

	/* print_tree_set(); */
	add(6);
	/* print_tree_set(); */
}

/* Count of add operations on set. */
static int add_cnt = ELEMENT_COUNT >> 1;

TEST_CASE("Add test for tree_set") {
	int i, num;
	/* printf("Add... "); */
	for (i = 0; i < add_cnt; i++) {
		/* printf("%d ", i); */
		num = rand() % 100;
		add(num);
	}
}

/* Count of del operations on set. */
static int del_cnt = ELEMENT_COUNT >> 1;

TEST_CASE("Delete test for tree_set") {
	int i, num;
	/* printf("Del..."); */
	for (i = 0; i < del_cnt; i++) {
	/* printf("%d ", i); */
		num = rand() % 100;
		del(num);
	}
}

static void tree_set_int_dispose(struct tree_set_link *link) { /* Nothing */ }

TEST_CASE("Test for clear method of test_set") {
	ideal_size = 0;
	tree_set_clear(set, tree_set_int_dispose);
	test_assert(compare());
}
#endif
