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
#include <ctype.h>
#include <unistd.h>
#include <embox/cmd.h>

EMBOX_CMD(exec);

/* Global return codes */
enum {
	FTP_RET_OK,
	FTP_RET_FAIL,
	FTP_RET_ERROR,
	FTP_RET_EXIT
};

/* Getters fo status code */
#define FTP_STAT_TYPE(stat) (stat / 100)
#define FTP_STAT_KIND(stat) (stat % 100 / 10)

/* Useful commands */
#define skip_spaces(ptr) while (*ptr && isspace(*ptr)) ptr++
#define skip_word(ptr) while (*ptr && !isspace(*ptr)) ptr++
static void split_word(char *ptr) { skip_word(ptr); *ptr = '\0'; }

/* Status code for first digit (X pos) of result (*YZ) */
enum {
	FTP_STAT_TYPE_POSITIVE_PRELIMINARY = 1,
	FTP_STAT_TYPE_POSITIVE_COMPLETION = 2,
	FTP_STAT_TYPE_POSITIVE_INTERMEDIATE = 3,
	FTP_STAT_TYPE_TRANSIENT_NEGATIVE_COMPLETION = 4,
	FTP_STAT_TYPE_PERMANENT_NEGATIVE_COMPLETION = 5
};

/* Status code for second digit (Y pos) of result (X*Z) */
enum {
	FTP_STAT_KIND_SYNTAX = 0,
	FTP_STAT_KIND_INFORMATION = 1,
	FTP_STAT_KIND_CONNECTIONS = 2,
	FTP_STAT_KIND_AUTHENTICATION = 3,
	FTP_STAT_KIND_UNSPECIFIED = 4,
	FTP_STAT_KIND_FILE_SYSTEM = 5
};

