/**
 * @file
 * @brief Test unit for util/set.
 *
 * @date Oct 8, 2001
 * @author Avdyukhin Dmitry
 */

#include <embox/test.h>
#include <stdio.h>
#include <string.h>
#include <util/set.h>
#include <util/rand.h>

EMBOX_TEST_SUITE("util/set test");

static struct set simple_set;
static struct set_link set_links[3];
static int set_address_comp(struct set_link* first, struct set_link* second) {
	return first - second;
}

TEST_CASE("Any test for set") {
	struct set_link *link;
	test_emit('-');
	set_init(&simple_set);
	set_min_link(&simple_set);
	set_add_link(&simple_set, set_links + 1, set_address_comp);
	set_add_link(&simple_set, set_links + 0, set_address_comp);
	set_add_link(&simple_set, set_links + 2, set_address_comp);
	set_foreach_link(link, &simple_set) { }
}

struct int_set_element {
	int data;
	struct set_link link;
};

static int set_int_comp(struct set_link *first, struct set_link *second) {
	return set_element(first, struct int_set_element, link)->data
		- set_element(second, struct int_set_element, link)->data;
}

#define ELEMENT_COUNT 100

static struct int_set_element elements[ELEMENT_COUNT];
static int elem_cnt;
int add_cnt = ELEMENT_COUNT >> 1, del_cnt = ELEMENT_COUNT >> 1;

static struct set set;

static struct int_set_element *ideal[ELEMENT_COUNT];
static int ideal_size;

void array_add(struct int_set_element *elem) {
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

void array_del(struct int_set_element * elem) {
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
	struct set_link *link;
	struct int_set_element *elem;
	int i = 0;
	set_foreach(link, elem, &set, link) {
//		if ((int)elem != (int)ideal[i]) {
		if (elem->data != ideal[i]->data) {
			return false;
		}
		i++;
	}
	assert(i == ideal_size);
	return true;
}
#if 0
static void print(void){
	int i;
	struct set_link *link;
	struct int_set_element *elem;
	printf ("\n");
	for (i = 0; i < ideal_size; i++) {
		printf ("%d ", ideal[i]->data);
	}
	printf ("\n");
	set_foreach(link, elem, &set, link) {
		printf ("%d ", elem->data);
	}
	printf ("\n");
}
#endif
static void add(int num) {
//	int i;
	struct int_set_element *elem = elements + elem_cnt++;
	elem->data = num;
	set_add_link(&set, &elem->link, set_int_comp);
	array_add(elem);
	//print();
	assert(compare());
}

static void del(int num) {
	struct int_set_element *elem = elements + elem_cnt++;
	elem->data = num;
	set_remove_link(&set, &elem->link, set_int_comp);
	array_del(elem);
	//print();
	assert(compare());
}

TEST_CASE("Big random test for set") {
	int i, num;
	set_init(&set);
	ideal_size = 0;
	//printf("Add... ");
	for (i = 0; i < add_cnt; i++) {
		num = rand() % 100;
		add(num);
	}
	//printf("Del...");
	for (i = 0; i < del_cnt; i++) {
		num = rand() % 100;
		del(num);
	}
}

