/**
 * @file
 * @brief Starts services
 *
 * @date 16.04.16
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <kernel/task.h>
#include <kernel/sched.h>
#include <kernel/thread/thread_sched_wait.h>
#include <cmd/cmdline.h>
#include <framework/cmd/api.h>

#define SRV_CMD_BUF_SIZE 0x100

struct cmd_data {
	int argc;
	char *argv[SRV_CMD_BUF_SIZE];
	char buf[SRV_CMD_BUF_SIZE * 2];
	const struct cmd *cmd;
};

static volatile bool srv_cmd_ready_flag = false;

static void srv_cmd_data_copy(struct cmd_data *dst, const struct cmd_data *src) {
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

static int srv_cdata_fill(const char *cmdline, struct cmd_data *cdata) {
	if (strlen(cmdline) >= SRV_CMD_BUF_SIZE) {
		return -EINVAL;
	}

	strcpy(cdata->buf, cmdline);

	cdata->argc = cmdline_tokenize(cdata->buf, cdata->argv);
	if (cdata->argc == 0) {
		return -EINVAL;
	}

	cdata->cmd = cmd_lookup(cdata->argv[0]);
	if (cdata->cmd == NULL) {
		return -ENOENT;
	}

	return 0;
}

static void *srv_run_cmd(void *data) {
	int ret;
	struct cmd_data cdata;

	srv_cmd_data_copy(&cdata, data);
	srv_cmd_ready_flag = true;

	ret = cmd_exec(cdata.cmd, cdata.argc, cdata.argv);
	if (ret != 0) {
		printf("%s: Command returned with code %d: %s\n",
				cmd_name(cdata.cmd), ret, strerror(-ret));
		return (void *)ret; /* error: ret */
	}

	return NULL;
}

static void srv_print_services(void) {
	printf("LIST OF SERVICES IS NOT SUPPORTED YET\n");
}

int main(int argc, char **argv) {
	const char *command;
	struct cmd_data cdata;
	int res;

	if (argv[1] == NULL) {
		srv_print_services();
		return 0;
	}

	command = argv[1];
	printf("Starting service: %s\n", command);

	res = srv_cdata_fill(command, &cdata);
	if (res < 0) {
		return res;
	}
	srv_cmd_ready_flag = false;
	new_task(cdata.argv[0], srv_run_cmd, &cdata);
	SCHED_WAIT(srv_cmd_ready_flag);

	return 0;
}
