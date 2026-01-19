/**
 * @file
 *
 * @date 30 Jan 2013
 * @author: Felix Sulima
 */

#include <embox/unit.h>
#include <util/log.h>

#define CXA_ATEXIT_TABLE_SIZE OPTION_GET(NUMBER, cxa_atexit_table_size)

static struct {
	void (*dtor)(void *);
	void *obj_ptr;
	void *dso_handle;
} cxa_atexit_table[CXA_ATEXIT_TABLE_SIZE];

static int cxa_atexit_counter = 0;

/* must be available for C++ ABI */
void *__dso_handle = 0;

int __cxa_atexit(void (*f)(void *), void *objptr, void *dso) {
	if (cxa_atexit_counter >= CXA_ATEXIT_TABLE_SIZE) {
		log_error("__cxa_atexit: static destruction table overflow");
		return -1;
	}

	cxa_atexit_table[cxa_atexit_counter].dtor = f;
	cxa_atexit_table[cxa_atexit_counter].obj_ptr = objptr;
	cxa_atexit_table[cxa_atexit_counter].dso_handle = dso;
	cxa_atexit_counter++;

	return 0;
};

void __cxa_finalize(void *f) {
	int i = cxa_atexit_counter;

	if (!f) {
		while (i--) {
			if (cxa_atexit_table[i].dtor) {
				(*cxa_atexit_table[i].dtor)(cxa_atexit_table[i].obj_ptr);
				cxa_atexit_table[i].dtor = 0;
			}
		}
		cxa_atexit_counter = 0;
	}
	else {
		for (; i >= 0; --i) {
			if (cxa_atexit_table[i].dtor == f) {
				(*cxa_atexit_table[i].dtor)(cxa_atexit_table[i].obj_ptr);
				cxa_atexit_table[i].dtor = 0;
			}
		}
	}
}

#if 0
int __cxa_thread_atexit(void (*dtor)(void *), void *obj, void *dso_handle) {
	/* TODO */
	return 0;
}
#endif
