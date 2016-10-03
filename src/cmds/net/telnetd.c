/**
 * @file
 * @brief Tiny telnetd server
 *
 * @date 18.04.2012
 * @author Vladimir Sokolov
 */

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <utmp.h>

#include <net/l3/ipv4/ip.h>
#include <net/inetdevice.h>

#include <util/math.h>

#include <kernel/task.h>

#include <cmd/shell.h>

#define TELNETD_MAX_CONNECTIONS OPTION_GET(NUMBER,telnetd_max_connections)
	/* Telnetd address bind to */
#define TELNETD_ADDR INADDR_ANY
	/* Telnetd port bind to */
#define TELNETD_PORT 23

#define XBUFF_LEN 128

	/* Allow to turn off/on extra debugging information */
#if 0
#	define MD(x) do {\
		x;\
	} while (0);
#else
#	define MD(x) do{\
	} while (0);
#endif

#define T_WILL		251
#define T_WONT		252
#define T_DO		253
#define T_DONT		254
#define T_IAC		255
#define T_INTERRUPT 244

#define O_ECHO		1		/* Manage ECHO, RFC 857 */
#define O_GO_AHEAD	3		/* Disable GO AHEAD, RFC 858 */

extern int ppty(int pptyfds[2]);

static struct {
	int fd;
	struct sockaddr_in addr_in;
} clients[TELNETD_MAX_CONNECTIONS];
static int listening_descr;

static void telnet_cmd(int sock, unsigned char op, unsigned char param) {
	unsigned char cmd[3];

	cmd[0] = T_IAC;
	cmd[1] = op;
	cmd[2] = param;
	write(sock, cmd, 3);
}

/* Skip management session */
static void ignore_telnet_options(int sock, int pptyfd) {
	unsigned char ch, op_type, param;

	read(sock, &ch, 1);
	while ((ch & (1 << 7)) && (ch != T_IAC)) {
		read(sock, &ch, 1);
	}

	while (1) {
		if (ch == T_IAC) {
			read(sock, &op_type, 1);

			if (op_type == T_WILL || op_type == T_DO || op_type == T_WONT || op_type == T_DONT) {
				read(sock, &param, 1);
			}

			if (op_type == T_WILL) {
				if (param == O_GO_AHEAD) {
					telnet_cmd(sock, T_DO, param);
				} else {
					telnet_cmd(sock, T_DONT, param);
				}
			} else if (op_type == T_DO) {
				if ((param == O_GO_AHEAD) || (param == O_ECHO)) {
					telnet_cmd(sock, T_WILL, param);
				} else {
					telnet_cmd(sock, T_WONT, param);
				}
			} else {
				/* Currently do nothing, probably it's an answer for our request */
			}
		} else {
			/* Get this symbol to shell, it belongs to usual traffic */
			write(pptyfd, &ch, 1);
			return;
		}

		/* We use here nonblock socket */
		if (read(sock, &ch, 1) <= 0) {
			return;
		}
	}
}

static int utmp_login(short ut_type, const char *host) {
	struct utmp utmp;

	utmp.ut_type = ut_type;
	utmp.ut_pid = getpid();
	snprintf(utmp.ut_id, UT_IDSIZE, "/%d", utmp.ut_pid);
	snprintf(utmp.ut_line, UT_LINESIZE, "pty/%d", utmp.ut_pid);
	strncpy(utmp.ut_host, host, UT_HOSTSIZE);
	memset(&utmp.ut_exit, 0, sizeof(struct exit_status));

	gettimeofday(&utmp.ut_tv, NULL);

	if (!pututline(&utmp)) {
		return errno;
	}

	return 0;

}

