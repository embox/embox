/**
 * @file
 *
 * @date Aug 31, 2021
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <cmd/shell.h>

#include <framework/mod/options.h>

int main(int argc, char **argv) {
	FILE *file;
	char buf[128];
	char *line;
	const struct shell *shell;

	if(argc != 2) {
		return -EINVAL;
	}

	file = fopen(argv[1], "r");
	if (!file) {
		return -EINVAL;
	}

	shell = shell_any();
	if (NULL == shell) {
		printf("any shell had not been found\n");
		fclose(file);
		return -ENOENT;
	}


	while (!feof(file)) {
		memset(buf, 0, sizeof(buf));
		line = fgets(buf, sizeof(buf), file);
		if (line == NULL) {
			break;
		}
		if (line[0] == '#' ) {
			continue;
		}
		if (strlen(line) == 1) {
			continue; /* empty line */
		}
		shell_exec(shell, line);
	}

	fclose(file);
	return 0;
}
