/**
 * @file
 * @brief
 *
 * @date 22.03.13
 * @author Ilia Vaprol
 */

#include <alloca.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <lib/libds/array.h>
#include <lib/crypt/b64.h>

#include <net/lib/smtp.h>

static char buff[SMTP_COMMAND_LEN + 1];

static int send_data(struct smtp_session *ss,
		const char *data, size_t data_len) {
	int res;

	assert(ss != NULL);
	assert(data != NULL);

	do {
		res = send(ss->sock, data, data_len, 0);
		if (res == -1) {
			return -errno;
		}

		data += res;
		data_len -= res;
	} while (data_len != 0);

	return 0;
}

static int send_req(struct smtp_session *ss,
		const char *command_fmt, va_list command_args) {
	int res;
	char *req;
	size_t req_len;

	assert(ss != NULL);
	assert(command_fmt != NULL);

	res = vsnprintf(&buff[0], ARRAY_SIZE(buff), command_fmt,
			command_args);
	if (res < 0) {
		return -errno;
	}

	req = &buff[0];
	req_len = res;

#if 0
	printf("smtp> %s", req);
#endif

	return send_data(ss, req, req_len);
}

static int recv_rep(struct smtp_session *ss, int multiline) {
	int res;
	char *rep, *end;
	size_t rep_len;

	assert(ss != NULL);

	/* get status */
	rep = &ss->status[0];
	rep_len = ARRAY_SIZE(ss->status) - 1;

	do {
		res = recv(ss->sock, rep, rep_len, 0);
		if (res == -1) {
			return -errno;
		}

		rep += res;
		rep_len -= res;

		*rep = '\0';
		end = strstr(&ss->status[0], "\r\n");
	} while ((end == NULL) && (rep_len != 0));

	if ((rep_len == 0) && (end == NULL)) {
		ss->code = 0;
		snprintf(&ss->status[0], ARRAY_SIZE(ss->status), "%s",
				"recv_rep: status buffer is full");
		return 0;
	}

	assert(end != NULL);
	*end = '\0';

	if (1 != sscanf(&ss->status[0], "%d", &ss->code)) {
		ss->code = 0;
		snprintf(&ss->status[0], ARRAY_SIZE(ss->status), "%s",
				"recv_rep: illegal status code");
		return 0;
	}
	else if (!smtp_ok(ss)) {
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
		res = recv(ss->sock, rep, rep_len, 0);
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

static int execute_cmd(struct smtp_session *ss, int multiline,
		const char *command_fmt, ...) {
	int ret;
	va_list command_args;

	assert(ss != NULL);
	assert(command_fmt != NULL);

	va_start(command_args, command_fmt);
	ret = send_req(ss, command_fmt, command_args);
	va_end(command_args);
	if (ret != 0) {
		return ret;
	}

	ret = recv_rep(ss, multiline);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

int smtp_open(struct smtp_session *ss, const char *host,
		unsigned short port) {
	int ret, sock;
	struct hostent *host_info;
	struct sockaddr_in addr;

	if ((ss == NULL) || (host == NULL)) {
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

	ss->sock = sock;
	ss->code = 0;
	ss->status[0] = '\0';

	ret = recv_rep(ss, 0);
	if (ret != 0) {
		close(sock);
		return ret;
	}

	return 0;
}

int smtp_close(struct smtp_session *ss) {
	if (ss == NULL) {
		return -EINVAL;
	}

	close(ss->sock);

	return 0;
}

int smtp_ok(struct smtp_session *ss) {
	return ss != NULL ? (ss->code >= 100) && (ss->code < 400) : 0;
}

int smtp_code(struct smtp_session *ss) {
	return ss != NULL ? ss->code : 0;
}

const char * smtp_status(struct smtp_session *ss) {
	if (ss == NULL) {
		return NULL;
	}

	return ss->status[0] != '\0' ? &ss->status[0] : NULL;
}

int smtp_mail_from(struct smtp_session *ss, const char *addr) {
	if ((ss == NULL) || (addr == NULL)) {
		return -EINVAL;
	}

	return execute_cmd(ss, 0, "MAIL FROM:%s\r\n", addr);
}

int smtp_rcpt_to(struct smtp_session *ss, const char *addr) {
	if ((ss == NULL) || (addr == NULL)) {
		return -EINVAL;
	}

	return execute_cmd(ss, 0, "RCPT TO:%s\r\n", addr);
}

int smtp_helo(struct smtp_session *ss, const char *domain) {
	if (ss == NULL) {
		return -EINVAL;
	}

	return execute_cmd(ss, 0, "HELO %s\r\n", domain);
}

int smtp_ehlo(struct smtp_session *ss, const char *domain) {
	if (ss == NULL) {
		return -EINVAL;
	}

	return execute_cmd(ss, 1, "EHLO %s\r\n", domain);
}

int smtp_data(struct smtp_session *ss) {
	if (ss == NULL) {
		return -EINVAL;
	}

	return execute_cmd(ss, 0, "DATA\r\n");
}

int smtp_data_add(struct smtp_session *ss, const char *data) {
	if ((ss == NULL) || (data == NULL)) {
		return -EINVAL;
	}

	return send_data(ss, data, strlen(data));
}

int smtp_data_end(struct smtp_session *ss) {
	if (ss == NULL) {
		return -EINVAL;
	}

	return execute_cmd(ss, 0, "\r\n.\r\n");
}

int smtp_rset(struct smtp_session *ss) {
	if (ss == NULL) {
		return -EINVAL;
	}

	return execute_cmd(ss, 0, "RSET\r\n");
}

int smtp_vrfy(struct smtp_session *ss, const char *str) {
	if ((ss == NULL) || (str == NULL)) {
		return -EINVAL;
	}

	return execute_cmd(ss, 0, "VRFY %s\r\n", str);
}

int smtp_expn(struct smtp_session *ss, const char *str) {
	if ((ss == NULL) || (str == NULL)) {
		return -EINVAL;
	}

	return execute_cmd(ss, 0, "EXPN %s\r\n", str);
}

int smtp_help(struct smtp_session *ss) {
	if (ss == NULL) {
		return -EINVAL;
	}

	return execute_cmd(ss, 0, "HELP\r\n");
}

int smtp_noop(struct smtp_session *ss) {
	if (ss == NULL) {
		return -EINVAL;
	}

	return execute_cmd(ss, 0, "NOOP\r\n");
}

int smtp_quit(struct smtp_session *ss) {
	if (ss == NULL) {
		return -EINVAL;
	}

	return execute_cmd(ss, 0, "QUIT\r\n");
}

int smtp_auth_plain(struct smtp_session *ss, const char *name,
		const char *secret) {
	int ret;
	char *plain, *coded;
	size_t name_len, secret_len, plain_len, coded_len;

	if ((ss == NULL) || (name == NULL) || (secret == NULL)) {
		return -EINVAL;
	}

	name_len = strlen(name);
	secret_len = strlen(secret);

	plain_len = 1 + name_len + 1 + secret_len;
	plain = alloca(plain_len);

	*plain = '\0';
	memcpy(plain + 1, name, name_len * sizeof(char));
	*(plain + 1 + name_len) = '\0';
	memcpy(plain + 1 + name_len + 1, secret, secret_len * sizeof(char));

	coded_len = b64_coded_len(plain, plain_len);
	coded = alloca(coded_len + 1);

	ret = b64_encode(plain, plain_len, coded, coded_len,
			&coded_len);
	if (ret != 0) {
		return ret;
	}

	*(coded + coded_len) = '\0';

	return execute_cmd(ss, 0, "AUTH PLAIN %s\r\n", coded);
}
