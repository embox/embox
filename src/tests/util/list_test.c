/**
 * @file
 * @brief Tests methods of util/list.
 *
 * @date 12.03.11
 * @author Eldar Abusalimov
 * @author Ilia Vaprol
 */

#include <embox/test.h>

#include <string.h>
#include <util/array.h>

#include <util/list.h>

EMBOX_TEST_SUITE("util/list test");

TEST_SETUP(setup);

struct element {
	int some_stuff;
	struct list_link lnk;
};

typedef member_t(struct element, lnk) element_in_list;

static struct element x, y, z;
static struct list m, n;

static struct element * const xyz[] = { &x, &y, &z, NULL };
static struct element * const xy[] = { &x, &y, NULL };
static struct element * const yz[] = { &y, &z, NULL };
static struct element * const xz[] = { &x, &z, NULL };

/**
 * Fills a @a list with the contents of an @a array.
 *
 * @param array
 *   @a NULL terminated array.
 * @param list
 *   The list to fill in.
 * @return
 *   The given @a list.
 */
static struct list *fill_in_from(struct element * const array[],
		struct list *list);

/**
 * Compares a @a list with the pattern @a array. The list becomes empty after
 * the comparison.
 *
 * @param array
 *   @a NULL terminated array with the expected pattern.
 * @param list
 *   The list to check.
 * @return
 *   The given @a list.
 */
static struct list *compare_with(struct element * const array[],
		struct list *list);

TEST_CASE("list_element should cast link member out to its container") {
	struct list_link *link = &x.lnk;
	test_assert_equal(list_element(link, struct element, lnk), &x);
}

TEST_CASE("list_init should return its argument") {
	struct list l;
	test_assert_equal(list_init(&l), &l);
}

TEST_CASE("list_link_init should return its argument") {
	struct element e;
	test_assert_equal(list_link_init(&e.lnk), &e.lnk);
}

TEST_CASE("list_init should have the same effect as static initializer") {
	struct list l = LIST_INIT(&l);
	char buff[sizeof(l)];
	memcpy(buff, &l, sizeof(l));
	memset(&l, 0xA5, sizeof(l)); /* poison. */

	test_assert_mem_equal(buff, list_init(&l), sizeof(l));
}

TEST_CASE("list_link_init should have the same effect as static initializer") {
	struct element e = { .lnk = LIST_LINK_INIT(&e.lnk), .some_stuff = 42, };
	char buff[sizeof(e.lnk)];
	memcpy(buff, &e.lnk, sizeof(e.lnk));
	memset(&e.lnk, 0xA5, sizeof(e.lnk)); /* poison. */

	test_assert_mem_equal(buff, list_link_init(&e.lnk), sizeof(e.lnk));
}

TEST_CASE("list_is_empty should return true for just created list") {
	test_assert_true(list_is_empty(&m));
}

TEST_CASE("list_alone_link should return true for just initialized link") {
	test_assert_true(list_alone_link(&x.lnk));
}

TEST_CASE("list_alone should return true for just initialized element") {
	test_assert_true(list_alone(element_in_list, &x));
}

TEST_CASE("list_first should return null for empty list") {
	test_assert_null(list_first(element_in_list, &m));
}

TEST_CASE("list_last should return null for empty list") {
	test_assert_null(list_last(element_in_list, &m));
}

TEST_CASE("list_first_link should return null for empty list") {
	test_assert_null(list_first_link(&m));
}

TEST_CASE("list_last_link should return null for empty list") {
	test_assert_null(list_last_link(&m));
}

TEST_CASE("list_add_first should make the list non empty "
		"and the element not alone") {
	list_add_first(element_in_list, &x, &m);

	test_assert_false(list_is_empty(&m));
	test_assert_false(list_alone(element_in_list, &x));
}

TEST_CASE("list_add_last should make the list non empty "
		"and the element not alone") {
	list_add_last(element_in_list, &x, &m);

	test_assert_false(list_is_empty(&m));
	test_assert_false(list_alone(element_in_list, &x));
}

