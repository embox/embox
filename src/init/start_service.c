/**
 * @file
 * @brief Starts services
 *
 * @date 05.04.16
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <util/array.h>
#include <embox/unit.h>
#include <framework/cmd/api.h>
#include <stdio.h>

#define CMD_BUF_SIZE 0x100
#define CMD_COUNT    0x10

EMBOX_UNIT_INIT(run_script);

static const char *script_commands[] = {
	#include <start_service.inc>
};

struct cmd_data {
	int argc;
	char *argv[CMD_BUF_SIZE];
	char buf[CMD_BUF_SIZE * 2];
	const struct cmd *cmd;
};

POOL_DEF(cmd_pool, struct cmd_data, CMD_COUNT);

static void cmd_data_copy(struct cmd_data *dst, const struct cmd_data *src) {
	int i_arg;
	char *dst_argv_p;

	dst->argc = src->argc;

	dst_argv_p = dst->buf;
	for (i_arg = 0; i_arg < src->argc; i_arg++) {

		strcpy(dst_argv_p, src->argv[i_arg]);
		dst->argv[i_arg] = dst_argv_p;

		dst_argv_p += strlen(src->argv[i_arg]);
		*dst_argv_p++ = '\0';
	}
	dst->argv[dst->argc] = NULL;
	dst->cmd = src->cmd;
}

static void *run_cmd(void *data) {
	int ret;
	struct cmd_data cdata;

	cmd_data_copy(&cdata, data);

	ret = cmd_exec(cdata.cmd, cdata.argc, cdata.argv);
	if (ret != 0) {
		printf("%s: Command returned with code %d: %s\n",
				cmd_name(cdata.cmd), ret, strerror(-ret));
		return (void *)ret; /* error: ret */
	}

	return NULL; /* ok */
}

static int run_script(void) {
	const char *command;
	struct cmd_data cdata;

	printf("Starting services:\n");
	array_foreach(command, script_commands, ARRAY_SIZE(script_commands)) {
		int ret;

		printf("> %s \n", command);

		new_task(cdata->argv[0], run_cmd, cdata);
	}

	return 0;
}
