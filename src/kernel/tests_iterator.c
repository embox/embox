/*
 * tests_contaner.c
 *
 *  Created on: Dec 4, 2008
 *      Author: anton
 */
#include "types.h"
#include "common.h"
#include "tests_headers.h"


typedef int (*EXPRESS_TEST_FUNC)(void);
typedef struct _EXPRESS_TEST_DESCRIPTION
{
	const char *name;
	const EXPRESS_TEST_FUNC handler;
}EXPRESS_TEST_DESCRIPTION;

EXPRESS_TEST_DESCRIPTION const express_tests_desc[] =
{
#include "tests_table.inc"
};
typedef struct _EXPRESS_TEST_STRUCT
{
	EXPRESS_TEST_DESCRIPTION test_desc;
	int result;
	int error_code;
}EXPRESS_TEST_STRUCT;


#define QUANTITY_TESTS (sizeof (express_tests_desc)/ sizeof (express_tests_desc[0]))
static EXPRESS_TEST_STRUCT const express_tests [QUANTITY_TESTS];
static int cur_test_number = 0;

void *express_test_get_next()
{
	if (cur_test_number >= QUANTITY_TESTS)
		return NULL;

	return (void *)&(express_tests_desc[cur_test_number ++]);
}
int express_test_get_last_error()
{
	return 0;
}
int express_test_run (void *handler)
{
	int result;
	EXPRESS_TEST_DESCRIPTION *test_desc = (EXPRESS_TEST_DESCRIPTION *)handler;
	if (NULL == test_desc)
		return FALSE;

	TRACE ("start test %s\n", test_desc->name);
	result = test_desc->handler();
	TRACE ("result %d\n", result);
	return result;
}