TEST_CASE("list_add_first_link should make the list non empty "
		"and the element's link not alone") {
	list_add_first_link(&x.lnk, &m);

	test_assert_false(list_is_empty(&m));
	test_assert_false(list_alone_link(&x.lnk));
}

TEST_CASE("list_add_last_link should make the list non empty "
		"and the element's link not alone") {
	list_add_last_link(&x.lnk, &m);

	test_assert_false(list_is_empty(&m));
	test_assert_false(list_alone_link(&x.lnk));
}

TEST_CASE("list_first_link and list_last_link on a single element list "
		"constructed using list_add_first should return the element's link") {
	list_add_first(element_in_list, &x, &m);

	test_assert_equal(list_first_link(&m), &x.lnk);
	test_assert_equal(list_last_link(&m), &x.lnk);
}

TEST_CASE("list_first_link and list_last_link on a single element list "
		"constructed using list_add_last should return the element's link") {
	list_add_last(element_in_list, &x, &m);

	test_assert_equal(list_first_link(&m), &x.lnk);
	test_assert_equal(list_last_link(&m), &x.lnk);
}

TEST_CASE("list_first_link and list_last_link should return a new and an old "
		"element accordingly after adding a new one with list_add_first") {
	test_assert_not_equal(&x, &y);

	list_add_first(element_in_list, &x, &m);
	list_add_first(element_in_list, &y, &m);

	test_assert_equal(list_first_link(&m), &y.lnk);
	test_assert_equal(list_last_link(&m), &x.lnk);
}

TEST_CASE("list_first_link and list_last_link should return a new and an old "
		"element accordingly after adding a new one with list_add_last") {
	test_assert_not_equal(&x, &y);

	list_add_last(element_in_list, &x, &m);
	list_add_last(element_in_list, &y, &m);

	test_assert_equal(list_first_link(&m), &x.lnk);
	test_assert_equal(list_last_link(&m), &y.lnk);
}

TEST_CASE("list_unlink on a single element list should make the list empty "
		"and element alone again") {
	list_add_first(element_in_list, &x, &m);

	list_unlink(element_in_list, &x);

	test_assert_true(list_is_empty(&m));
	test_assert_true(list_alone(element_in_list, &x));
}

TEST_CASE("single list_unlink and subsequent list_add_first to another list "
		"should make the first list empty but an element not alone") {
	test_assert_not_equal(&m, &n);

	list_add_first(element_in_list, &x, &m);

	list_unlink(element_in_list, &x);
	list_add_first(element_in_list, &x, &n);

	test_assert_true(list_is_empty(&m));
	test_assert_false(list_is_empty(&n));
	test_assert_false(list_alone(element_in_list, &x));
}

TEST_CASE("multiple list_unlink and subsequent list_add_first to another list "
		"should make the first list empty but elements not alone") {
	fill_in_from(xyz, &m);

	list_unlink(element_in_list, &x);
	list_unlink(element_in_list, &z);
	list_unlink(element_in_list, &y);

	fill_in_from(xyz, &n);

	test_assert_true(list_is_empty(&m));
	test_assert_false(list_is_empty(&n));
	test_assert_false(list_alone(element_in_list, &x));
	test_assert_false(list_alone(element_in_list, &y));
	test_assert_false(list_alone(element_in_list, &z));

	test_assert_equal(list_first_link(&n), &x.lnk);
	test_assert_equal(list_last_link(&n), &z.lnk);
}

TEST_CASE("list_remove_first should return null for empty list") {
	test_assert_null(list_remove_first(element_in_list, &m));
}

TEST_CASE("list_remove_last should return null for empty list") {
	test_assert_null(list_remove_last(element_in_list, &m));
}

