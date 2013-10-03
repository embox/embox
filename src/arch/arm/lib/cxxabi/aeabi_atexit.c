/**
 * @file
 *
 * @date 27.09.2013
 * @author Alexander Kalmuk
 */

extern int __cxa_atexit(void (*destructor) (void *), void *arg, void *__dso_handle);

int __aeabi_atexit(void *object, void (*dtor)(void *this), void *handle) {
	return __cxa_atexit(dtor, object, handle);
}
