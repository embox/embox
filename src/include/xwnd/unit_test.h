#ifndef XWND_UNIT_TEST_H_
#define XWND_UNIT_TEST_H_

#include <kernel/thread/sync/sem.h>
#include <stdio.h>

typedef int (*xwnd_unit_test_routine) (void);

enum xwnd_unit_test_state {XWND_TEST_FAIL, XWND_TEST_OK, XWND_TEST_NOT_DONE, XWND_TEST_ERROR_TEST_BEFORE_INIT};

struct xwnd_unit_test_node {
	const char * name;
	xwnd_unit_test_routine test;
	enum xwnd_unit_test_state state;
	struct xwnd_unit_test_node * next;
};

struct xwnd_unit_test_queue {
	struct xwnd_unit_test_node * first;
	struct xwnd_unit_test_node * last;
};

struct xwnd_unit_test_data {
	FILE * log_file;
	sem_t test_sem;
	struct xwnd_unit_test_queue queue;
	int initialized;
};

extern int xwnd_unit_test_init (const char * log_name);
extern void xwnd_unit_test_quit (void);
extern int xwnd_unit_test_self_test (void);
extern int xwnd_unit_test_run_test (const char * test_name, xwnd_unit_test_routine test);
extern int xwnd_unit_test_add_test (const char * test_name, xwnd_unit_test_routine test);
extern int xwnd_unit_test_run_sheduled_test (struct xwnd_unit_test_node * test);
extern int xwnd_unit_test_run_all_sheduled_tests (void);

#endif /*XWND_UNIT_TEST_H_*/
