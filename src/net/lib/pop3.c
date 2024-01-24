/**
 * @file
 * @brief
 *
 * @date 17.03.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <net/lib/pop3.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <lib/libds/array.h>

#define BUFF_SZ 1024
static char buff[BUFF_SZ];

static int send_req(struct pop3_session *p3s,
		const char *command_fmt, va_list command_args) {
	int res;
	char *req;
	size_t req_len;

	assert(p3s != NULL);
	assert(command_fmt != NULL);

	res = vsnprintf(&buff[0], ARRAY_SIZE(buff), command_fmt,
			command_args);
	if (res < 0) {
		return -errno;
	}

	req = &buff[0];
	req_len = res;

#if 0
	printf("pop3> %s", req);
#endif

	do {
		res = send(p3s->sock, req, req_len, 0);
		if (res == -1) {
			return -errno;
		}

		req += res;
		req_len -= res;
	} while (req_len != 0);

	return 0;
}

static int recv_rep(struct pop3_session *p3s, int multiline) {
	int res;
	char *rep, *end;
	size_t rep_len;

	assert(p3s != NULL);

	/* get status */
	rep = &p3s->status[0];
	rep_len = ARRAY_SIZE(p3s->status) - 1;

	do {
		res = recv(p3s->sock, rep, rep_len, 0);
		if (res == -1) {
			return -errno;
		}

		rep += res;
		rep_len -= res;

		*rep = '\0';
		end = strstr(&p3s->status[0], "\r\n");
	} while ((end == NULL) && (rep_len != 0));

	if ((rep_len == 0) && (end == NULL)) {
		p3s->ok = 0;
		snprintf(&p3s->status[0], ARRAY_SIZE(p3s->status), "%s",
				"recv_rep: status buffer is full");
		return 0;
	}

	assert(end != NULL);
	*end = '\0';

	if (0 == strncmp("+OK ", &p3s->status[0], strlen("+OK "))) {
		p3s->ok = 1;
	}
	else if (0 == strncmp("-ERR ", &p3s->status[0], strlen("-ERR "))) {
		p3s->ok = 0;
		return 0;
	}
	else {
		p3s->ok = 0;
		snprintf(&p3s->status[0], ARRAY_SIZE(p3s->status), "%s",
				"recv_rep: illegal status code");
		return 0;
	}

	/* get data */
	if (!multiline) {
		return 0;
	}

	rep = &buff[0];
	rep_len = ARRAY_SIZE(buff) - 1;

	end += 2;
	strcpy(rep, end);
	rep += strlen(end);
	rep_len -= strlen(end);

	do {
		res = recv(p3s->sock, rep, rep_len, 0);
		if (res == -1) {
			return -errno;
		}

		rep += res;
		rep_len -= res;

		*rep = '\0';
		end = strstr(&buff[0], "\r\n.\r\n");

		if ((rep_len == 0) || (end != NULL)) {
			fprintf(stdin, "%s", &buff[0]);
			rep = &buff[0];
			rep_len = ARRAY_SIZE(buff) - 1;
		}
	} while (end == NULL);

	return 0;
}

