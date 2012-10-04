/**
 * @file
 * @brief Tiny telnetd server
 * @date 18.04.2012
 * @author Vladimir Sokolov
 */

#include <unistd.h>
#include <string.h>
#include <net/ip.h>
#include <net/socket.h>
#include <net/inetdevice.h>
#include <util/ring_buff.h>
#include <embox/cmd.h>
#include <cmd/shell.h>
#include <err.h>
#include <errno.h>
#include <kernel/task.h>
#include <fcntl.h>

#define TELNET_SHELL_NAME "tish"

EMBOX_CMD(exec);

	/* Upper limit of concurent telnet connections.
	 * ToDo: move those into config files
	 */
#define TELNETD_MAX_CONNECTIONS 5
static int clients[TELNETD_MAX_CONNECTIONS];
	/* Telnetd address bind to */
#define TELNETD_ADDR INADDR_ANY
	/* Telnetd port bind to */
#define TELNETD_PORT 23

	/* Allow to turn off/on extra debugging information */
#if 0
#	define MD(x) do {\
		x;\
	} while (0);
#else
#	define MD(x) do{\
	} while (0);
#endif

#if 0
	/* Out a bunch of different error messages to the output and to the socket */
static void out_msgs(const char *msg, const char *msg2, const char *msg3,
					int client_descr, struct sockaddr_in *client_socket) {
	const int m_len = strlen(msg) + 1;
	MD(printf("%s", msg2));
	if(m_len != sendto(client_descr, msg, m_len, 0,
					   (struct sockaddr *)client_socket, sizeof(*client_socket))) {
		MD(printf("Can't write to the socket (%s)\n", msg3));
	}
}
#endif

#define T_WILL		251
#define T_WONT		252
#define T_DO		253
#define T_DONT		254
#define T_IAC		255
#define T_INTERRUPT 244

#define O_ECHO		1		/* Manage ECHO, RFC 857 */
#define O_GO_AHEAD	3		/* Disable GO AHEAD, RFC 858 */

static void telnet_cmd(int sock, unsigned char op, unsigned char param) {
	unsigned char cmd[3];

	cmd[0] = T_IAC;
	cmd[1] = op;
	cmd[2] = param;
	write(sock, cmd, 3);
}

	/* Skip management session */
static void ignore_telnet_options(int msg[2]) {
	unsigned char ch, op_type, param;
	int sock = msg[0];

	read(sock, &ch, 1);
	while ((ch & (1 << 7)) && (ch != T_IAC)) {
		read(sock, &ch, 1);
	}

	while (1) {
		if (ch == T_IAC) {
			read(sock, &op_type, 1);

			if (op_type == T_WILL || op_type == T_DO ||
					op_type == T_WONT || op_type == T_DONT) {
				read(sock, &param, 1);
			}

			if (op_type == T_WILL) {
				if (param == O_GO_AHEAD) {
					telnet_cmd(sock, T_DO, param);
				} else {
					telnet_cmd(sock, T_DONT, param);
				}
			} else if (op_type == T_DO) {
				if ( (param == O_GO_AHEAD) || (param == O_ECHO) ) {
					telnet_cmd(sock, T_WILL, param);
				} else {
					telnet_cmd(sock, T_WONT, param);
				}
			} else {
				/* Currently do nothing, probably it's an answer for our request */
			}
		}
		else {
			/* Get this symbol to shell, it belongs to usual traffic */
			write(msg[1], &ch, 1);
			return;
		}

		if (read(sock, &ch, 1) <= 0) {
			return;
		}
	}
}

static void *shell_hnd(void* args) {
	const struct shell *shell;
	int *msg = (int*)args;

	close(STDIN_FILENO);
	close(STDOUT_FILENO);

	dup2(msg[0], STDIN_FILENO);
	dup2(msg[1], STDOUT_FILENO);

	shell = shell_lookup(TELNET_SHELL_NAME);
	if (shell != NULL) {
		shell->exec();
	}

	return NULL;
}

/* in echo mode \n -> \r\n. See RFC 854 for NVT options */
static size_t buf_copy(unsigned char *dst, const unsigned char *src, size_t n) {
	size_t len = 0;

	for (int i = 0; i < n; i++) {
		if (*src != '\n') {
			*dst++ = *src++;
			len++;
		} else {
			*dst++ = '\r';
			*dst++ = *src++;
			len += 2;
		}
	}

	return len;
}

	/* Shell thread for telnet */
