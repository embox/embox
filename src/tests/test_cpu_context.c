/**
 * \file test_cpu_context.c
 *
 *  \date Jan 26, 2009
 *  \author Anton Bondarev
 */
#include "types.h"
#include "common.h"
#include "cpu_context.h"
#include "sys.h"
#include "test_cpu_context.h"
#include "express_tests.h"

static CPU_CONTEXT context;

#define TEST_NAME "cpu context"
static char *get_test_name(){
	return TEST_NAME;
}

/**
 * in this trap function contex_save takes place
 * this function save all procesor context to pointed memory
 * then call function context_restore
 * @return 0 if success
 */
static int exec() {
    volatile static BOOL started;

    started = FALSE;
    context_save(&context);
    {
        if (!started) {
            started = TRUE;
        } else {
            return 0;
        }
    }
    context_restore(&context);

    // we'll never reach this line
    return -1;
}

static EXPRESS_TEST_HANDLER exp_handler = {get_test_name, exec};
REGISTER_EXPRESS_TEST(&exp_handler);
