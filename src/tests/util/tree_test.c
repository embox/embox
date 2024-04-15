/**
 * @file
 * @brief Test unit for util/tree.
 *
 * @date Oct 8, 2011
 * @author Avdyukhin Dmitry
 */

#include <stdint.h>

#include <embox/test.h>

#include <lib/libds/tree.h>

EMBOX_TEST_SUITE("util/tree test");

struct int_tree {
	int data;
	struct tree_link link;
};

static void dispose(struct tree_link *link) { /* Nothing to do. */ }

/** Pool of nodes. */
static struct int_tree nodes[100];
static int node_cnt = 0;

/** Return pointer to link field of int_tree structure. */
static struct tree_link *get_link(struct int_tree *node) {
	return &node->link;
}

/** Allocate new node and initialize it. */
static struct int_tree *new_node(void) {
	tree_link_init(get_link(nodes + node_cnt));
	return nodes + node_cnt++;
}

/** The values in traversal must be equal to values in this array. */
static int ans[100];
static int ans_cnt;

/** Clear answer array. */
static void tree_clear_ans(void) {
	ans_cnt = 0;
}

static void tree_reset_all(void) {
	node_cnt = 0;
	tree_clear_ans();
}

/** Add element to tail of ans array. */
static void push(int val) {
	ans[ans_cnt++] = val;
}

/** Compare result of tree traversal with expected result. */
static void compare(struct tree_link *root) {
	struct tree_link *link;
	struct int_tree *elem;
	int i;
	i = 0;
#if 0
	printf ("tree: ");
	tree_postorder_traversal(link, elem, root, link) {
		printf("%d ", elem->data);
	}
	printf ("\n");
#endif
	tree_postorder_traversal_link(link, root) {}
	tree_postorder_traversal_link_safe(link, root) {}
	tree_postorder_traversal(elem, root, link) {
		/* printf("%d %d %d\n", i, elem->data, ans[i]); */
		test_assert(elem->data == ans[i]);
		i++;
	}
	/* printf("\n"); */
	test_assert(i == ans_cnt);
}

/** Returns the leftmost child of specified node. */
static struct tree_link *get_left_child(struct tree_link *link) {
	return dlist_entry(dlist_first(&link->children), struct tree_link, list_link);
}

/**
 * Build test tree, to what all operations will be applied.
 * Tree structure will be:
 *
 *      10
 *     /  \
 *    20  21
 *   /  \
 *  30  31
 *
 */
static struct tree_link *tree_build_base(void) {
	struct int_tree *n[5];
	struct tree_link *root;
	int i;
	for (i = 0; i < 5; i++) {
		n[i] = new_node();
	}
	n[0]->data = 10;
	n[1]->data = 20;
	n[2]->data = 21;
	n[3]->data = 30;
	n[4]->data = 31;
	root = get_link(n[0]);
	tree_add_link(root, get_link(n[1]));
	tree_add_link(root, get_link(n[2]));
	tree_add_link(get_link(n[1]), get_link(n[3]));
	tree_add_link(get_link(n[1]), get_link(n[4]));
	return root;
}

TEST_CASE("One node tree test") {
	struct int_tree *root_node;
	struct tree_link *root;
	tree_reset_all();
	root_node = new_node();
	root = get_link(root_node);
	root_node->data = 10;
	push(10);
	compare(root);
}

TEST_CASE("Foreach on full tree") {
	struct int_tree *child1, *child2, *root_node;
	struct tree_link *root;
	tree_reset_all();
	root_node = new_node();
	child1 = new_node();
	child2 = new_node();
	root_node->data = 10;
	root = get_link(root_node);
	child1->data = 20;
	child2->data = 21;
	tree_add_link(root, get_link(child1));
	tree_add_link(root, get_link(child2));
	push(20);
	push(21);
	push(10);
	compare(root);
}

TEST_CASE("Foreach on subtree") {
	struct tree_link *root = tree_build_base();
	tree_clear_ans();
	push(30);
	push(31);
	push(20);
	compare(get_left_child(root));
}

/** Return data field of node, corresponding to given link. */
static int link2int(struct tree_link *link) {
	assert(link);
	return tree_element(link, struct int_tree, link)->data;
}

static int link2int_is(struct tree_link *link, void *x) {
	return (link2int(link) == (intptr_t) x);
}

TEST_CASE("Successful search among children") {
	struct tree_link *root = tree_build_base();
	test_assert(tree_lookup_child(root, link2int_is, (void *) 20) != NULL);
}

TEST_CASE("Unsuccessful search among children") {
	struct tree_link *root = tree_build_base();
	test_assert(tree_lookup_child(root, link2int_is, (void *) 30) == NULL);
}

TEST_CASE("Successful search in subtree") {
	struct tree_link *root = tree_build_base();
	test_assert(tree_lookup(root, link2int_is, (void *) 30) != NULL);
}

TEST_CASE("Unsuccessful search in subtree") {
	struct tree_link *root = tree_build_base();
	test_assert(tree_lookup(root, link2int_is, (void *) 100) == NULL);
}

TEST_CASE("Move nodes") {
	struct tree_link *root, *ch1, *ch2;
	tree_reset_all();
	root = tree_build_base();
	ch1 = get_left_child(get_left_child(root)); /* 30 */
	ch2 = tree_postorder_next(ch1); /* 31 */
	tree_move_link(ch1, ch2);

	tree_clear_ans();
	push(link2int(ch2)); /* 31 */
	push(link2int(ch1)); /* 30 */
	push(20);
	push(21);
	push(10);
	compare(root);
}

TEST_CASE("Unlink subtree") {
	struct tree_link *node, *root;
	tree_reset_all();
	root = tree_build_base();
	node = get_left_child(root);
	tree_unlink_link(node);

	tree_clear_ans();
	push(30);
	push(31);
	push(20);
	compare(node);

	tree_clear_ans();
	push(21);
	push(10);
	compare(root);

	tree_add_link(root, node);

	tree_clear_ans();
	push(21);
	push(30);
	push(31);
	push(20);
	push(10);

	compare(root);
}

TEST_CASE("Delete subtree") {
	struct tree_link *root, *node;
	root = tree_build_base();
	node = get_left_child(root);
	tree_delete_link(node, dispose);

	tree_clear_ans();
	push(link2int(node));
	compare(node);

	tree_clear_ans();
	push(21);
	push(10);
	compare(root);
}

TEST_CASE("After adding tree element must be in children") {
	struct tree_link root, child, *item;
	struct tree_link *pattern[] = {&child};
	int pattern_pos = 0;

	tree_link_init(&root);
	tree_link_init(&child);
	tree_add_link(&root, &child);

	tree_foreach_children_link(item, &root) {
		test_assert_equal(pattern[pattern_pos++], item);
	}

	test_assert_equal(pattern_pos, sizeof(pattern) / sizeof(pattern[0]));
}