TEST_CASE("list_remove_first on a single element list should return the "
		"element and make the list empty and element alone again") {
	list_add_first(element_in_list, &x, &m);

	test_assert_equal(list_remove_first(element_in_list, &m), &x);

	test_assert_true(list_is_empty(&m));
	test_assert_true(list_alone(element_in_list, &x));
}

TEST_CASE("list_remove_last on a single element list should return the "
		"element and make the list empty and element alone again") {
	list_add_first(element_in_list, &x, &m);

	test_assert_equal(list_remove_last(element_in_list, &m), &x);

	test_assert_true(list_is_empty(&m));
	test_assert_true(list_alone(element_in_list, &x));
}

TEST_CASE("list_insert_before on a single element list should make "
		"a new element the first one in the list") {
	list_add_first(element_in_list, &x, &m);

	list_insert_before(element_in_list, &y, &x);

	test_assert_equal(list_last_link(&m), &x.lnk);
	test_assert_equal(list_first_link(&m), &y.lnk);
}

TEST_CASE("list_insert_after on a single element list should make "
		"a new element the last one in the list") {
	list_add_first(element_in_list, &x, &m);

	list_insert_after(element_in_list, &y, &x);

	test_assert_equal(list_first_link(&m), &x.lnk);
	test_assert_equal(list_last_link(&m), &y.lnk);
}

TEST_CASE("list_insert_before: inserting a new element before the last one "
		"in a list of two elements should insert a new one between them") {
	list_add_first(element_in_list, &x, &m);
	list_add_last(element_in_list, &z, &m);

	list_insert_before(element_in_list, &y, &z);

	test_assert_equal(list_remove_first_link(&m), &x.lnk);
	test_assert_equal(list_remove_last_link(&m), &z.lnk);

	test_assert_equal(list_first_link(&m), &y.lnk);
	test_assert_equal(list_last_link(&m), &y.lnk);
}

TEST_CASE("list_insert_after: inserting a new element after the first one "
		"in a list of two elements should insert a new one between them") {
	list_add_first(element_in_list, &x, &m);
	list_add_last(element_in_list, &z, &m);

	list_insert_after(element_in_list, &y, &x);

	test_assert_equal(list_remove_first_link(&m), &x.lnk);
	test_assert_equal(list_remove_last_link(&m), &z.lnk);

	test_assert_equal(list_first_link(&m), &y.lnk);
	test_assert_equal(list_last_link(&m), &y.lnk);
}

TEST_CASE("list_insert_before and list_insert_after on a single element list"
		"should make new elements the first and the last accordingly") {
	list_add_first(element_in_list, &y, &m);

	list_insert_before(element_in_list, &x, &y);
	list_insert_after(element_in_list, &z, &y);

	test_assert_equal(list_first_link(&m), &x.lnk);
	test_assert_equal(list_last_link(&m), &z.lnk);
}

TEST_CASE("list_front should return null for empty list") {
	test_assert_null(list_front(element_in_list, &m));
}

TEST_CASE("list_back should return null for empty list") {
	test_assert_null(list_back(element_in_list, &m));
}

TEST_CASE("list_dequeue should return null for empty list") {
	test_assert_null(list_dequeue(element_in_list, &m));
}

TEST_CASE("list_front and list_back on a single element list "
		"constructed using list_enqueue should return the element's link") {
	list_enqueue(element_in_list, &x, &m);

	test_assert_equal(list_front(element_in_list, &m), &x);
	test_assert_equal(list_back(element_in_list, &m), &x);

	test_assert_equal(list_dequeue(element_in_list, &m), &x);
}

TEST_CASE("list_front and list_back should return a new and an old "
		"element accordingly after adding a new one with list_enqueue") {
	test_assert_not_equal(&x, &y);

	list_enqueue(element_in_list, &x, &m);
	list_enqueue(element_in_list, &y, &m);

	test_assert_equal(list_front(element_in_list, &m), &x);
	test_assert_equal(list_back(element_in_list, &m), &y);

	test_assert_equal(list_dequeue(element_in_list, &m), &x);
	test_assert_equal(list_front(element_in_list, &m), &y);
	test_assert_equal(list_back(element_in_list, &m), &y);

	test_assert_equal(list_dequeue(element_in_list, &m), &y);
}

