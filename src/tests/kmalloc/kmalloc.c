/**
 * @file
 * @brief Test dynamic memory allocation
 * @auther Michail Skorginskii
 */

#include <stdio.h>

#include <embox/test.h>
#include <kernel/mm/kmalloc.h>
#include <lib/list.h>
#include <kernel/mm/mpallocator.h>

EMBOX_TEST(run);

/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
struct list {
	struct list_head *next, *prev;
	int p;
};

static LIST_HEAD(int_list);

static int run(void) {
	int result = 0;

	int i;
	struct list_head *tmp_h;
	struct list *tmp;

	putchar('\n');
	printf("\t\t = Allocate list =\n");
	for(i = 0; i <= 10; i++) {
		tmp = (struct list *)kmalloc(sizeof(struct list));
		if ( tmp == 0 ) {
			return -1;
		}
		tmp->p = i;
		list_add( (struct list_head*) tmp, &int_list);
		printf("tmp[%d] = %d \n ", i, tmp->p);
	}

	printf("\n\n\t\t = Test our list =\n");
	list_for_each( tmp_h, &int_list) {
		tmp = (struct list *)tmp_h;
		printf("tmp[%d] = %d \n ",tmp->p,tmp->p);
	}

	printf("\n\n\t\t = Test free =\n");
	list_for_each( tmp_h, &int_list) {
		tmp = (struct list *)tmp_h;
		printf("now tmp[%d] is free \n ", tmp->p);
	}
	TRACE("\ntest ");

	return result;
}
