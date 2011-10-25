/**
 * @file
 * @brief Test unit for util/tree_set.
 *
 * @date Oct 8, 2001
 * @author Avdyukhin Dmitry
 */

#include <embox/test.h>
#include <stdio.h>
#include <string.h>
#include <util/tree_set.h>
#include <util/rand.h>

EMBOX_TEST_SUITE("util/tree_set test");

static struct tree_set simple_tree_set;
static struct tree_set_link tree_set_links[3];
static int tree_set_address_comp(struct tree_set_link* first, struct tree_set_link* second) {
	return first - second;
}

TEST_CASE("Any test for tree_set") {
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

static int tree_set_int_comp(struct tree_set_link *first, struct tree_set_link *second) {
	return tree_set_element(first, struct int_tree_set_element, link)->data
		- tree_set_element(second, struct int_tree_set_element, link)->data;
}

#define ELEMENT_COUNT 100

static struct int_tree_set_element elements[ELEMENT_COUNT];
static int elem_cnt;
static int add_cnt = ELEMENT_COUNT >> 1, del_cnt = ELEMENT_COUNT >> 1;

static struct tree_set tree_set;

static struct int_tree_set_element *ideal[ELEMENT_COUNT];
static int ideal_size;

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

static bool compare(void) {
	struct tree_set_link *link;
	struct int_tree_set_element *elem;
	int i = 0;
	tree_set_foreach(link, elem, &tree_set, link) {
		if (elem->data != ideal[i]->data) {
			return false;
		}
		i++;
	}
	return i == ideal_size;
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
	tree_set_foreach(link, elem, &tree_set, link) {
		printf ("%d ", elem->data);
	}
	printf ("\n");
}
#endif
static void add(int num) {
	struct int_tree_set_element *elem = elements + elem_cnt++;
	/* printf ("%d\n", num); */
	elem->data = num;
	tree_set_add_link(&tree_set, &elem->link, tree_set_int_comp);
	array_add(elem);
	/* print(); */
	test_assert(compare());
	test_assert(check_rb(&tree_set));
}

static void del(int num) {
	struct int_tree_set_element *elem = elements + elem_cnt++;
	/* printf ("%d\n", num); */
	elem->data = num;
	tree_set_remove_link(&tree_set, &elem->link, tree_set_int_comp);
	array_del(elem);
	/* print(); */
	test_assert(compare());
	test_assert(check_rb(&tree_set));
}

TEST_CASE("Add test for tree_set") {
	int i, num;
	tree_set_init(&tree_set);
	ideal_size = 0;
	elem_cnt = 0;
	/* printf("Add... "); */
	for (i = 0; i < add_cnt; i++) {
		/* printf("%d ", i); */
		num = rand() % 100;
		add(num);
	}
}

TEST_CASE("Delete test for tree_set") {
	int i, num;
	/* printf("Del..."); */
	for (i = 0; i < del_cnt; i++) {
	/* printf("%d ", i); */
		num = rand() % 100;
		del(num);
	}
}

