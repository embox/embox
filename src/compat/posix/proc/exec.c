/**
 * @file
 *
 * @brief
 *
 * @date 13.09.2011
 * @author Anton Bondarev
 * @author Anton Kozlov
 * 	- shell invokation
 */

#include <string.h>
#include <cmd/shell.h>
#include <unistd.h>

int execv(const char *path, char *const argv[]) {
	const struct shell *sh;
	const char *default_shells[] = { "/bin/sh", "sh", NULL };
	const char **shellp;
	int ecode;

	sh = shell_lookup(path);
	if (!sh) {
		for (shellp = default_shells; shellp != NULL; shellp++) {
			if (!strcmp(*shellp, path)) {
				sh = shell_lookup("tish");
				break;
			}
		}
	}

	/* FIXME pass argv to shell_exec */
	ecode = sh ? shell_run(sh) : shell_exec(shell_any(), path);

	_exit(ecode);

	return 0;
}
