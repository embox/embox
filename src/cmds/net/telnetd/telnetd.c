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

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <utmp.h>
#include <stdlib.h>
#include <sched.h>

#include <util/math.h>

#include <framework/mod/options.h>

#define TELNETD_MAX_CONNECTIONS OPTION_GET(NUMBER,telnetd_max_connections)
#define TELNETD_USE_PTHEAD      OPTION_GET(NUMBER,telnetd_use_pthread)

#if TELNETD_USE_PTHEAD
#include <pthread.h>
#endif

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

/* http://www.tcpipguide.com/free/t_TelnetProtocolCommands-3.htm */
#define T_WILL      251
#define T_WONT      252
#define T_DO        253
#define T_DONT      254
#define T_IAC       255
#define T_INTERRUPT 244

#define O_ECHO      1     /* Manage ECHO, RFC 857 */
#define O_GO_AHEAD  3     /* Disable GO AHEAD, RFC 858 */

//TODO is not posix. Must be posix_openpt() and friends
extern int ppty(int pptyfds[2]);

static int telnet_fix_crnul(unsigned char *buf, int len);

static void telnet_cmd(int sock, unsigned char op, unsigned char param) {
	unsigned char cmd[3];

	cmd[0] = T_IAC;
	cmd[1] = op;
	cmd[2] = param;
	write(sock, cmd, 3);
}

/* Handle telnet options. Returns bytes handled. */
static size_t handle_telnet_options(const unsigned char *buf, size_t len,
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
			return min(n, len);
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
static size_t handle_client_data(unsigned char *data, size_t len,
		int sock, int pptyfd) {
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
		ops_n = handle_telnet_options(p, len, sock);
		len -= ops_n;
		p += ops_n;
	}

	/* Return size of client data copied */
	return (out - data);
}

static int utmp_login(short ut_type, const char *host) {
	struct utmp utmp;

	memset(&utmp, 0, sizeof(utmp));

	utmp.ut_type = ut_type;
	utmp.ut_pid = getpid();
	snprintf(utmp.ut_id, UT_IDSIZE, "/%d", utmp.ut_pid);
	snprintf(utmp.ut_line, UT_LINESIZE, "pty/%d", utmp.ut_pid);
	strncpy(utmp.ut_host, host, UT_HOSTSIZE - 1);
	memset(&utmp.ut_exit, 0, sizeof(struct exit_status));

	gettimeofday(&utmp.ut_tv, NULL);

	if (!pututline(&utmp)) {
		return errno;
	}

	return 0;
}

static inline void *shell_hnd(void* args) {
	int ret;
	int *msg = (int*)args;
	struct sockaddr_in sockaddr;
	socklen_t socklen = sizeof(sockaddr);

	ret = getpeername(msg[2], (struct sockaddr *)&sockaddr, &socklen);
	if (ret != 0) {
		MD(printf("getpeername return error: %d\n", errno));
		_exit(ret);
	}

	ret = utmp_login(LOGIN_PROCESS, inet_ntoa(sockaddr.sin_addr));
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
#if TELNETD_USE_PTHEAD
	ret = system("tish");
	if (ret != 0) {
		printf("system return error: %d\n", ret);
		_exit(ret);
	}
#else
	ret = execv("tish", NULL);
	if (ret != 0) {
		printf("execv return error: %d\n", ret);
		_exit(ret);
	}
#endif
	ret = utmp_login(DEAD_PROCESS, "");
	if (ret != 0) {
		MD(printf("utmp_login DEAD error: %d\n", ret));
	}

	_exit(0);
	return NULL;
}

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

struct client_args {
	int sock;
	int pptyfd[2];
};

#if !TELNETD_USE_PTHEAD
static void sigchild_handler(int sig) {
	wait(NULL);
	_exit(0);
}
#endif

