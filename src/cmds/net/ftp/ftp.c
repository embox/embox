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
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define FTP_PORT 21

#if defined (__EMBOX__)

#include <framework/mod/options.h>

#define MODOPS_CMD_BUFF_SZ  OPTION_GET(NUMBER, cmd_buff_sz)
#define MODOPS_DATA_BUFF_SZ OPTION_GET(NUMBER, data_buff_sz)

#else

#define MODOPS_CMD_BUFF_SZ  128
#define MODOPS_DATA_BUFF_SZ 1024

#endif /* defined (__EMBOX__) */

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
#define FTP_STAT_FINI(stat) (stat % 10)

/* Useful commands */
#define skip_spaces(ptr) \
				while (*ptr && isspace(*ptr)) { ptr++; }

#define skip_word(ptr)  \
				while (*ptr && !isspace(*ptr)) { ptr++; }

static inline void split_word(char *ptr) {
	 skip_word(ptr); *ptr = '\0';
}

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
	char buff[MODOPS_DATA_BUFF_SZ];    /* general buffer for commands and data transfering */
	char cmd_buff[MODOPS_CMD_BUFF_SZ]; /* command buffer */
	int cmd_sock;                      /* command socket */
	int is_connected;                  /* connection is esteblishment */
	int stat_code;                     /* result of last command */
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
static const struct fm_info ftp_mtds[] = {
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
static int fs_snd_request(struct fs_info *session, const char *cmd) {
	int ret;

	ret = send(session->cmd_sock, cmd, strlen(cmd), 0);
	if (ret <= 0) {
		fprintf(stderr, "Can't send data\n");
		return FTP_RET_ERROR;
	}

	return FTP_RET_OK;
}

/* Check status code and make needed actions */
static int fs_check_stat(struct fs_info *session) {
	int stat;

	stat = session->stat_code;

	switch (FTP_STAT_TYPE(stat)) {
	default:
		break;
	case FTP_STAT_TYPE_TRANSIENT_NEGATIVE_COMPLETION:
		switch (FTP_STAT_KIND(stat)) {
		default:
			break;
		case FTP_STAT_KIND_CONNECTIONS:
			switch (FTP_STAT_FINI(stat)) {
			case 1:
				close(session->cmd_sock);
				session->is_connected = 0;
				break;
			}
			break;

		}
		return FTP_RET_FAIL;
	case FTP_STAT_TYPE_PERMANENT_NEGATIVE_COMPLETION:
		return FTP_RET_FAIL;
	}

	return FTP_RET_OK;
}

/* Receive reply in the current session */
static int fs_rcv_reply(struct fs_info *session, char *buff, size_t buff_sz) {
	int ret;
	char *status;

	ret = recv(session->cmd_sock, buff, buff_sz - 1, 0);
	if (ret <= 0) {
		fprintf(stderr, "Can't receive data\n");
		return FTP_RET_ERROR;
	}
	*(buff + ret) = '\0';

	fwrite(buff, 1, ret, stdout);

	status = buff;
	do {
		if (isdigit(*status)) {
			ret = sscanf(status, "%d", &session->stat_code);
			if (ret != 1) {
				return FTP_RET_ERROR;
			}

			ret = fs_check_stat(session);
			if (ret != FTP_RET_OK) {
				return ret;
			}

			status = strstr(status, "\r\n");
		}

		status = strstr(status, "\r\n");
		if (status == NULL) {
			break;
		}
		status += 2;
	} while (*status != '\0');

	return FTP_RET_OK;
}

/* Execute command in the current session (send request and receive response */
static int fs_execute(struct fs_info *session, const char *cmd_format, ...) {
	int ret;
	va_list args;

	va_start(args, cmd_format);
	vsnprintf(&session->buff[0], sizeof(session->buff), cmd_format, args);
	va_end(args);

	ret = fs_snd_request(session, &session->buff[0]);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	ret = fs_rcv_reply(session, &session->buff[0], sizeof(session->buff));
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

	memset(&remote_addr, 0, sizeof(remote_addr));

	remote_addr.sin_addr.s_addr = remote_ip;
	remote_addr.sin_port = htons(remote_port);
	remote_addr.sin_family = AF_INET;

	ret = connect(sock, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
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

	ret = fs_execute(session, "PASV\r\n"); /* XXX only passive connection now */
	if (ret != FTP_RET_OK) {
		return ret;
	}

	/* Parse destonation address */
	if (!(tmp = strchr(&session->buff[0], '(')) || (sscanf(++tmp, "%d", &ip_part[0]) != 1)
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

/* Receive all from socket and write to file using buff */
static int fill_file_from_socket(FILE *file, int sock, char *buff, size_t buff_sz) {
	int received, written;

	while (1) {
		received = recv(sock, buff, buff_sz, 0);
		if (received < 0) {
			fprintf(stderr, "Can't receive data.\n");
			return FTP_RET_ERROR;
		}

		if (received == 0) {
			break;
		}

		written = fwrite(buff, 1, received, file);
		if (written != received) {
			fprintf(stderr, "Can't save data.\n");
			return FTP_RET_ERROR;
		}
	}

	return FTP_RET_OK;
}

/* Read all from file and send to remote side via sock using buff */
static int flush_file_to_socket(FILE *file, int sock, char *buff, size_t buff_sz) {
	int readed, posted;

	while (1) {
		readed = fread(buff, 1, buff_sz, file);
		if (ferror(file) || readed < 0) {
			fprintf(stderr, "Can't read data from file.\n");
			return FTP_RET_ERROR;
		}

		if (readed == 0) {
			break;
		}

		posted = send(sock, buff, readed, 0);
		if (posted != readed) {
			fprintf(stderr, "Can't send data.\n");
			return FTP_RET_ERROR;
		}
	}

	return FTP_RET_OK;
}


/**
 * Realization of ftp client's commands
 */

static int fs_cmd_open(struct fs_info *session) {
	/* Usage: open <host-name> [port] */
	int ret;
	char *tmp, *arg_hostname, *arg_port;
	struct in_addr remote_ip;
	unsigned int remote_port;

	if (session->is_connected) {
		fprintf(stderr, "Already connected, use close first.\n");
		return FTP_RET_FAIL;
	}

	/* Get args */
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
		remote_port = FTP_PORT; /* Use default settings */
	}
	else {
		if (sscanf(arg_port, "%u", &remote_port) != 1) {
			fprintf(stderr, "Can't parse port '%s`.\n", arg_port);
			errno = EINVAL;
			return FTP_RET_ERROR;
		}
	}

	ret = make_active_socket(remote_ip.s_addr, (uint16_t)remote_port,
									&session->cmd_sock);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	fprintf(stdout, "Connected to %s:%u.\n", arg_hostname, remote_port);
	session->is_connected = 1;

	ret = fs_rcv_reply(session, &session->buff[0], sizeof(session->buff));
	if (ret != FTP_RET_OK) {
		return ret;
	}

	return FTP_RET_OK;
}

static int fs_cmd_close(struct fs_info *session) {
	/* Usage: close */
	if (!session->is_connected) {
		fprintf(stderr, "Not connected.\n");
		return FTP_RET_FAIL;
	}

	fs_execute(session, "QUIT\r\n");
	close(session->cmd_sock);
	session->is_connected = 0;

	return FTP_RET_OK;
}

static int fs_cmd_user(struct fs_info *session) {
	/* Usage: user <user-name> */
	int ret;
	char *tmp, *arg_username, *password;

	if (!session->is_connected) {
		fprintf(stderr, "Not connected.\n");
		return FTP_RET_FAIL;
	}

	/* Get args */
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

	ret = fs_execute(session, "USER %s\r\n", arg_username);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	password = getpass("Password: ");
	if (password == NULL) {
		fprintf(stderr, "Cant get password for %s.\n", arg_username);
		errno = EINVAL;
		return FTP_RET_ERROR;
	}

	ret = fs_execute(session, "PASS %s\r\n", password);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	return FTP_RET_OK;
}

static int fs_cmd_cd(struct fs_info *session) {
	/* Usage: cd <remote-directory> */
	int ret;
	char *tmp, *arg_remotedir;

	if (!session->is_connected) {
		fprintf(stderr, "Not connected.\n");
		return FTP_RET_FAIL;
	}

	/* Get args */
	tmp = &session->cmd_buff[0];
	skip_spaces(tmp); skip_word(tmp); skip_spaces(tmp);
	arg_remotedir = tmp;

	split_word(arg_remotedir);

	if (*arg_remotedir == '\0') {
		fprintf(stderr, "Remote directory is not specified.\n");
		errno = EINVAL;
		return FTP_RET_ERROR;
	}
	/* --- END OF Get args --- */

	ret = fs_execute(session, "CWD %s\r\n", arg_remotedir);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	return FTP_RET_OK;
}

static int fs_cmd_pwd(struct fs_info *session) {
	/* Usage: pwd */
	int ret;

	if (!session->is_connected) {
		fprintf(stderr, "Not connected.\n");
		return FTP_RET_FAIL;
	}

	ret = fs_execute(session, "PWD\r\n");
	if (ret != FTP_RET_OK) {
		return ret;
	}

	return FTP_RET_OK;
}

static int fs_cmd_mkdir(struct fs_info *session) {
	/* Usage: mkdir <directory-name> */
	int ret;
	char *tmp, *arg_dirname;

	if (!session->is_connected) {
		fprintf(stderr, "Not connected.\n");
		return FTP_RET_FAIL;
	}

	/* Get args */
	tmp = &session->cmd_buff[0];
	skip_spaces(tmp); skip_word(tmp); skip_spaces(tmp);
	arg_dirname = tmp;

	split_word(arg_dirname);

	if (*arg_dirname == '\0') {
		fprintf(stderr, "Directory name is not specified.\n");
		errno = EINVAL;
		return FTP_RET_ERROR;
	}
	/* --- END OF Get args --- */

	ret = fs_execute(session, "MKD %s\r\n", arg_dirname);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	return FTP_RET_OK;
}

static int fs_cmd_rmdir(struct fs_info *session) {
	/* Usage: rmdir <directory-name> */
	int ret;
	char *tmp, *arg_dirname;

	if (!session->is_connected) {
		fprintf(stderr, "Not connected.\n");
		return FTP_RET_FAIL;
	}

	/* Get args */
	tmp = &session->cmd_buff[0];
	skip_spaces(tmp); skip_word(tmp); skip_spaces(tmp);
	arg_dirname = tmp;

	split_word(arg_dirname);

	if (*arg_dirname == '\0') {
		fprintf(stderr, "Directory name is not specified.\n");
		errno = EINVAL;
		return FTP_RET_ERROR;
	}
	/* --- END OF Get args --- */

	ret = fs_execute(session, "RMD %s\r\n", arg_dirname);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	return FTP_RET_OK;
}

static int fs_cmd_mv(struct fs_info *session) {
	/* Usage: mv <from-name> <to-name> */
	int ret;
	char *tmp, *arg_fromname, *arg_toname;

	if (!session->is_connected) {
		fprintf(stderr, "Not connected.\n");
		return FTP_RET_FAIL;
	}

	/* Get args */
	tmp = &session->cmd_buff[0];
	skip_spaces(tmp); skip_word(tmp); skip_spaces(tmp);
	arg_fromname = tmp;
	skip_word(tmp); skip_spaces(tmp);
	arg_toname = tmp;

	split_word(arg_fromname);
	split_word(arg_toname);

	if ((*arg_fromname == '\0') || (*arg_toname == '\0')) {
		fprintf(stderr, "Please specify source and destonation names.\n");
		errno = EINVAL;
		return FTP_RET_ERROR;
	}
	/* --- END OF Get args --- */

	fprintf(stdout, "from: '%s`, to: '%s`\n", arg_fromname, arg_toname);

	ret = fs_execute(session, "RNFR %s\r\n", arg_fromname);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	ret = fs_execute(session, "RNTO %s\r\n", arg_toname);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	return FTP_RET_OK;
}

static int fs_cmd_ls(struct fs_info *session) {
	/* Usage: ls [remote-directory] */
	int ret, data_sock;
	char *tmp, *arg_remotedir;

	if (!session->is_connected) {
		fprintf(stderr, "Not connected.\n");
		return FTP_RET_FAIL;
	}

	/* Get args */
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

	ret = (arg_remotedir == NULL)
		? fs_execute(session, "LIST\r\n")
		: fs_execute(session, "LIST %s\r\n", arg_remotedir);
	if (ret != FTP_RET_OK) {
		close(data_sock);
		return ret;
	}

	ret = fill_file_from_socket(stdout, data_sock, &session->buff[0],
										sizeof(session->buff));
	if (ret != FTP_RET_OK) {
		close(data_sock);
		return ret;
	}

	close(data_sock);

	if (FTP_STAT_TYPE_POSITIVE_PRELIMINARY
			== FTP_STAT_TYPE(session->stat_code)) {
		ret = fs_rcv_reply(session, &session->buff[0], sizeof(session->buff));
		if (ret != FTP_RET_OK) {
			return ret;
		}
	}

	return FTP_RET_OK;
}

static int fs_cmd_get(struct fs_info *session) {
	/* Usage: get <remote-file> [local-file] */
	int ret, data_sock;
	char *tmp, *arg_remotefile, *arg_localfile;
	FILE *file;

	if (!session->is_connected) {
		fprintf(stderr, "Not connected.\n");
		return FTP_RET_FAIL;
	}

	/* Get args */
	tmp = &session->cmd_buff[0];
	skip_spaces(tmp); skip_word(tmp); skip_spaces(tmp);
	arg_remotefile = tmp;
	skip_word(tmp); skip_spaces(tmp);
	arg_localfile = tmp;

	split_word(arg_remotefile);
	split_word(arg_localfile);

	if (*arg_remotefile == '\0') {
		fprintf(stderr, "Remote file name is not specified.\n");
		errno = EINVAL;
		return FTP_RET_ERROR;
	}
	if (*arg_localfile == '\0') {
		arg_localfile = strchr(arg_remotefile, '/');
		if (arg_localfile != NULL) {
			arg_localfile += 1; /* skip '/' character */
		}
		else {
			arg_localfile = arg_remotefile;
		}
	}
	/* --- END OF Get args --- */

	fprintf(stdout, "remote: '%s`, local: '%s`\n", arg_remotefile, arg_localfile);

	file = fopen(arg_localfile, "w");
	if (file == NULL) {
		fprintf(stderr, "Can't open file '%s` for writing.\n", arg_localfile);
		return FTP_RET_ERROR;
	}

	ret = make_data_socket(session, &data_sock);
	if (ret != FTP_RET_OK) {
		fclose(file);
		return ret;
	}

	ret = fs_execute(session, "RETR %s\r\n", arg_remotefile);
	if (ret != FTP_RET_OK) {
		fclose(file);
		close(data_sock);
		return ret;
	}

	ret = fill_file_from_socket(file, data_sock, &session->buff[0],
									sizeof(session->buff));
	if (ret != FTP_RET_OK) {
		fclose(file);
		close(data_sock);
		return ret;
	}

	fclose(file);
	close(data_sock);

	ret = fs_rcv_reply(session, &session->buff[0], sizeof(session->buff));
	if (ret != FTP_RET_OK) {
		return ret;
	}

	return FTP_RET_OK;
}

static int fs_cmd_put(struct fs_info *session) {
	/* Usage: put <local-file> <remote-file> */
	int ret, data_sock;
	char *tmp, *arg_localfile, *arg_remotefile;
	FILE *file;

	if (!session->is_connected) {
		fprintf(stderr, "Not connected.\n");
		return FTP_RET_FAIL;
	}

	/* Get args */
	tmp = &session->cmd_buff[0];
	skip_spaces(tmp); skip_word(tmp); skip_spaces(tmp);
	arg_localfile = tmp;
	skip_word(tmp); skip_spaces(tmp);
	arg_remotefile = tmp;

	split_word(arg_localfile);
	split_word(arg_remotefile);

	if ((*arg_localfile == '\0') && (*arg_remotefile == '\0')) {
		fprintf(stderr, "Please specify names of local and remote files.\n");
		errno = EINVAL;
		return FTP_RET_ERROR;
	}
	/* --- END OF Get args --- */

	fprintf(stdout, "local: '%s`, remote: '%s`\n", arg_localfile, arg_remotefile);

	file = fopen(arg_localfile, "r");
	if (file == NULL) {
		fprintf(stderr, "Can't open file '%s` for reading.\n", arg_localfile);
		return FTP_RET_ERROR;
	}

	ret = make_data_socket(session, &data_sock);
	if (ret != FTP_RET_OK) {
		fclose(file);
		return ret;
	}

	ret = fs_execute(session, "STOR %s\r\n", arg_remotefile);
	if (ret != FTP_RET_OK) {
		fclose(file);
		close(data_sock);
		return ret;
	}

	ret = flush_file_to_socket(file, data_sock, &session->buff[0],
									sizeof(session->buff));
	if (ret != FTP_RET_OK) {
		fclose(file);
		close(data_sock);
		return ret;
	}

	fclose(file);
	close(data_sock);

	ret = fs_rcv_reply(session, &session->buff[0], sizeof(session->buff));
	if (ret != FTP_RET_OK) {
		return ret;
	}

	return FTP_RET_OK;
}

static int fs_cmd_rm(struct fs_info *session) {
	/* Usage: rm <remote-file> */
	int ret;
	char *tmp, *arg_remotefile;

	if (!session->is_connected) {
		fprintf(stderr, "Not connected.\n");
		return FTP_RET_FAIL;
	}

	/* Get args */
	tmp = &session->cmd_buff[0];
	skip_spaces(tmp); skip_word(tmp); skip_spaces(tmp);
	arg_remotefile = tmp;

	split_word(arg_remotefile);

	if (*arg_remotefile == '\0') {
		fprintf(stderr, "File name is not specified.\n");
		errno = EINVAL;
		return FTP_RET_ERROR;
	}
	/* --- END OF Get args --- */

	ret = fs_execute(session, "DELE %s\r\n", arg_remotefile);
	if (ret != FTP_RET_OK) {
		return ret;
	}

	return FTP_RET_OK;
}

static int fs_cmd_help(struct fs_info *session) {
	/* Usage: help */
	size_t i;

	fprintf(stdout, "Use the next commands:\n");
	for (i = 0; i < (sizeof(ftp_mtds) / sizeof(ftp_mtds[0])); ++i) {
		fprintf(stdout, "\t%s -- %s\n", ftp_mtds[i].mtd_usage, ftp_mtds[i].mtd_description);
	}

	return FTP_RET_OK;
}

static int fs_cmd_bye(struct fs_info *session) {
	/* Usage: bye */
	if (session->is_connected) {
		fs_cmd_close(session);
	}
	return FTP_RET_EXIT;
}

int main(int argc, char **argv) {
	int ret;
	size_t i;
	struct fs_info fsi;
	char old_value, *cmd_name, *first_space;

	fsi.is_connected = 0; /* initialize FTP Session Information structure */

	if (argc > 3) {
		return -EINVAL;
	}
	else if (argc > 1) {
		if (strcmp(argv[1], "-h") == 0) {
			printf("Usage: %s [address [port]]\n", argv[0]);
			return 0;
		}
		cmd_name = &fsi.cmd_buff[0];
		snprintf(&fsi.cmd_buff[0], sizeof(fsi.cmd_buff), "open %s %s",
				argv[1], argc == 3 ? argv[2] : "");
		goto parse_cmd;
	}

	while (1) {
		fprintf(stdout, "%s> ", argv[0]);

		cmd_name = fgets(&fsi.cmd_buff[0], sizeof(fsi.cmd_buff), stdin);
		if (cmd_name == NULL) {
			fprintf(stderr, "%s: fatal error: fgets return null.\n", argv[0]);
			break;
		}

parse_cmd:
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
		for (i = 0; i < (sizeof(ftp_mtds) / sizeof(ftp_mtds[0])); ++i) {
			if (strcmp(ftp_mtds[i].mtd_name, cmd_name) == 0) {
				break;
			}
		}
		if (i == (sizeof(ftp_mtds) / sizeof(ftp_mtds[0]))) {
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