static void *shell_hnd(void* args) {
	int ret;
	int *msg = (int*)args;

	ret = utmp_login(LOGIN_PROCESS, inet_ntoa(clients[msg[2]].addr_in.sin_addr));
	if (ret != 0) {
		MD(printf("utmp_login LOGIN error: %d\n", ret));
	}

	if (-1 == close(STDIN_FILENO)) {
		MD(printf("close STDIN_FILENO error: %d\n", errno));
	}
	if (-1 == close(STDOUT_FILENO)) {
		MD(printf("close STDOUT_FILENO error: %d\n", errno));
	}
	if (-1 == close(STDERR_FILENO)) {
		MD(printf("close STDERR_FILENO error: %d\n", errno));
	}

	if (-1 == dup2(msg[0], STDIN_FILENO)) {
		MD(printf("dup2 STDIN_FILENO error: %d\n", errno));
	}
	if (-1 == dup2(msg[1], STDOUT_FILENO)) {
		MD(printf("dup2 STDOUT_FILENO error: %d\n", errno));
	}
	if (-1 == dup2(msg[1], STDERR_FILENO)) {
		MD(printf("dup2 STDERR_FILENO error: %d\n", errno));
	}

	ret = shell_run(shell_lookup("tish"));
	if (ret != 0) {
		MD(printf("shell_run error: %d\n", ret));
	}

	ret = utmp_login(DEAD_PROCESS, "");
	if (ret != 0) {
		MD(printf("utmp_login DEAD error: %d\n", ret));
	}

	return NULL;
}

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

/* Shell thread for telnet */
static void *telnet_thread_handler(void* args) {
	/* Choose tmpbuff size a half of size of pbuff to make
	 * replacement: \n\n...->\r\n\r\n... */
	unsigned char sbuff[XBUFF_LEN], pbuff[XBUFF_LEN];
	unsigned char *s = sbuff, *p = pbuff;
	int sock_data_len = 0; /* len of rest of socket data in local buffer sbuff */
	int pipe_data_len = 0; /* len of rest of pipe data in local buffer pbuff */
	int client_num = (int) args;
	int sock = clients[client_num].fd;
	int msg[3];
	int pptyfd[2];
	int tid;
	int nfds;
	fd_set readfds, writefds, exceptfds;
	struct timeval timeout;

	MD(printf("starting telnet_thread_handler\n"));
	/* Set socket to be nonblock. See ignore_telnet_options() */
	fcntl(sock, F_SETFL, O_NONBLOCK);

	if (ppty(pptyfd) < 0) {
		MD(printf("new pipe error: %d\n", errno));
		close(sock);
		goto out;
	}

	/* Send our parameters */
	telnet_cmd(sock, T_WILL, O_GO_AHEAD);
	telnet_cmd(sock, T_WILL, O_ECHO);

	/* handle options from client */
	ignore_telnet_options(sock, pptyfd[0]);

	fcntl(sock, F_SETFL, 0); /* O_NONBLOCK */

	msg[0] = msg[1] = pptyfd[1];
	msg[2] = client_num;
	if ((tid = new_task("telnetd user", shell_hnd, &msg)) < 0) {
		MD(printf("new task error: %d\n", -tid));
		close(sock);
		close(pptyfd[0]);
		close(pptyfd[1]);
		goto out;
	}

	close(pptyfd[1]);

	/* Preparations for select call */
	nfds = max(sock, pptyfd[0]) + 1;

	timeout.tv_sec = 100;
	timeout.tv_usec = 0;

	/* Try to read/write into/from pipes. We write raw data from socket into pipe,
	 * and than receive from it the result of command running, and send it back to
	 * client. */
	while(1) {
		int len;
		int fd_cnt;

		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);

		FD_SET(sock, &readfds);
		FD_SET(pptyfd[0], &readfds);
		if (pipe_data_len > 0) {
			FD_SET(sock, &writefds);
		}
		if (sock_data_len > 0) {
			FD_SET(pptyfd[0], &writefds);
		}
		FD_SET(sock, &exceptfds);

		MD(printf("."));

		fd_cnt = select(nfds, &readfds, &writefds, &exceptfds, &timeout);

		/* XXX telnet must receive signal on socket closing, but now
		 * alternatively here is this check */
		if (!fd_cnt) {
#if 0
			fcntl(sock, F_SETFL, O_NONBLOCK);
			len = read(sock, s, XBUFF_LEN);
			if ((len == 0) || ((len == -1) && (errno != EAGAIN))) {
				MD(printf("read on sock: %d %d\n", len, errno));
				goto out_kill;
			}
			fcntl(sock, F_SETFL, 0);

			/* preventing further execution since some fds is set,
 			 * but they are not active and will block (fd_cnt == 0)
			 */
#endif
			continue;
		}

		if (FD_ISSET(sock, &exceptfds)) {
			goto out_kill;
		}

		if (FD_ISSET(sock, &writefds)) {
			if ((len = write(sock, p, pipe_data_len)) > 0) {
				pipe_data_len -= len;
				p += len;
			} else {
				MD(printf("write on sock: %d %d\n", len, errno));
				goto out_kill;
			}
		}

		if (FD_ISSET(pptyfd[0], &readfds)){
			p = pbuff;
			if ((pipe_data_len = read(pptyfd[0], pbuff, XBUFF_LEN)) <= 0) {
				MD(printf("read on pptyfd: %d %d\n", pipe_data_len, errno));
				goto out_close;
			}
		}

		if (FD_ISSET(pptyfd[0], &writefds)) {
			if ((len = write(pptyfd[0], s, sock_data_len)) > 0) {
				sock_data_len -= len;
				s += len;
			} else {
				MD(printf("write on pptyfd: %d %d\n", len, errno));
				if (errno == EPIPE) {
					goto out_kill; /* this means that pipe was closed by shell */
				}
			}
		}

		if (FD_ISSET(sock, &readfds)){
			s = sbuff;
			if ((sock_data_len = read(sock, s, XBUFF_LEN)) <= 0) {
				MD(printf("read on sock: %d %d\n", sock_data_len, errno));
			}
			sock_data_len = telnet_fix_crnul(s, sock_data_len);
			if (errno == ECONNREFUSED) {
				goto out_kill;
			}
		}
	} /* while(1) */

