/**
 * @file
 * @brief FTP client
 *
 * @date 06.08.12
 * @author Ilia Vaprol
 */

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <net/socket.h>
#include <net/ip.h>
#include <net/in.h>
#include <embox/cmd.h>

EMBOX_CMD(exec);

enum {
	FTP_RET_OK,
	FTP_RET_ERROR,
	FTP_RET_EXIT
};

/* FTP Session info */
struct fs_info {
	int cmd_sock;      /* command socket */
};

/* FTP Method info */
struct fm_info {
	const char *mtd_name;
	const char *mtd_usage;
	const char *mtd_description;
	int (*mtd_hnd)(struct fs_info *);
};

/* FTP Session commands */
static int fs_cmd_open(struct fs_info *session);
static int fs_cmd_close(struct fs_info *session);
static int fs_cmd_user(struct fs_info *session);
static int fs_cmd_cd(struct fs_info *session/*, const char *remote_dir*/);
static int fs_cmd_pwd(struct fs_info *session);
static int fs_cmd_mkdir(struct fs_info *session/*, const char *remote_dir*/);
static int fs_cmd_rmdir(struct fs_info *session/*, const char *remote_dir*/);
static int fs_cmd_mv(struct fs_info *session/*, const char *remote_old, const char *remote_new*/);
static int fs_cmd_ls(struct fs_info *session/*, const char *remote_dir*/);
static int fs_cmd_get(struct fs_info *session/*, const char *remote_file, const char *local_file*/);
static int fs_cmd_put(struct fs_info *session/*, const char *local_file, const char *remote_file*/);
static int fs_cmd_rm(struct fs_info *session/*, const char *remote_file*/);
static int fs_cmd_help(struct fs_info *session);
static int fs_cmd_bye(struct fs_info *session);

/* Table of FTP Methods */
static struct fm_info ftp_mtds[] = {
	{ "open", "open <host-name> [port]", "connect to remote ftp", &fs_cmd_open },
	{ "close", "close", "terminate ftp session", &fs_cmd_close },
	{ "user", "user <user-name>", "login on remote ftp", &fs_cmd_user },
	{ "cd", "cd <remote-directory>", "change remote working directory", &fs_cmd_cd },
	{ "pwd", "pwd", "print remote working directory", &fs_cmd_pwd },
	{ "mkdir", "mkdir <directory-name>", "make directory on the remote machine", &fs_cmd_mkdir },
	{ "rmdir", "rmdir <directory-name>", "remove directory on the remote machine", &fs_cmd_rmdir },
	{ "mv", "mv <from-name> <to-name>", "move files", &fs_cmd_mv },
	{ "ls", "ls [remote-directory]", "list contents of remote directory", &fs_cmd_ls },
	{ "get", "get <remote-file> [local-file]", "receive file", &fs_cmd_get },
	{ "put", "put <local-file> <remote-file>", "send one file", &fs_cmd_put },
	{ "rm", "rm <remote-file>", "delete remote file", &fs_cmd_rm },
	{ "help", "help", "print local help information", &fs_cmd_help },
	{ "bye", "bye", "terminate ftp session and exit", &fs_cmd_bye }
};

/* Execute the command for current FTP Session */
static int fs_execute(const char *cmd, struct fs_info *session) {
	int ret;
	char answer[256];

	ret = sendto(session->cmd_sock, cmd, strlen(cmd), 0, 0, 0);
	if (ret < 0) {
		return ret;
	}

	ret = recvfrom(session->cmd_sock, &answer[0], sizeof answer, 0, 0, 0);
	if (ret <= 0) {
		return ret;
	}

	answer[ret] = '\0';
	puts(answer);

	sscanf(answer, "%d", &ret);

	return ret;
}

/* Realization of ftp client's commands */
static int fs_cmd_open(struct fs_info *session) {
	int sock;
	struct sockaddr_in remote;
	char *hostname;

	hostname = "10.0.2.10";

	if (inet_aton(hostname, &remote.sin_addr) == 0) {
		fprintf(stderr, "Can't parse IP address\n");
		errno = EINVAL;
		return FTP_RET_ERROR;
	}
	remote.sin_port = htons(21);
	remote.sin_family = AF_INET;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		fprintf(stderr, "Can't create command socket\n");
		return FTP_RET_ERROR;
	}

	if (connect(sock, (struct sockaddr *)&remote, sizeof remote) < 0) {
		fprintf(stderr, "Can't connect to remote host\n");
		close(sock);
		return FTP_RET_ERROR;
	}

	printf("Connected to %s\n", hostname);

	session->cmd_sock = sock;

	return FTP_RET_OK;
}

static int fs_cmd_close(struct fs_info *session) {
	if (session->cmd_sock >= 0) {
		close(session->cmd_sock);
		session->cmd_sock = -1;
	}

	return FTP_RET_OK;
}

static int fs_cmd_user(struct fs_info *session) {
	/*int ret;*/

	fs_execute("USER anonymous\r\n", session);
	fs_execute("PASS blablabla\r\n", session);

	return FTP_RET_OK;
}

static int fs_cmd_cd(struct fs_info *session) { return FTP_RET_ERROR; }
static int fs_cmd_pwd(struct fs_info *session) { return FTP_RET_ERROR; }
static int fs_cmd_mkdir(struct fs_info *session) { return FTP_RET_ERROR; }
static int fs_cmd_rmdir(struct fs_info *session) { return FTP_RET_ERROR; }
static int fs_cmd_mv(struct fs_info *session) { return FTP_RET_ERROR; }
static int fs_cmd_ls(struct fs_info *session) { return FTP_RET_ERROR; }
static int fs_cmd_get(struct fs_info *session) { return FTP_RET_ERROR; }
static int fs_cmd_put(struct fs_info *session) { return FTP_RET_ERROR; }
static int fs_cmd_rm(struct fs_info *session) { return FTP_RET_ERROR; }

static int fs_cmd_help(struct fs_info *session) {
	size_t i;

	printf("Use the next commands:\n");
	for (i = 0; i < sizeof ftp_mtds / sizeof ftp_mtds[0]; ++i) {
		printf("\t%s -- %s\n", ftp_mtds[i].mtd_usage, ftp_mtds[i].mtd_description);
	}

	return FTP_RET_OK;
}

static int fs_cmd_bye(struct fs_info *session) {
	fs_cmd_close(session);
	return FTP_RET_EXIT;
}

static int exec(int argc, char **argv) {
	int ret;
	size_t i;
	struct fs_info fsi;
	char buff[128];

	fsi.cmd_sock = -1; /* initialize FTP Session Information structure */

	while (1) {
		printf("ftp> ");
		scanf("%s", &buff[0]); /* XXX hope overflow not happen */

		if (strlen(&buff[0]) == 0) { /* Skip empty commands */
			continue;
		}

		for (i = 0; i < sizeof ftp_mtds / sizeof ftp_mtds[0]; ++i) {
			if (strcmp(ftp_mtds[i].mtd_name, &buff[0]) == 0) {
				ret = (*ftp_mtds[i].mtd_hnd)(&fsi);
				break;
			}
		}
		if (i == sizeof ftp_mtds / sizeof ftp_mtds[0]) {
			printf("%s: unknown command `%s'\n", argv[0], &buff[0]);
			continue;
		}
		else if (ret == FTP_RET_ERROR) {
			printf("%s: error occurred: code=%d, description=%s", argv[0], errno, strerror(errno));
			continue;
		}
		else if (ret == FTP_RET_EXIT) {
			break; /* wanna exit from ftp client */
		}
		/* ASSERT ret is FTP_RET_OK */
		/* nothing to do */
	}

	return 0;
}

