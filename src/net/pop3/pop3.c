/**
 * @file
 * @brief
 *
 * @date 17.03.13
 * @author Ilia Vaprol
 */

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

int pop3_session_create(struct pop3_session *p3s) {
	int sock;

	if (p3s == NULL) {
		return -EINVAL;
	}

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1) {
		return -errno;
	}

	p3s->sock = sock;
	p3s->buff[0] = '\0';
	p3s->status = &p3s->buff[0];

	return 0;
}

int pop3_session_destroy(struct pop3_session *p3s) {
	if (p3s == NULL) {
		return -EINVAL;
	}

	return close(p3s->sock);
}

static int format_req(struct pop3_session *p3s, const char *fmt, va_list args) {
	return vsnprintf(&p3s->buff[0], ARRAY_SIZE(p3s->buff),
				fmt, args) < 0 ? -errno : 0;
}

static int send_req(struct pop3_session *p3s) {
	return -ENOSYS;
}

static int receive_rep(struct pop3_session *p3s) {
	return -ENOSYS;
}

static int parse_rep(struct pop3_session *p3s) {
	return -ENOSYS;
}

static int execute_cmd(struct pop3_session *p3s,
		const char *command_fmt, ...) {
	int ret;
	va_list command_args;

	if ((p3s == NULL) || (command_fmt == NULL)) {
		return -EINVAL;
	}

	va_start(command_args, command_fmt);
	ret = format_req(p3s, command_fmt, command_args);
	va_end(command_args);
	if (ret != 0) {
		return ret;
	}

	ret = send_req(p3s);
	if (ret != 0) {
		return ret;
	}

	ret = receive_rep(p3s);
	if (ret != 0) {
		return ret;
	}

	ret = parse_rep(p3s);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

int pop3_stat(struct pop3_session *p3s) {
	return execute_cmd(p3s, "STAT\r\n");
}

int pop3_list(struct pop3_session *p3s, int msg_or_none) {
	return msg_or_none == POP3_MSG_NONE
			? execute_cmd(p3s, "LIST\r\n")
			: execute_cmd(p3s, "LIST %d\r\n", msg_or_none);
}

int pop3_retr(struct pop3_session *p3s, int msg) {
	return execute_cmd(p3s, "RETR %d\r\n", msg);
}

int pop3_dele(struct pop3_session *p3s, int msg) {
	return execute_cmd(p3s, "DELE %d\r\n", msg);
}

int pop3_noop(struct pop3_session *p3s) {
	return execute_cmd(p3s, "NOOP\r\n");
}

int pop3_rset(struct pop3_session *p3s) {
	return execute_cmd(p3s, "RSET\r\n");
}

int pop3_quit(struct pop3_session *p3s) {
	return execute_cmd(p3s, "QUIT\r\n");
}

int pop3_top(struct pop3_session *p3s, int msg, unsigned int n) {
	return execute_cmd(p3s, "TOP %d %u\r\n", msg, n);
}

int pop3_uidl(struct pop3_session *p3s, int msg_or_none) {
	return msg_or_none == POP3_MSG_NONE
			? execute_cmd(p3s, "UIDL\r\n")
			: execute_cmd(p3s, "UIDL %d\r\n", msg_or_none);
}

int pop3_user(struct pop3_session *p3s, const char *name) {
	return execute_cmd(p3s, "USER %s\r\n", name);
}

int pop3_pass(struct pop3_session *p3s, const char *str) {
	return execute_cmd(p3s, "PASS %s\r\n", str);
}

int pop3_apop(struct pop3_session *p3s, const char *name, const char *salt) {
	return -ENOSYS;
}
