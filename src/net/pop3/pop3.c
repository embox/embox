/**
 * @file
 * @brief
 *
 * @date 17.03.13
 * @author Ilia Vaprol
 */

#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <net/pop3.h>
#include <stddef.h>
#include <util/array.h>
#include <stdio.h>
#include <netdb.h>

static int format_req(char *buff, size_t buff_sz, const char *fmt,
		va_list args, size_t *out_len) {
	int res;

	res = vsnprintf(buff, buff_sz, fmt, args);
	if (res < 0) {
		return -errno;
	}

	*out_len = res;
	return 0;
}

static int send_req(int sock, const char *req,
		size_t req_len) {
	int res;

	puts(req);

	do {
		res = send(sock, req, req_len, 0);
		if (res == -1) {
			return -errno;
		}

		req += res;
		req_len -= res;
	} while (req_len != 0);

	return 0;
}

static int receive_rep(int sock, char *buff, size_t size,
		int multiline, size_t *out_len) {
	int res;
	char *curr;

	curr = buff;
	size -= 1; /* for null-terminated string */

	do {
//		if (size == 0) {
//			return -ENOMEM;
//		}

		res = recv(sock, curr, size, 0);
		if (res == -1) {
			return -errno;
		}
		curr[res] = '\0';
		printf("%s", curr);

//		curr += res;
//		size -= res;

//		*curr = '\0';
	} while (strstr(buff, multiline ? "\r\n.\r\n" : "\r\n") == NULL);

	if (out_len != NULL) {
		*out_len = curr - buff;
	}
	return 0;
}

static int parse_rep(const char *rep, size_t rep_sz) {
	puts(rep);
	return 0;
}

static int execute_cmd(struct pop3_session *p3s, int multiline,
		const char *command_fmt, ...) {
	int ret;
	va_list command_args;
	size_t str_len;

	if ((p3s == NULL) || (command_fmt == NULL)) {
		return -EINVAL;
	}

	va_start(command_args, command_fmt);
	ret = format_req(&p3s->buff[0], ARRAY_SIZE(p3s->buff),
			command_fmt, command_args, &str_len);
	va_end(command_args);
	if (ret != 0) {
		return ret;
	}

	ret = send_req(p3s->sock, &p3s->buff[0], str_len);
	if (ret != 0) {
		return ret;
	}

	ret = receive_rep(p3s->sock, &p3s->buff[0],
			ARRAY_SIZE(p3s->buff), multiline, &str_len);
	if (ret != 0) {
		return ret;
	}

	ret = parse_rep(&p3s->buff[0], str_len);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

int pop3_session_create(struct pop3_session *p3s, const char *server,
		unsigned short int port) {
	int ret, sock;
	struct hostent *hostinfo;
	struct sockaddr_in addr;

	if (p3s == NULL) {
		return -EINVAL;
	}

	hostinfo = gethostbyname(server);
	if (hostinfo == NULL) {
		return -ESRCH;
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1) {
		return -errno;
	}

	memset(&addr, 0, sizeof addr);
	addr.sin_family = hostinfo->h_addrtype;
	addr.sin_port = htons(port);
	memcpy(&addr.sin_addr, hostinfo->h_addr_list[0], hostinfo->h_length);

	if (-1 == connect(sock, (struct sockaddr *)&addr, sizeof addr)) {
		close(sock);
		return -errno;
	}

	p3s->sock = sock;
	p3s->buff[0] = '\0';
	p3s->status = &p3s->buff[0];

	ret = receive_rep(sock, &p3s->buff[0], ARRAY_SIZE(p3s->buff),
			0, NULL);
	if (ret != 0) {
		close(sock);
		return ret;
	}

	ret = parse_rep(&p3s->buff[0], 0);
	if (ret != 0) {
		close(sock);
		return ret;
	}

	return 0;
}

int pop3_session_destroy(struct pop3_session *p3s) {
	if (p3s == NULL) {
		return -EINVAL;
	}

	return close(p3s->sock);
}

int pop3_stat(struct pop3_session *p3s) {
	return execute_cmd(p3s, 0, "STAT\r\n");
}

int pop3_list(struct pop3_session *p3s, int msg_or_none) {
	return msg_or_none == POP3_MSG_NONE
			? execute_cmd(p3s, 1, "LIST\r\n")
			: execute_cmd(p3s, 0, "LIST %d\r\n", msg_or_none);
}

int pop3_retr(struct pop3_session *p3s, int msg) {
	return execute_cmd(p3s, 1, "RETR %d\r\n", msg);
}

int pop3_dele(struct pop3_session *p3s, int msg) {
	return execute_cmd(p3s, 0, "DELE %d\r\n", msg);
}

int pop3_noop(struct pop3_session *p3s) {
	return execute_cmd(p3s, 0, "NOOP\r\n");
}

int pop3_rset(struct pop3_session *p3s) {
	return execute_cmd(p3s, 0, "RSET\r\n");
}

int pop3_quit(struct pop3_session *p3s) {
	return execute_cmd(p3s, 0, "QUIT\r\n");
}

int pop3_top(struct pop3_session *p3s, int msg, unsigned int n) {
	return execute_cmd(p3s, 1, "TOP %d %u\r\n", msg, n);
}

int pop3_uidl(struct pop3_session *p3s, int msg_or_none) {
	return msg_or_none == POP3_MSG_NONE
			? execute_cmd(p3s, 1, "UIDL\r\n")
			: execute_cmd(p3s, 0, "UIDL %d\r\n", msg_or_none);
}

int pop3_user(struct pop3_session *p3s, const char *name) {
	return execute_cmd(p3s, 0, "USER %s\r\n", name);
}

int pop3_pass(struct pop3_session *p3s, const char *str) {
	return execute_cmd(p3s, 0, "PASS %s\r\n", str);
}

int pop3_apop(struct pop3_session *p3s, const char *name,
		const char *str, const char *salt) {
	return -ENOSYS;
}