static void *telnet_thread_handler(void* args) {
	/* Choose tmpbuff size a half of size of pbuff to make
	 * replacement: \n\n...->\r\n\r\n... */
	unsigned char sbuff[128], pbuff[128], tmpbuff[64];
	unsigned char *s = sbuff, *p = pbuff;
	int sock_data_len = 0; /* len of rest of socket data in local buffer sbuff */
	int pipe_data_len = 0; /* len of rest of pipe data in local buffer pbuff */
	int sock = *(int *)args;
	int pipefd1[2], pipefd2[2], msg[2];

	fcntl(sock, F_SETFD, O_NONBLOCK);

	pipe(pipefd1);
	pipe(pipefd2);

	fcntl(pipefd1[1], F_SETFD, O_NONBLOCK);
	fcntl(pipefd2[0], F_SETFD, O_NONBLOCK);

	/* Set our parameters */
	telnet_cmd(sock, T_WILL, O_GO_AHEAD);
	telnet_cmd(sock, T_WILL, O_ECHO);

	msg[0] = sock;
	msg[1] = pipefd1[1];
	ignore_telnet_options(msg);

	msg[0] = pipefd1[0];
	msg[1] = pipefd2[1];
	new_task(shell_hnd, &msg);

	/* Close unused ends of pipes. */
	close(pipefd1[0]);
	close(pipefd2[1]);

	/* Try to read/write into/from pipes. We write raw data from socket into pipe,
	 * and than receive from it the result of command running, and send it back to
	 * client. */
	while(1) {
		int len;

		if (pipe_data_len > 0) {
			if ((len = write(sock, p, pipe_data_len)) > 0) {
				pipe_data_len -= len;
				p += len;
			}
		} else {
			p = pbuff;
			pipe_data_len = read(pipefd2[0], tmpbuff, 64);
			pipe_data_len = buf_copy(pbuff, tmpbuff, pipe_data_len);
		}

		if (sock_data_len > 0) {
			if ((len = write(pipefd1[1], s, sock_data_len)) > 0) {
				sock_data_len -= len;
				s += len;
			}
		} else {
			s = sbuff;
			sock_data_len = read(sock, s, 128);
		}
	}

	close(pipefd1[1]);
	close(pipefd2[0]);
	close(sock);
	*(int *)args = -1;

	return NULL;
}

static int exec(int argc, char **argv) {
	int res;
	int listening_descr;
	struct sockaddr_in listening_socket;

	for (res = 0; res < TELNETD_MAX_CONNECTIONS; res++) {
		clients[res] = -1;
	}

	listening_socket.sin_family = AF_INET;
	listening_socket.sin_port= htons(TELNETD_PORT);
	listening_socket.sin_addr.s_addr = htonl(TELNETD_ADDR);

	if ((listening_descr = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("can't create socket\n");
		return -1;
	}

	if ((res = bind(listening_descr, (struct sockaddr *)&listening_socket,
					sizeof(listening_socket))) < 0) {
		printf("bind() failed\n");
		goto listen_failed;
	}

	if ((res = listen(listening_descr, TELNETD_MAX_CONNECTIONS)) < 0) {
		printf("listen() failed\n");
		goto listen_failed;
	}

	MD(printf("telnetd is ready to accept connections\n"));
	while (1) {
		struct sockaddr_in client_socket;
		bool connectlimit = true;
		int client_socket_len = sizeof(client_socket);
		int client_descr = accept(listening_descr, (struct sockaddr *)&client_socket,
								  &client_socket_len);

		if (client_descr < 0) {
			MD(printf("accept() failed. code=%d\n", client_descr));
		} else {
			uint i;
			struct thread *thread;

			MD(printf("Attempt to connect from address %s:%d",
					inet_ntoa(client_socket.sin_addr), ntohs(client_socket.sin_port)) );

			for (i = 0; i < TELNETD_MAX_CONNECTIONS; i++) {
				if (clients[i] == -1) {
					clients[i] = client_descr;

					if (0 != thread_create(&thread, 0, telnet_thread_handler, &clients[i])) {
						telnet_cmd(client_descr, T_INTERRUPT, 0);
						MD(printf("thread_create() returned with code=%d\n", res));
						clients[i] = -1;
					}

					connectlimit = false;
					break;
				} /* if - if we have space for this connection */
			}

			if (connectlimit) {
				telnet_cmd(client_descr, T_INTERRUPT, 0);
				printf("%s\n", "limit");
				MD(printf("%s\n", "limit of connections"));
			}
		}
	} /* while(1) */

	assert(0);
	{
		/* Free resources, don't touch threads, but close socket decriptor
		 * So it'll influence to them
		 */
		uint i;
		for (i = 0; i < TELNETD_MAX_CONNECTIONS; i++) {
			if (clients[i] != -1) {
				close(clients[i]);
			}
		}
		close(listening_descr);
	}

	return ENOERR;

listen_failed:
	close(listening_descr);
	return -1;
}