out_kill:
	kill(tid, 9);
out_close:
	close(pptyfd[0]);
	close(sock);
	clients[client_num].fd = -1;

	waitpid(tid, NULL, 0);

out:
	MD(printf("exiting from telnet_thread_handler\n"));

	return NULL;
}

int main(int argc, char **argv) {
	int res;
	struct sockaddr_in listening_socket;
	struct sockaddr_in client_socket;
	int client_socket_len = sizeof(client_socket);

	for (res = 0; res < TELNETD_MAX_CONNECTIONS; res++) {
		clients[res].fd = -1;
	}

	listening_socket.sin_family = AF_INET;
	listening_socket.sin_port = htons(TELNETD_PORT);
	listening_socket.sin_addr.s_addr = htonl(TELNETD_ADDR);

	if ((listening_descr = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("can't create socket\n");
		return -errno;
	}

	if (bind(listening_descr, (struct sockaddr *)&listening_socket,
											sizeof(listening_socket)) < 0) {
		printf("bind() failed\n");
		goto listen_failed;
	}

	if (listen(listening_descr, TELNETD_MAX_CONNECTIONS) < 0) {
		printf("listen() failed\n");
		goto listen_failed;
	}

	MD(printf("telnetd is ready to accept connections\n"));
	while (1) {
		int client_descr = accept(listening_descr, (struct sockaddr *)&client_socket,
								  &client_socket_len);
		struct thread *thread;
		size_t i;

		if (client_descr < 0) {
			MD(printf("accept() failed. code=%d\n", -errno));
			continue;
		}

		MD(printf("Attempt to connect from address %s:%d\n",
			inet_ntoa(client_socket.sin_addr), ntohs(client_socket.sin_port)));

		for (i = 0; i < TELNETD_MAX_CONNECTIONS; i++) {
			if (clients[i].fd == -1) {
				break;
			}
		}

		if (i == TELNETD_MAX_CONNECTIONS) {
			telnet_cmd(client_descr, T_INTERRUPT, 0);
			MD(printf("limit of connections exceded\n"));
			continue;
		}

		clients[i].fd = client_descr;
		memcpy(&clients[i].addr_in, &client_socket, sizeof(client_socket));

		if (pthread_create(&thread, NULL, telnet_thread_handler, (void *) i)) {
			telnet_cmd(client_descr, T_INTERRUPT, 0);
			MD(printf("thread_create() returned with code=%d\n", res));
			clients[i].fd = -1;
		}
	}

listen_failed:
	res = -errno;
	close(listening_descr);
	return res;
}