static int execute_cmd(struct pop3_session *p3s, int multiline,
		const char *command_fmt, ...) {
	int ret;
	va_list command_args;

	assert(p3s != NULL);
	assert(command_fmt != NULL);

	va_start(command_args, command_fmt);
	ret = send_req(p3s, command_fmt, command_args);
	va_end(command_args);
	if (ret != 0) {
		return ret;
	}

	ret = recv_rep(p3s, multiline);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

int pop3_open(struct pop3_session *p3s, const char *host,
		unsigned short port) {
	int ret, sock;
	struct hostent *host_info;
	struct sockaddr_in addr;

	if ((p3s == NULL) || (host == NULL)) {
		return -EINVAL;
	}

	host_info = gethostbyname(host);
	if (host_info == NULL) {
		return -ESRCH;
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1) {
		return -errno;
	}

	memset(&addr, 0, sizeof addr);
	addr.sin_family = host_info->h_addrtype;
	addr.sin_port = htons(port);
	memcpy(&addr.sin_addr, host_info->h_addr_list[0],
			host_info->h_length);

	if (-1 == connect(sock, (struct sockaddr *)&addr, sizeof addr)) {
		close(sock);
		return -errno;
	}

	p3s->sock = sock;
	p3s->ok = 0;
	p3s->status[0] = '\0';
	p3s->data = NULL;

	ret = recv_rep(p3s, 0);
	if (ret != 0) {
		close(sock);
		return ret;
	}

	return 0;
}

int pop3_close(struct pop3_session *p3s) {
	if (p3s == NULL) {
		return -EINVAL;
	}

	close(p3s->sock);

	return 0;
}

int pop3_ok(struct pop3_session *p3s) {
	if (p3s == NULL) {
		return -EINVAL;
	}

	return p3s->ok;
}

const char * pop3_status(struct pop3_session *p3s) {
	if (p3s == NULL) {
		return NULL;
	}

	return p3s->status[0] != '\0' ? &p3s->status[0] : NULL;
}

const char * pop3_data(struct pop3_session *p3s) {
	if (p3s == NULL) {
		return NULL;
	}

	return p3s->data;
}

int pop3_stat(struct pop3_session *p3s) {
	if (p3s == NULL) {
		return -EINVAL;
	}

	return execute_cmd(p3s, 0, "STAT\r\n");
}

int pop3_list(struct pop3_session *p3s, int msg_or_any) {
	if (p3s == NULL) {
		return -EINVAL;
	}

	return msg_or_any == POP3_MSG_ANY
			? execute_cmd(p3s, 1, "LIST\r\n")
			: execute_cmd(p3s, 0, "LIST %d\r\n", msg_or_any);
}

int pop3_retr(struct pop3_session *p3s, int msg) {
	if (p3s == NULL) {
		return -EINVAL;
	}

	return execute_cmd(p3s, 1, "RETR %d\r\n", msg);
}

int pop3_dele(struct pop3_session *p3s, int msg) {
	if (p3s == NULL) {
		return -EINVAL;
	}

	return execute_cmd(p3s, 0, "DELE %d\r\n", msg);
}

int pop3_noop(struct pop3_session *p3s) {
	if (p3s == NULL) {
		return -EINVAL;
	}

	return execute_cmd(p3s, 0, "NOOP\r\n");
}

int pop3_rset(struct pop3_session *p3s) {
	if (p3s == NULL) {
		return -EINVAL;
	}

	return execute_cmd(p3s, 0, "RSET\r\n");
}

int pop3_quit(struct pop3_session *p3s) {
	if (p3s == NULL) {
		return -EINVAL;
	}

	return execute_cmd(p3s, 0, "QUIT\r\n");
}

int pop3_top(struct pop3_session *p3s, int msg, unsigned int n) {
	if (p3s == NULL) {
		return -EINVAL;
	}

	return execute_cmd(p3s, 1, "TOP %d %u\r\n", msg, n);
}

int pop3_uidl(struct pop3_session *p3s, int msg_or_any) {
	if (p3s == NULL) {
		return -EINVAL;
	}

	return msg_or_any == POP3_MSG_ANY
			? execute_cmd(p3s, 1, "UIDL\r\n")
			: execute_cmd(p3s, 0, "UIDL %d\r\n", msg_or_any);
}

int pop3_user(struct pop3_session *p3s, const char *name) {
	if ((p3s == NULL) || (name == NULL)) {
		return -EINVAL;
	}

	return execute_cmd(p3s, 0, "USER %s\r\n", name);
}

int pop3_pass(struct pop3_session *p3s, const char *secret) {
	if ((p3s == NULL) || (secret == NULL)) {
		return -EINVAL;
	}

	return execute_cmd(p3s, 0, "PASS %s\r\n", secret);
}

int pop3_apop(struct pop3_session *p3s, const char *name,
		const char *secret, const char *salt) {
	if ((p3s == NULL) || (name == NULL) || (secret == NULL)
			|| (salt == NULL)) {
		return -EINVAL;
	}

	return -ENOSYS;
}