TEST_CASE("list_top should return null for empty list") {
	test_assert_null(list_top(element_in_list, &m));
}

TEST_CASE("list_pop should return null for empty list") {
	test_assert_null(list_pop(element_in_list, &m));
}

TEST_CASE("list_top on a single element list "
		"constructed using list_push should return the element's link") {
	list_push(element_in_list, &x, &m);

	test_assert_equal(list_top(element_in_list, &m), &x);
	test_assert_equal(list_pop(element_in_list, &m), &x);
}

TEST_CASE("list_top should return a new "
		"element accordingly after adding a new one with list_push") {
	test_assert_not_equal(&x, &y);

	list_push(element_in_list, &x, &m);
	list_push(element_in_list, &y, &m);

	test_assert_equal(list_top(element_in_list, &m), &y);
	test_assert_equal(list_pop(element_in_list, &m), &y);

	test_assert_equal(list_top(element_in_list, &m), &x);
	test_assert_equal(list_pop(element_in_list, &m), &x);
}

TEST_CASE("list_next should return null for last element in list") {
	list_add_last(element_in_list, &x, &m);
	test_assert_null(list_next(element_in_list, &x, &m));
}

TEST_CASE("list_next should return next element for not empty list") {
	list_add_last(element_in_list, &x, &m);
	list_add_last(element_in_list, &y, &m);
	test_assert_equal(&y, list_next(element_in_list, &x, &m));
	test_assert_null(list_next(element_in_list, &y, &m));
}

TEST_CASE("list_prev should return null for first element in list") {
	list_add_last(element_in_list, &x, &m);
	test_assert_null(list_prev(element_in_list, &x, &m));
}

TEST_CASE("list_next should return prev element for not empty list") {
	list_add_last(element_in_list, &x, &m);
	list_add_last(element_in_list, &y, &m);
	test_assert_equal(&x, list_prev(element_in_list, &y, &m));
	test_assert_null(list_prev(element_in_list, &x, &m));
}

TEST_CASE("list_bulk_add_first shouldn't modify a destination list "
		"if a source list is empty") {
	fill_in_from(xyz, &n);

	list_bulk_add_first(&m, &n);

	test_assert_true(list_is_empty(&m));
	compare_with(xyz, &n);
}

TEST_CASE("list_bulk_add_last shouldn't modify a destination list "
		"if a source list is empty") {
	fill_in_from(xyz, &n);

	list_bulk_add_last(&m, &n);

	test_assert_true(list_is_empty(&m));
	compare_with(xyz, &n);
}

TEST_CASE("list_bulk_add_first should move all the elements from a source "
		"list to the beginning of a destination and make the source empty") {
	fill_in_from(xy, &m);
	list_add_last(element_in_list, &z, &n);

	list_bulk_add_first(&m, &n);

	test_assert_true(list_is_empty(&m));
	compare_with(xyz, &n);
}

TEST_CASE("list_bulk_add_last should move all the elements from a source "
		"list to the end of a destination and make the source empty") {
	list_add_first(element_in_list, &x, &n);
	fill_in_from(yz, &m);

	list_bulk_add_last(&m, &n);

	test_assert_true(list_is_empty(&m));
	compare_with(xyz, &n);
}

TEST_CASE("list_bulk_insert_before shouldn't modify a destination list "
		"if a source list is empty") {
	fill_in_from(xyz, &n);

	list_bulk_insert_before(&m, &y, lnk);

	test_assert_true(list_is_empty(&m));
	compare_with(xyz, &n);
}

TEST_CASE("list_bulk_insert_after shouldn't modify a destination list "
		"if a source list is empty") {
	fill_in_from(xyz, &n);

	list_bulk_insert_after(&m, &y, lnk);

	test_assert_true(list_is_empty(&m));
	compare_with(xyz, &n);
}

