#include <stdio.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(reent_init);

struct _reent {
	int _errno;           /* local copy of errno */

  /* FILE is a big struct and may change over time.  To try to achieve binary
     compatibility with future versions, put stdin,stdout,stderr here.
     These are pointers into member __sf defined below.  */
	FILE *_stdin, *_stdout, *_stderr;
};

struct _reent global_newlib_reent;

void *_impure_ptr = &global_newlib_reent;

static int reent_init(void) {
	global_newlib_reent._stdin = stdin;
	global_newlib_reent._stdout = stdout;
	global_newlib_reent._stderr = stderr;

	return 0;
}
