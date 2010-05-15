/**
 * @file
 * @brief Test dynamic memory allocation
 * @auther Michail Skorginskii
 */

#include <embox/test.h>
#include <lib/dm_malloc.h>

EMBOX_TEST(run);

/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
typedef struct test_struct
{
	int i;
} test_struct_t;

static int run(void) {
	int result = 0;
	int g;

	test_struct_t *p;
	p = (test_struct_t*) dm_malloc( sizeof(test_struct_t) );

	printf("\n Allocated adress is %d", dm_malloc( 1 ) );

	if (p == 0) {
		return -1;
	}

	p->i = 666;
	TRACE("\nTrace: ");
	g = p->i;
	printf("%d\n", p->i);
	dm_free(p);

	return result;
}
