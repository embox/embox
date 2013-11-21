/*

*/
#include <errno.h>
#include <assert.h>

#include <kernel/light_thread/light_thread.h>

void lthread_init(struct lthread *lt, void *(*run)(void *), void *arg) {
	assert(run);

	lt->run = run;
	lt->run_arg = arg;
}

struct lthread * lthread_create(void *(*run)(void *), void *arg) {
	struct lthread lt;

	lthread_init(*lt, run, arg);

	return *lt;
}
