/**
 * \file tests_contaner.c
 *
 * \date Dec 4, 2008
 * \author anton
 */
#include "types.h"
#include "common.h"
#include "express_tests.h"
/*
#include "tests_headers.h"


typedef int (*EXPRESS_TEST_FUNC)(void);

typedef struct _EXPRESS_TEST_DESCRIPTION {
	const char *name;
	const EXPRESS_TEST_FUNC handler;
} EXPRESS_TEST_DESCRIPTION;

EXPRESS_TEST_DESCRIPTION const express_tests_desc[] = {
	#include "tests_table.inc"
};

typedef struct _EXPRESS_TEST_STRUCT {
	EXPRESS_TEST_DESCRIPTION test_desc;
	int result;
	int error_code;
} EXPRESS_TEST_STRUCT;

#define QUANTITY_TESTS array_len(express_tests_desc)
static EXPRESS_TEST_STRUCT const express_tests [QUANTITY_TESTS];
static int cur_test_number = 0;

void *express_test_get_next() {
	if (cur_test_number >= QUANTITY_TESTS)
		return NULL;

	return (void *)&(express_tests_desc[cur_test_number ++]);
}

int express_test_get_last_error() {
	return 0;
}

int express_test_run (void *handler) {
	int result;
	EXPRESS_TEST_DESCRIPTION *test_desc = (EXPRESS_TEST_DESCRIPTION *)handler;
	if (NULL == test_desc)
		return FALSE;

	if (NULL == test_desc->handler)
		return FALSE;

	TRACE ("start test %s\n", test_desc->name);
	result = test_desc->handler();
	TRACE ("result ..... %s\n", result == 0 ? "OK" : "FAILED");
	return result;
}
int express_tests_execute(){
    void* descriptor;
    while (NULL != (descriptor = (void *) express_test_get_next())) {
            express_test_run(descriptor);
        }
    return 0;
}
*/

int express_tests_execute(){
    int retval = 0;
    extern char __express_tests_start, __express_tests_end;
    EXPRESS_TEST_HANDLER ** handler = (EXPRESS_TEST_HANDLER **)&__express_tests_start;
    for(; (unsigned int)handler < (unsigned int)&__express_tests_end; handler ++){
        if (NULL == (*handler)){
            TRACE("Error: Wrong express test handler\n");
            return -1;
        }
        if (NULL == ((*handler)->get_test_name)){
            TRACE("Error: Wrong express test handler. Can't find get_test_name function\n");
            return -1;
        }
        if (NULL == ((*handler)->exec)){
            TRACE("Error: Wrong express test handler. Can't find exec function for test %s\n", (*handler)->get_test_name());
            return -1;
        }
        TRACE("start test %s ... ", (*handler)->get_test_name());
        if (-1 == (*handler)->exec()){
            TRACE("FAILED\n");
            retval = -1;
            continue;
        }
        TRACE("OK\n");
    }
    return retval;
}
