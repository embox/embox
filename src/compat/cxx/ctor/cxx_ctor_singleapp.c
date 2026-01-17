/**
 * @file
 *
 * @date 30 Jan 2013
 * @author: Felix Sulima
 */

extern void cxx_invoke_constructors(void);
extern void cxx_invoke_destructors(void);

void cxx_app_startup(void) {
	cxx_invoke_constructors();
}

void cxx_app_termination(void) {
	cxx_invoke_destructors();
}