/* Shell thread for telnet */
static void *telnetd_client_handler(struct client_args* client_args) {
	/* Choose tmpbuff size a half of size of pbuff to make
	 * replacement: \n\n...->\r\n\r\n... */
	unsigned char sbuff[XBUFF_LEN], pbuff[XBUFF_LEN];
	unsigned char *s = sbuff, *p = pbuff;
	int sock_data_len = 0; /* len of rest of socket data in local buffer sbuff */
	int pipe_data_len = 0; /* len of rest of pipe data in local buffer pbuff */

	int nfds;
	fd_set readfds, writefds, exceptfds;
	struct timeval timeout;
#if TELNETD_USE_PTHEAD
	pthread_t thread;
	int msg[3];
#endif

	MD(printf("starting telnet_thread_handler\n"));
	/* Set socket to be nonblock. See ignore_telnet_options() */
	fcntl(client_args->sock, F_SETFL, O_NONBLOCK);

	/* Send our parameters */
	telnet_cmd(client_args->sock, T_WILL, O_GO_AHEAD);
	telnet_cmd(client_args->sock, T_WILL, O_ECHO);

	fcntl(client_args->sock, F_SETFL, 0); /* O_NONBLOCK */
#if TELNETD_USE_PTHEAD
	msg[0] = msg[1] = client_args->pptyfd[1];
	msg[2] = client_args->sock;

	if (pthread_create(&thread, NULL, shell_hnd, (void *) &msg)) {
		telnet_cmd(client_args->sock, T_INTERRUPT, 0);
		close(client_args->sock);
		close(client_args->pptyfd[0]);
		close(client_args->pptyfd[1]);
		goto out;
	}
#else
	signal(SIGCHLD, sigchild_handler);
#endif
	/* Preparations for select call */
	nfds = max(client_args->sock, client_args->pptyfd[0]) + 1;

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

		FD_SET(client_args->sock, &readfds);
		FD_SET(client_args->pptyfd[0], &readfds);
		if (pipe_data_len > 0) {
			FD_SET(client_args->sock, &writefds);
		}
		if (sock_data_len > 0) {
			FD_SET(client_args->pptyfd[0], &writefds);
		}
		FD_SET(client_args->sock, &exceptfds);

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

		if (FD_ISSET(client_args->sock, &exceptfds)) {
			goto out_kill;
		}

		if (FD_ISSET(client_args->sock, &writefds)) {
			if ((len = write(client_args->sock, p, pipe_data_len)) > 0) {
				pipe_data_len -= len;
				p += len;
			} else {
				MD(printf("write on sock: %d %d\n", len, errno));
				goto out_kill;
			}
		}

		if (FD_ISSET(client_args->pptyfd[0], &readfds)){
			p = pbuff;
			if ((pipe_data_len = read(client_args->pptyfd[0], pbuff, XBUFF_LEN)) <= 0) {
				MD(printf("read on pptyfd: %d %d\n", pipe_data_len, errno));
				goto out_close;
			}
		}

		if (FD_ISSET(client_args->pptyfd[0], &writefds)) {
			if ((len = write(client_args->pptyfd[0], s, sock_data_len)) > 0) {
				sock_data_len -= len;
				s += len;
			} else {
				MD(printf("write on pptyfd: %d %d\n", len, errno));
				if (errno == EPIPE) {
					goto out_kill; /* this means that pipe was closed by shell */
				}
			}
		}

		if (FD_ISSET(client_args->sock, &readfds)) {
			s = sbuff;

			if ((sock_data_len = read(client_args->sock, s, XBUFF_LEN)) <= 0) {
				MD(printf("read on sock: %d %d\n", sock_data_len, errno));
			}

			sock_data_len = handle_client_data(s, sock_data_len,
				client_args->sock, client_args->pptyfd[0]);

			if (errno == ECONNREFUSED) {
				goto out_kill;
			}
		}
	} /* while(1) */

out_kill:
	write(client_args->pptyfd[0], "exit\n", 6);
out_close:
	close(client_args->pptyfd[0]);
	close(client_args->sock);
#if TELNETD_USE_PTHEAD
out:
#endif
	MD(printf("exiting from telnet_thread_handler\n"));
	_exit(0);

	return NULL;
}

