/**
 * @file
 *
 * @brief
 *
 * @date 12.05.2012
 * @author Anton Bondatev
 */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <util/array.h>
#include <framework/cmd/api.h>

int main(int argc, char **argv) {
	const struct cmd *cmd;

	if (argc <= 1) {
		printf("Please enter command name\n");
		return -EINVAL;
	}

	if(NULL == (cmd = cmd_lookup(argv[1]))) {
		printf("wrong command name\n");
		return -EINVAL;
	}

	cmd->exec(argc - 1, &argv[1]);

	return 0;
}
