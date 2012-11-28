#include <xwnd/unit_test.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

static struct xwnd_unit_test_data data;

int xwnd_unit_test_self_test () {
	return 1;
}

void xwnd_unit_test_log_test (const char * test_str, int status) {
	/*fprintf(data.log_file, "%s ... ", test_str);
	switch (status) {
		case XWND_TEST_FAIL:
			fprintf(data.log_file, "[FAIL]\n");
			break;
		case XWND_TEST_OK:
			fprintf(data.log_file, "[OK]\n");
			break;
		case XWND_TEST_NOT_DONE:
			fprintf(data.log_file, "[NOT DONE]\n");
			break;
		case XWND_TEST_ERROR_TEST_BEFORE_INIT:
			fprintf(data.log_file, "[TEST BEFORE INIT]\n");
			break;
		default:
			fprintf(data.log_file, "[UNKNOWN ERROR]\n");
			break;
	}*/
}

static void xwnd_unit_test_print_log (const char * test_str, int status) {
	printf("%s ... ", test_str);
	switch (status) {
		case XWND_TEST_FAIL:
			printf("[FAIL]\n");
			break;
		case XWND_TEST_OK:
			printf("[OK]\n");
			break;
		case XWND_TEST_NOT_DONE:
			printf("[NOT DONE]\n");
			break;
		case XWND_TEST_ERROR_TEST_BEFORE_INIT:
			printf("[TEST BEFORE INIT]\n");
			break;
		default:
			printf("[UNKNOWN ERROR]\n");
			break;
	}
}

int xwnd_unit_test_init (const char * log_name) {

	sem_init(&data.test_sem, 2);
	sem_enter(&data.test_sem);

	if (!log_name) {
		data.log_file = NULL;
	} else {
		data.log_file = fopen(log_name, "w");
	}

	data.queue.first = NULL;
	data.queue.last = NULL;

	data.initialized = 1;
	sem_leave(&data.test_sem);

	/*xwnd_unit_test_add_test ("XWnd UnitTest self-testing", xwnd_unit_test_self_test);*/
	return 1;
}

int xwnd_unit_test_add_test (const char * test_name, xwnd_unit_test_routine test) {
	struct xwnd_unit_test_node * new_test;

	new_test = malloc (sizeof(struct xwnd_unit_test_node));
	if (!new_test) {
		return 0;
	}

	new_test->next = NULL;
	new_test->test = test;
	/*FIXME: Make it copy test name, not only assign the pointer to it. May cause stupid behaviour in multithred environment*/
	new_test->name = test_name;
	new_test->state = XWND_TEST_NOT_DONE;

	sem_enter(&data.test_sem);
	if (data.queue.first == NULL) {
		data.queue.first = new_test;
		data.queue.last = new_test;
	} else {
		data.queue.last->next = new_test;
		data.queue.last = new_test;
	}
	sem_leave(&data.test_sem);
	return 1;
}

void xwnd_unit_test_quit () {
	struct xwnd_unit_test_node * tmp, * nxt;
	sem_enter(&data.test_sem);
	if (!data.initialized) {
		return;
	}
	for (tmp = data.queue.first; nxt = tmp->next, tmp; tmp = nxt) {
		free (tmp);
	}
	if (data.log_file) {
		fclose(data.log_file);
	}
	data.initialized = 0;
	sem_leave(&data.test_sem);
}

int xwnd_unit_test_run_test (const char * test_name, xwnd_unit_test_routine test) {
	int result = XWND_TEST_NOT_DONE;

	sem_enter(&data.test_sem);
	if (!data.initialized) {
		result = XWND_TEST_ERROR_TEST_BEFORE_INIT;
	} else if (test) {
		result = test();
	}
	xwnd_unit_test_print_log(test_name, result);
	sem_leave(&data.test_sem);
	return result;
}

int xwnd_unit_test_run_sheduled_test (struct xwnd_unit_test_node * test) {
	test->state = xwnd_unit_test_run_test (test->name, test->test);
	return test->state;
}

int xwnd_unit_test_run_all_sheduled_tests (void) {
	struct xwnd_unit_test_node * tmp, * nxt;
	int result = XWND_TEST_OK;
	for (tmp = data.queue.first; nxt = tmp->next, tmp; tmp = nxt) {
		if (xwnd_unit_test_run_sheduled_test(tmp) != XWND_TEST_OK) {
			result = XWND_TEST_FAIL;
		}
	}
	return result;
}
