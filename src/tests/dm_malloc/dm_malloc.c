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
	test_struct_t *p;
	p = dm_malloc(sizeof(test_struct_t));
	p->i = 0;
	TRACE(p->i);
	dm_free(p);

	return result;
}