TEST_CASE("list_bulk_insert_before: inserting new elements before the last "
		"one in a list of two elements should insert them in the middle of the"
		"target list") {
	list_add_last(element_in_list, &y, &m);
	fill_in_from(xz, &n);

	list_bulk_insert_before(&m, &z, lnk);

	test_assert_true(list_is_empty(&m));
	compare_with(xyz, &n);
}

TEST_CASE("list_bulk_insert_after: inserting new elements after the first "
		"one in a list of two elements should insert them in the middle of the"
		"target list") {
	list_add_last(element_in_list, &y, &m);
	fill_in_from(xz, &n);

	list_bulk_insert_after(&m, &x, lnk);

	test_assert_true(list_is_empty(&m));
	compare_with(xyz, &n);
}

TEST_CASE("list_foreach_link on empty list must not execute the body and "
		"should not touch the iteration variable") {
	struct list_link *lnk = (struct list_link *) 0xc0ffee00;

	list_foreach_link(lnk, &m) {
		test_fail("the list must be empty");
	}

	test_assert_equal(lnk, (struct list_link *) 0xc0ffee00);
}

TEST_CASE("list_foreach_link on a single element list should execute the body "
		"only once") {
	struct list_link *lnk = NULL;
	int executed = 0;

	list_add_first(element_in_list, &x, &m);

	list_foreach_link(lnk, &m) {
		test_assert_zero(executed++);
		test_assert_equal(lnk, &x.lnk);
	}

	test_assert_not_zero(executed);
	test_assert_equal(lnk, &x.lnk);
}

TEST_CASE("list_foreach_link should continue iterating even if the value of "
		"the iteration variable is modified in the body") {
	struct list_link *lnk = NULL;
	struct element * const *p_element = xyz;

	fill_in_from(p_element, &m);

	list_foreach_link(lnk, &m) {
		test_assert_equal(lnk, &(*p_element++)->lnk);
		lnk = NULL;
	}

	test_assert_null(lnk);
}

TEST_CASE("list_foreach_link should evaluate the list argument only once") {
	struct list_link *lnk = NULL;
	int i = 0, eval_cnt = 0;

	fill_in_from(xyz, &m);

	list_foreach_link(lnk, eval_cnt++ ? &n : &m) {
		++i;
	}

	test_assert_equal(eval_cnt, 1);
	test_assert_equal(i, 3);
	test_assert_equal(lnk, &z.lnk);
}

TEST_CASE("list_foreach_link should support safe removal of the element "
		"pointed by the iteration variable") {
	struct list_link *lnk;

	fill_in_from(xyz, &m);

	list_foreach_link(lnk, &m) {
		list_unlink_link(lnk);
	}

	test_assert_true(list_is_empty(&m));
}

TEST_CASE("list_foreach simple test case") {
	struct element *e = NULL;
	struct element * const *p_element = xyz;

	fill_in_from(p_element, &m);

	list_foreach(e, &m, lnk) {
		test_assert_equal(e, *p_element++);
	}

	test_assert_equal(e, &z);
}

static struct list *fill_in_from(struct element * const array[],
		struct list *list) {
	struct element *e;

	test_assert_true(list_is_empty(list));
	array_nullterm_foreach(e, array) {
		test_assert_true(list_alone(element_in_list, e));
		list_add_last(element_in_list, e, list);
	}
	test_assert_false(list_is_empty(list));

	return list;
}

static struct list *compare_with(struct element * const array[],
		struct list *list) {
	struct element *e;

	array_nullterm_foreach(e, array) {
		test_assert_equal(list_remove_first(element_in_list, list), e);
	}
	test_assert_true(list_is_empty(list));

	return list;
}

static int setup(void) {
	list_init(&m);
	list_init(&n);
	list_link_init(&x.lnk);
	list_link_init(&y.lnk);
	list_link_init(&z.lnk);
	return 0;
}
