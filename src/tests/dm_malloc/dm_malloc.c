/**
 * @file
 * @brief Test dynamic memory allocation
 * @auther Michail Skorginskii
 */

#include <embox/test.h>
#include <lib/dm_malloc.h>
#include <lib/list.h>

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
	struct list *tmp;

	putchar('\n');
	for(i = 0; i <= 4; i++) {
		tmp = dm_malloc(sizeof(struct list));
		tmp->p = i*2;
		list_add( (struct list_head*) tmp, &int_list);
		printf("element %d added... Equal %d\n", i, tmp->p);
	}

	list_for_each( (struct list_head*) tmp, &int_list) {
		printf("element %d equal %d\n",tmp->p/2, tmp->p);
	}

	list_for_each( (struct list_head*) tmp, &int_list) {
		printf("element %d free... \n", tmp->p/2);
		if ((tmp->p/2)==3) {
			break;
		}
	}
	TRACE("test");
	return result;
}
