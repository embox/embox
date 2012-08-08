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

/* Send command in the current session */
static int fs_snd_request(const char *cmd, struct fs_info *session) {
	int ret;

	ret = sendto(session->cmd_sock, cmd, strlen(cmd), 0, 0, 0);
	if (ret <= 0) {
		fprintf(stderr, "Can't send data\n");
		return FTP_RET_ERROR;
	}

	return FTP_RET_OK;
}

/* Receive reply in the current session */
static int fs_rcv_reply(struct fs_info *session) {
	int ret;

	ret = recvfrom(session->cmd_sock, &session->cmd_buff[0], sizeof session->cmd_buff - 1, 0, 0, 0);
	if (ret <= 0) {
		fprintf(stderr, "Can't receive data\n");
		return FTP_RET_ERROR;
	}

	session->cmd_buff[ret] = '\0';
	fprintf(stdout, "%s", &session->cmd_buff[0]);

	ret = sscanf(&session->cmd_buff[0], "%d", &session->stat_code);
	if (ret != 1) {
		return FTP_RET_ERROR;
	}

	return FTP_RET_OK;
}

/* Execute command in the current session (send request and receive response */
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

/* Create socket */
static int make_socket(int *out_sock) {
	int sock;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		fprintf(stderr, "Can't create socket.\n");
		return FTP_RET_ERROR;
	}

	*out_sock = sock;

	return FTP_RET_OK;
}

/* Create socket and connect to remote address */
static int make_active_socket(in_addr_t remote_ip, uint16_t remote_port, int *out_sock) {
	int ret, sock;
	struct sockaddr_in remote_addr;

	ret = make_socket(&sock);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	memset(&remote_addr, 0, sizeof remote_addr);

	remote_addr.sin_addr.s_addr = remote_ip;
	remote_addr.sin_port = htons(remote_port);
	remote_addr.sin_family = AF_INET;

	ret = connect(sock, (struct sockaddr *)&remote_addr, sizeof remote_addr);
	if (ret < 0) {
		fprintf(stderr, "Can't connect to host %s:%d.\n", inet_ntoa(remote_addr.sin_addr), (int)remote_port);
		close(sock);
		return FTP_RET_ERROR;
	}

	*out_sock = sock;

	return FTP_RET_OK;
}

/* Create socket for data transfering */
static int make_data_socket(struct fs_info *session, int *out_sock) {
	/* FIXME only PASV command is supported */
	int ret, sock, ip_part[4], port_part[2];
	char *tmp;
	in_addr_t ip_full;
	uint16_t port_full;

	ret = fs_execute("PASV\r\n", session); /* XXX only passive connection now */
	if (ret != FTP_RET_OK) {
		return ret;
	}

	/* Parse destonation address */
	if (!(tmp = strchr(&session->cmd_buff[0], '(')) || (sscanf(++tmp, "%d", &ip_part[0]) != 1)
			|| !(tmp = strchr(tmp, ',')) || (sscanf(++tmp, "%d", &ip_part[1]) != 1)
			|| !(tmp = strchr(tmp, ',')) || (sscanf(++tmp, "%d", &ip_part[2]) != 1)
			|| !(tmp = strchr(tmp, ',')) || (sscanf(++tmp, "%d", &ip_part[3]) != 1)
			|| !(tmp = strchr(tmp, ',')) || (sscanf(++tmp, "%d", &port_part[0]) != 1)
			|| !(tmp = strchr(tmp, ',')) || (sscanf(++tmp, "%d", &port_part[1]) != 1)) {
		fprintf(stderr, "Can't parse remote address.\n");
		return FTP_RET_FAIL;
	}

	ip_full = (ip_part[3] << 24) | (ip_part[2] << 16) | (ip_part[1] << 8) | (ip_part[0] << 0);
	port_full = (port_part[0] << 8) | (port_part[1] << 0);

	ret = make_active_socket(ip_full, port_full, &sock);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	*out_sock = sock;

	return FTP_RET_OK;
}

static int save_all_data(FILE *file_to, int sock_from, char *buff, size_t buff_sz) {
	int received, written;

	while (1) {
		received = recvfrom(sock_from, buff, buff_sz, 0, 0, 0);
		if (received < 0) {
			fprintf(stderr, "Can't receive data.\n");
			return FTP_RET_ERROR;
		}

		if (received == 0) {
			break;
		}

		written = fwrite(buff, 1, received, file_to);
		if (written != received) {
			fprintf(stderr, "Can't save data to 0x%p.\n", (void *)file_to);
			return FTP_RET_ERROR;
		}
	}

	return FTP_RET_OK;
}


/**
 * Realization of ftp client's commands
 */

static int fs_cmd_open(struct fs_info *session) {
	int ret;
	char *tmp, *arg_hostname, *arg_port;
	struct in_addr remote_ip;
	unsigned int remote_port;

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

	if (!inet_aton(arg_hostname, &remote_ip)) {
		fprintf(stderr, "Can't parse IP address.\n");
		errno = EINVAL;
		return FTP_RET_ERROR;
	}
	if (arg_port == NULL) {
		remote_port = 21; /* Use default settings */
	}
	else {
		if (sscanf(arg_port, "%u", &remote_port) != 1) {
			fprintf(stderr, "Can't parse port '%s`.\n", arg_port);
			errno = EINVAL;
			return FTP_RET_ERROR;
		}
	}

	ret = make_active_socket(remote_ip.s_addr, (uint16_t)remote_port, &session->cmd_sock);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	fprintf(stdout, "Connected to %s:%u.\n", arg_hostname, remote_port);
	session->is_connected = 1;

	ret = fs_rcv_reply(session);
	if (ret != FTP_RET_OK) {
		return ret;
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
	int ret, data_sock;
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

	ret = make_data_socket(session, &data_sock);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	sprintf(&session->cmd_buff[0],
			(arg_remotedir == NULL) ? "LIST\r\n" : "LIST %s\r\n",
			arg_remotedir);

	ret = fs_execute(&session->cmd_buff[0], session);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	ret = save_all_data(stdout, data_sock, &session->cmd_buff[0], sizeof session->cmd_buff);
	if (ret != FTP_RET_OK) {
		close(data_sock);
		return ret;
	}

	close(data_sock);

	ret = fs_rcv_reply(session);
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

