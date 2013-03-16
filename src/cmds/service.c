/**
 * @file
 * @brief
 *
 * @date 16.03.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <embox/cmd.h>
#include <stdio.h>
#include <util/array.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <cmd/shell.h>

EMBOX_CMD(exec);

struct serv_cmd {
	const char *name;
	const char *cmd;
};

struct serv_info {
	const char *serv;
	const struct serv_cmd **cmds;
};

static const struct serv_info servs[] = {
	{
		.serv = "httpd",
		.cmds = (const struct serv_cmd *[]){
			&(struct serv_cmd){
				.name = "start",
				.cmd = "servd"
			},
			&(struct serv_cmd){
				.name = "stop",
				.cmd = "servd -S"
			},
			&(struct serv_cmd){
				.name = "status",
				.cmd = "servd --status"
			},
			NULL
		}
	}
};

static const struct serv_info * serv_info_lookup(const char *serv) {
	const struct serv_info *si;

	assert(serv != NULL);

	array_foreach_ptr(si, servs, ARRAY_SIZE(servs)) {
		if (strcmp(serv, si->serv) == 0) {
			return si;
		}
	}

	return NULL;
}

static const struct serv_cmd * serv_cmd_lookup(const char *name,
		const struct serv_info *si) {
	const struct serv_cmd *sc;

	assert(name != NULL);
	assert(si != NULL);

	array_nullterm_foreach(sc, si->cmds) {
		if (strcmp(name, sc->name) == 0) {
			return sc;
		}
	}

	return NULL;
}

static int exec(int argc, char **argv) {
	int ind;
	const struct serv_info *sinfo;
	const struct serv_cmd *scmd;

	if (argc == 1) {
		return -EINVAL;
	}

	sinfo = NULL;
	scmd = NULL;

	for (ind = 1; ind < argc; ++ind) {
		if ((strcmp(argv[ind], "-h") == 0)
				|| (strcmp(argv[ind], "--help") == 0)) {
			printf("Usage: %s --status-all | service_name [command]\n", argv[0]);
			return 0;
		}
		else if (strcmp(argv[ind], "--status-all") == 0) {
			return -ENOSYS;
		}
		else if (sinfo == NULL) {
			sinfo = serv_info_lookup(argv[ind]);
			if (sinfo == NULL) {
				printf("%s: %s: unrecognized service\n",
						argv[0], argv[ind]);
				return -ESRCH;
			}
		}
		else {
			scmd = serv_cmd_lookup(argv[ind], sinfo);
			if (scmd == NULL) {
				printf("%s: %s: invalid command\n",
						argv[0], argv[ind]);
				return -EINVAL;
			}
			break;
		}
	}

	assert(sinfo != NULL);

	if (scmd == NULL) {
		printf("Usage: %s %s COMMAND\n", argv[0],
				sinfo->serv);
		return -EINVAL;
	}

	return shell_exec(shell_any(), scmd->cmd);
}