/* FTP Session info */
struct fs_info {
	char cmd_buff[128]; /* command buffer */
	int cmd_sock;       /* command socket */
	int is_connected;   /* connection is esteblishment */
	int stat_code;      /* result of last command */
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
static int fs_snd_request(const char *cmd, struct fs_info *session) {
	int ret;

	ret = sendto(session->cmd_sock, cmd, strlen(cmd), 0, 0, 0);
	if (ret <= 0) {
		fprintf(stderr, "Can't send data\n");
		return FTP_RET_ERROR;
	}

	return FTP_RET_OK;
}

static int fs_rcv_reply(struct fs_info *session) {
	int ret;
	char answer[256];

	ret = recvfrom(session->cmd_sock, &answer[0], sizeof answer - 1, 0, 0, 0);
	if (ret <= 0) {
		fprintf(stderr, "Can't receive data\n");
		return FTP_RET_ERROR;
	}

	answer[ret] = '\0';
	fprintf(stdout, "%s", answer);

	ret = sscanf(answer, "%d", &session->stat_code);
	if (ret != 1) {
		return FTP_RET_ERROR;
	}

	return FTP_RET_OK;
}

static int fs_execute(const char *cmd, struct fs_info *session) {
	int ret;

	ret = fs_snd_request(cmd, session);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	ret = fs_rcv_reply(session);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	return FTP_RET_OK;
}

/* Realization of ftp client's commands */
static int fs_cmd_open(struct fs_info *session) {
	struct sockaddr_in remote_addr;
	char *tmp, *arg_hostname, *arg_port;
	unsigned int arg_port_int;

	if (session->is_connected) {
		fprintf(stderr, "Already connected, use close first.\n");
		return FTP_RET_FAIL;
	}

	/* Get args. Format: open <host-name> [port] */
	tmp = &session->cmd_buff[0];
	skip_spaces(tmp); skip_word(tmp); skip_spaces(tmp);
	arg_hostname = tmp;
	skip_word(tmp); skip_spaces(tmp);
	arg_port = tmp;

	split_word(arg_hostname);
	split_word(arg_port);

	if (*arg_hostname == '\0') {
		fprintf(stderr, "Remote address is not specified.\n");
		errno = EINVAL;
		return FTP_RET_ERROR;
	}

	if (*arg_port == '\0') {
		arg_port = NULL;
	}
	/* --- END OF Get args --- */

	memset(&remote_addr, 0, sizeof remote_addr);

	if (!inet_aton(arg_hostname, &remote_addr.sin_addr)) {
		fprintf(stderr, "Can't parse IP address.\n");
		errno = EINVAL;
		return FTP_RET_ERROR;
	}
	if (arg_port == NULL) {
		arg_port_int = 21; /* Use default settings */
	}
	else {
		if (sscanf(arg_port, "%u", &arg_port_int) != 1) {
			fprintf(stderr, "Can't parse port '%s`.\n", arg_port);
			errno = EINVAL;
			return FTP_RET_ERROR;
		}
	}
	remote_addr.sin_port = htons(arg_port_int);
	remote_addr.sin_family = AF_INET;

	session->cmd_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (session->cmd_sock < 0) {
		fprintf(stderr, "Can't create command socket.\n");
		return FTP_RET_ERROR;
	}

	if (connect(session->cmd_sock, (struct sockaddr *)&remote_addr, sizeof remote_addr) < 0) {
		fprintf(stderr, "Can't connect to remote host %s:%u.\n", arg_hostname, arg_port_int);
		close(session->cmd_sock);
		return FTP_RET_ERROR;
	}

	fprintf(stdout, "Connected to %s:%u.\n", arg_hostname, arg_port_int);
	session->is_connected = 1;

	if (fs_rcv_reply(session) != 0) {
		return FTP_RET_ERROR;
	}

	return FTP_RET_OK;
}

static int fs_cmd_close(struct fs_info *session) {
	int ret;

	if (!session->is_connected) {
		fprintf(stderr, "Not connected.\n");
		return FTP_RET_FAIL;
	}

	ret = fs_execute("QUIT\r\n", session);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	close(session->cmd_sock);
	session->is_connected = 0;

	return FTP_RET_OK;
}

static int fs_cmd_user(struct fs_info *session) {
	int ret;
	char *tmp, *arg_username, password[32];

	if (!session->is_connected) {
		fprintf(stderr, "Not connected.\n");
		return FTP_RET_FAIL;
	}

	/* Get args. Format: user <user-name> */
	tmp = &session->cmd_buff[0];
	skip_spaces(tmp); skip_word(tmp); skip_spaces(tmp);
	arg_username = tmp;

	split_word(arg_username);

	if (*arg_username == '\0') {
		fprintf(stderr, "User name is not specified.\n");
		errno = EINVAL;
		return FTP_RET_ERROR;
	}
	/* --- END OF Get args --- */

	sprintf(&session->cmd_buff[0], "USER %s\r\n", arg_username);

	ret = fs_execute(&session->cmd_buff[0], session);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	fprintf(stdout, "Password: ");
	if (fgets(&password[0], sizeof password, stdin) == NULL) {
		return FTP_RET_FAIL;
	}

	sprintf(&session->cmd_buff[0], "PASS %s\r\n", &password[0]);

	ret = fs_execute(&session->cmd_buff[0], session);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	return FTP_RET_OK;
}

static int fs_cmd_cd(struct fs_info *session) { return FTP_RET_FAIL; }
static int fs_cmd_pwd(struct fs_info *session) { return FTP_RET_FAIL; }
static int fs_cmd_mkdir(struct fs_info *session) { return FTP_RET_FAIL; }
static int fs_cmd_rmdir(struct fs_info *session) { return FTP_RET_FAIL; }
static int fs_cmd_mv(struct fs_info *session) { return FTP_RET_FAIL; }

static int fs_cmd_ls(struct fs_info *session) {
	int ret;
	char *tmp, *arg_remotedir;

	if (!session->is_connected) {
		fprintf(stderr, "Not connected.\n");
		return FTP_RET_FAIL;
	}

	/* Get args. Format: ls [remote-directory] */
	tmp = &session->cmd_buff[0];
	skip_spaces(tmp); skip_word(tmp); skip_spaces(tmp);
	arg_remotedir = tmp;

	split_word(arg_remotedir);

	if (*arg_remotedir == '\0') {
		arg_remotedir = NULL;
	}
	/* --- END OF Get args --- */

	if (arg_remotedir == NULL) {
		sprintf(&session->cmd_buff[0], "LIST\r\n");
	}
	else {
		sprintf(&session->cmd_buff[0], "LIST %s\r\n", arg_remotedir);
	}

	ret = fs_execute(&session->cmd_buff[0], session);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	return FTP_RET_OK;
}

static int fs_cmd_get(struct fs_info *session) { return FTP_RET_FAIL; }
static int fs_cmd_put(struct fs_info *session) { return FTP_RET_FAIL; }
static int fs_cmd_rm(struct fs_info *session) { return FTP_RET_FAIL; }

static int fs_cmd_help(struct fs_info *session) {
	size_t i;

	fprintf(stdout, "Use the next commands:\n");
	for (i = 0; i < sizeof ftp_mtds / sizeof ftp_mtds[0]; ++i) {
		fprintf(stdout, "\t%s -- %s\n", ftp_mtds[i].mtd_usage, ftp_mtds[i].mtd_description);
	}

	return FTP_RET_OK;
}

static int fs_cmd_bye(struct fs_info *session) {
	if (session->is_connected) {
		fs_cmd_close(session);
	}
	return FTP_RET_EXIT;
}

static int exec(int argc, char **argv) {
	int ret;
	size_t i;
	struct fs_info fsi;
	char old_value, *cmd_name, *first_space;

	fsi.is_connected = 0; /* initialize FTP Session Information structure */

	while (1) {
		fprintf(stdout, "ftp> ");

		cmd_name = fgets(&fsi.cmd_buff[0], sizeof fsi.cmd_buff, stdin);
		if (cmd_name == NULL) {
			fprintf(stdout, "%s: fatal error: fgets return null.\n", argv[0]);
			break;
		}

		/* Skip spaces */
		skip_spaces(cmd_name);

		/* Skip empty commands */
		if (*cmd_name == '\0') {
			continue;
		}

		/* Set '\0' to the end of command's name */
		first_space = cmd_name;
		skip_word(first_space);
		if (*first_space == '\0') {
			first_space = NULL;
		}
		else {
			old_value = *first_space;
			*first_space = '\0';
		}

		/* Try find this command */
		for (i = 0; i < sizeof ftp_mtds / sizeof ftp_mtds[0]; ++i) {
			if (strcmp(ftp_mtds[i].mtd_name, cmd_name) == 0) {
				break;
			}
		}
		if (i == sizeof ftp_mtds / sizeof ftp_mtds[0]) {
			fprintf(stderr, "%s: unknown command `%s'.\n", argv[0], cmd_name);
			continue;
		}

		/* Restore original string */
		if (first_space != NULL) {
			*first_space = old_value;
		}

		/* Remove spaces before command */
		memmove(&fsi.cmd_buff[0], cmd_name, strlen(cmd_name) + 1);

		/* Execute user command by its idx */
		ret = (*ftp_mtds[i].mtd_hnd)(&fsi);
		if (ret == FTP_RET_FAIL) {
			continue; /* nothing to do */
		}
		else if (ret == FTP_RET_ERROR) {
			fprintf(stderr, "%s: error occurred: code=%d, description=%s.\n", argv[0], errno, strerror(errno));
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

