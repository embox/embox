/**
 * @file
 * @brief Tiny telnetd server
 *
 * @date 18.04.2012
 * @author Vladimir Sokolov
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>

#include "telnet.h"

/* Here we delete '\0' symbols from *buf:
   abc\r\0de\r\0
   abc\rde\r*/
static int telnet_fix_crnul(unsigned char *buf, int len) {
	unsigned char *bpi = buf, *bpo = buf;
	while (bpi < buf + len) {
		if (bpi < buf + len - 1 && 0 == memcmp(bpi, "\r\0", 2)) {
			*bpo++ = *bpi;
			bpi += 2;
		} else {
			*bpo++ = *bpi++;
		}
	}
	return bpo - buf;
}

void telnet_cmd(int sock, unsigned char op, unsigned char param) {
	unsigned char cmd[3];

	cmd[0] = T_IAC;
	cmd[1] = op;
	cmd[2] = param;
	write(sock, cmd, 3);
}

/* Handle telnet options. Returns bytes handled. */
static size_t telnet_handle_options(const unsigned char *buf, size_t len,
		int sock) {
	unsigned char *p;
	unsigned char op_type;
	unsigned char op_param = '\0';
	size_t n = 0;

	if (len == 0) {
		return 0;
	}

	p = (unsigned char *) buf;

	while (1) {
		if (*p++ != T_IAC || n++ >= len) {
			return n < len ? n : len;
		}

		op_type = *p++;
		n++;

		if (op_type == T_WILL || op_type == T_DO ||
				op_type == T_WONT || op_type == T_DONT) {
			op_param = *p++;
			n++;
		}

		if (n > len) {
			/* XXX End of packet. It means, probably, it's something wrong
			 * with telnet options, e.g. only part of options arrived
			 * in this packet. So just return @len. */
			return len;
		}

		if (op_type == T_WILL) {
			if (op_param == O_GO_AHEAD) {
				telnet_cmd(sock, T_DO, op_param);
			} else {
				telnet_cmd(sock, T_DONT, op_param);
			}
		} else if (op_type == T_DO) {
			if ((op_param == O_GO_AHEAD) || (op_param == O_ECHO)) {
				telnet_cmd(sock, T_WILL, op_param);
			} else {
				telnet_cmd(sock, T_WONT, op_param);
			}
		} else {
			/* Currently do nothing, probably it's an answer for our request */
		}
	}
	return 0;
}

/* Process raw client data from @data and save it back to original @data.
 *
 * Client data can contain not only client data, but also telnet options.
 * Example of packets:
 *  - |***client_data***telnet_options***|
 *  - |***telnet_options***client_data***|
 *  - |***client_data***telnet_options***client_data***|
 */
size_t telnet_handle_client_data(unsigned char *data, size_t len,
		int sock) {
	unsigned char *p;
	unsigned char *out;
	unsigned char *client_start; /* Start of client bytes */
	size_t client_n; /* Number of client bytes */
	size_t ops_n; /* Number of telnet options bytes */

	p = data;
	out = data;

	while (len) {
		/* Get client data till the next telnet options */
		client_start = p;
		client_n = 0;
		while ((*p != T_IAC) && len) {
			p++;
			client_n++;
			len--;
		}
		/* Process client data */
		if (client_n > 0) {
			client_n = telnet_fix_crnul(client_start, client_n);
			memcpy(out, client_start, client_n);
			out += client_n;
		}

		/* Precess telnet options till the next client data */
		assert((*p == T_IAC) || (len == 0));
		ops_n = telnet_handle_options(p, len, sock);
		len -= ops_n;
		p += ops_n;
	}

	/* Return size of client data copied */
	return (out - data);
}

extern int ppty(int pptyfds[2]);

int telnet_open_master_pty(struct telnet_session *ts) {
	int master_pty;

#ifdef TELNETD_FOR_LINUX
	master_pty = open("/dev/ptmx", O_RDWR);
	if (master_pty < 0) {
		return -1;
	}
	grantpt(master_pty);
	unlockpt(master_pty);
#else
	if (ppty(ts->pptyfds)) {
		return -1;
	}
	master_pty = ts->pptyfds[0];
#endif
	return master_pty;
}

int telnet_open_slave_pty(struct telnet_session *ts, int master_pty) {
	int slave_pty;

#ifdef TELNETD_FOR_LINUX
	pid_t pid;
	char slave_name[32];

	pid = getpid();
	setsid();

	ptsname_r(master_pty, slave_name, 32);
	slave_pty = open(slave_name, O_RDWR);

	tcsetpgrp(slave_pty, pid);
#else
	slave_pty = ts->pptyfds[1];
#endif

	return slave_pty;
}