int main(int argc, char **argv) {
	int listening_descr;
	int res;
	struct sockaddr_in listening_socket;
	struct sockaddr_in client_socket;
	socklen_t client_socket_len;
	int client_descr;
	int telnet_connections_count;

	if (argc > 1) {
		struct client_args client_args;

		if (0 == strcmp("connection", argv[1])) {
			client_args.sock = atoi(argv[2]);

			if (ppty(client_args.pptyfd) < 0) {
				MD(printf("new pipe error: %d\n", errno));
				close(client_args.sock);
				_exit(0);
			}
#if !TELNETD_USE_PTHEAD
			{
				int child_pid;
				char *child_argv[5];
				char client_pty[10];

				child_pid = vfork();
				if (child_pid < 0) {
					MD(printf("cannot vfork process err=%d\n", child_pid));
					_exit(0);
				}
				child_argv[0] = "telnetd";
				child_argv[1] = "shell";
				child_argv[2] = argv[2];
				child_argv[3] = itoa(client_args.pptyfd[1], client_pty, 10);
				child_argv[4] = NULL;
				if (child_pid == 0) {
					res = execv("telnetd", child_argv);
					if (res == -1) {
						MD(printf("cannot execv process err=%d\n", errno));
					}
				}
				/* FIXME
				 * It's required the only for sync.
				 * Now telnetd_client_handler() must be execute after
				 * "telnetd shell" has started
				 */
				sched_yield();
			}
#endif
			MD(printf("telnetd: %d\n", client_args.sock));
			telnetd_client_handler(&client_args);
			_exit(0);
		}
#if !TELNETD_USE_PTHEAD
		if (0 == strcmp("shell", argv[1])) {
			int msg[3];
			msg[0] = msg[1] = atoi(argv[3]);
			msg[2] = atoi(argv[2]);
			MD(printf("telnetd: shell sock %d pty %d\n", msg[1], msg[2]));
			shell_hnd(&msg);
			_exit(0);
		}
#endif
	}

	telnet_connections_count = 0;

	listening_socket.sin_family = AF_INET;
	listening_socket.sin_port = htons(TELNETD_PORT);
	listening_socket.sin_addr.s_addr = htonl(TELNETD_ADDR);

	if ((listening_descr = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("can't create socket\n");
		return -errno;
	}

	client_socket_len = sizeof(client_socket);
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
		int child_pid;
		char *child_argv[4];
		char client_desc_str[5];

		if (telnet_connections_count >= TELNETD_MAX_CONNECTIONS) {
			sleep(1);
			continue;
		}

		if (telnet_connections_count > 0) {
			child_pid = waitpid(-1, NULL, WNOHANG);
			if (child_pid > 0) {
				MD(printf("Child process with pid = %d exited\n", child_pid));
				telnet_connections_count --;
			}
		}

		client_descr = accept(listening_descr,
				(struct sockaddr *)&client_socket, &client_socket_len);

		if (client_descr < 0) {
			MD(printf("accept() failed. code=%d\n", -errno));
			continue;
		}

		MD(printf("Attempt to connect from address %s:%d\n",
			inet_ntoa(client_socket.sin_addr), ntohs(client_socket.sin_port)));

		if (client_descr >= 1000) {
			/* there are 4 digit buffer for an accepted descriptor */
			MD(printf("too big descriptor number=%d\n", client_descr));
			telnet_cmd(client_descr, T_INTERRUPT, 0);
			close(client_descr);
			continue;
		}

		child_pid = vfork();
		if (child_pid < 0) {
			telnet_cmd(client_descr, T_INTERRUPT, 0);
			MD(printf("cannot vfork process err=%d\n", child_pid));
			close(client_descr);
			continue;
		}
		child_argv[0] = "telnetd";
		child_argv[1] = "connection";
		child_argv[2] = itoa(client_descr, client_desc_str, 10);
		child_argv[3] = NULL;
		if (child_pid == 0) {
			res = execv("telnetd", child_argv);
			if (res == -1) {
				MD(printf("cannot execv process err=%d\n", errno));
			}
		}

		close(client_descr);
		telnet_connections_count ++;
	}

listen_failed:
	res = -errno;
	close(listening_descr);

	return res;
}
