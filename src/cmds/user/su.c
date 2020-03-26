/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.03.2013
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <framework/cmd/api.h>

#include <kernel/task/resource/security.h>
#include <kernel/task/resource/u_area.h>
#include <security/smac/smac.h>

int main(int argc, char *argv[]) {
	struct task_u_area *uarea = task_self_resource_u_area();
	const struct cmd *login_cmd = cmd_lookup("login");
	uid_t euid = geteuid();
	uid_t reuid = getuid();
	gid_t egid = getegid();
	gid_t regid = getgid();
	char old_smac_label[SMAC_LABELLEN];
	int opt, ret;
	char *cmd = NULL, *name = "root";
	char *newargv[5];
	int newargc;


	while (-1 != (opt = getopt(argc, argv, "c:"))) {
		switch(opt) {
		case 'c':
			cmd = optarg;
			break;
		default:
			break;
		}
	}

	if (optind < argc) {
		name = argv[optind];
	}

	uarea->reuid = uarea->euid = 0;
	uarea->regid = uarea->egid = 0;
	strcpy(old_smac_label, task_self_resource_security());
	strcpy(task_self_resource_security(), smac_admin);

	if (cmd) {
		char *nargv[] = {"", "-c", cmd};
		newargc = 3;
		memcpy(newargv, nargv, sizeof(nargv));
	} else {
		char *nargv[] = {""};
		newargc = 1;
		memcpy(newargv, nargv, sizeof(nargv));
	}

	if (!euid) {
		newargv[newargc++] = "-p";
	}

	newargv[newargc++] = name;

	ret = cmd_exec(login_cmd, newargc, newargv);

	if (ret) {
		printf("%s: incorrect password\n", argv[0]);
	}

	strcpy(task_self_resource_security(), old_smac_label);
	uarea->reuid = reuid;
	uarea->euid = euid;
	uarea->regid = regid;
	uarea->egid = egid;

	return 0;

}

