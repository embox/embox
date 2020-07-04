/**
 * @file
 * @brief Tiny telnetd server
 *
 * @date 18.04.2012
 * @author Vladimir Sokolov
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include "telnet.h"

/* Uncomment to enable debug messages. */
/* #define TELNETD_DEBUG */

#ifdef TELNETD_FOR_LINUX
/* Using port >= 1024 to run without sudo */
#define TELNETD_PORT 3023
#define TELNETD_MAX_CONNECTIONS 4
#define SHELL_NAME  "/bin/sh"
#else
#include <framework/mod/options.h>
#define TELNETD_PORT OPTION_GET(NUMBER,port)
#define TELNETD_MAX_CONNECTIONS OPTION_GET(NUMBER,max_connections)
#define SHELL_NAME OPTION_STRING_GET(shell)
#endif

#ifdef TELNETD_DEBUG
#define DPRINTF(fmt, ...) \
    do { \
        fprintf(stdout, "telnet: %s: " fmt, __func__, ## __VA_ARGS__); \
    } while (0)
#else
#define DPRINTF(fmt, ...) do {} while (0)
#endif

/* Telnetd address bind to */
#define TELNETD_ADDR INADDR_ANY
/* Telnetd port bind to */

static struct telnet_session *tsessions[TELNETD_MAX_CONNECTIONS];
static int telnet_maxfd;

static void telnet_find_new_maxfd(void) {
	int i;
	struct telnet_session *ts;
	for (i = 0; i < TELNETD_MAX_CONNECTIONS; i++) {
		ts = tsessions[i];
		if (!ts) {
			continue;
		}
		if (ts->sockfd > telnet_maxfd) {
			telnet_maxfd = ts->sockfd;
		}
		if (ts->ptyfd > telnet_maxfd) {
			telnet_maxfd = ts->ptyfd;
		}
	}
}

static struct telnet_session *telnet_alloc_session(void) {
	struct telnet_session *ts;
	int i;
	DPRINTF("telnet_alloc_session\n");
	for (i = 0; i < TELNETD_MAX_CONNECTIONS; i++) {
		if (!tsessions[i]) {
			ts = malloc(sizeof *ts);
			if (!ts) {
				return NULL;
			}
			memset(ts, 0, sizeof *ts);
			tsessions[i] = ts;
			return ts;
		}
	}
	return NULL;
}

static void telnet_free_session(struct telnet_session *ts) {
	int i;
	pid_t pid;

	DPRINTF("telnet_free_session\n");

	pid = ts->pid;
	if (pid != -1) {
		/* Kill session if it is not killed yet with usual exit. */
		kill(pid, SIGKILL);
		waitpid(pid, NULL, 0);
	}

	close(ts->sockfd);
	close(ts->ptyfd);

	for (i = 0; i < TELNETD_MAX_CONNECTIONS; i++) {
		if (tsessions[i] == ts) {
			free(ts);
			tsessions[i] = NULL;
			break;
		}
	}
}

static void telnet_sigchld_hnd(int sig) {
	pid_t pid;
	struct telnet_session *ts;
	int i;

	DPRINTF("Get SIGCHLD\n");

	while (1) {
		pid = waitpid(-1, NULL, WNOHANG);
		if (pid <= 0) {
			break;
		}
		for (i = 0; i < TELNETD_MAX_CONNECTIONS; i++) {
			ts = tsessions[i];
			if (ts && (ts->pid == pid)) {
				ts->pid = -1;
				break;
			}
		}
	}
}

static void telnet_execv_shell(void) {
	char *const shell_args[] = {SHELL_NAME, NULL};
	execv(SHELL_NAME, shell_args);
}

static int telnet_create_new_session(int sock, struct telnet_session **tsp) {
	int master_pty, slave_pty;
	pid_t pid;
	struct telnet_session *ts;

	ts = telnet_alloc_session();
	if (!ts) {
		DPRINTF("Failed to alloc new session\n");
		return -1;
	}
	*tsp = ts;

	master_pty = telnet_open_master_pty(ts);
	if (master_pty < 0) {
		DPRINTF("Failed to open master pty\n");
		return -1;
	}
	/* It's used to do not pass pty master to children. */
	fcntl(master_pty, F_SETFD, FD_CLOEXEC);

	ts->sockfd = sock;
	ts->ptyfd  = master_pty;
	if (ts->sockfd > telnet_maxfd) {
		telnet_maxfd = ts->sockfd;
	}
	if (ts->ptyfd > telnet_maxfd) {
		telnet_maxfd = ts->ptyfd;
	}

	/* Send our parameters */
	telnet_cmd(ts->sockfd, T_WILL, O_GO_AHEAD);
	telnet_cmd(ts->sockfd, T_WILL, O_ECHO);

	pid = vfork();
	if (pid < 0) {
		close(master_pty);
		telnet_free_session(ts);
		DPRINTF("vfork failed\n");
		return -1;
	}
	if (pid > 0) {
		/* Parent */
#ifndef TELNETD_FOR_LINUX
		/* FIXME remove. CLosing the slave end. */
		close((*tsp)->pptyfds[1]);
#endif
		return pid;
	}
	/* Child */
	slave_pty = telnet_open_slave_pty(ts, master_pty);
	if (slave_pty < 0) {
		DPRINTF("Failed to open slave pty\n");
		exit(1);
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	dup2(slave_pty, STDIN_FILENO);
	dup2(slave_pty, STDOUT_FILENO);
	dup2(slave_pty, STDERR_FILENO);

	telnet_execv_shell();

	return -1;
}

static void telnet_main_loop(int server_sock) {
	int fd_count;
	fd_set readfds, writefds;
	struct telnet_session *ts;
	int i;

	signal(SIGCHLD, telnet_sigchld_hnd);

main_loop:
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	/* Setup select fd sets for each active session. */
	for (i = 0; i < TELNETD_MAX_CONNECTIONS; i++) {
		ts = tsessions[i];
		if (!ts) {
			continue;
		}
		if (ts->pid == -1) {
			telnet_free_session(ts);
			continue;
		}
		if (!ts->sock_buff.count) {
			/* Read to buf only if it is empty */
			ts->sock_buff.p = ts->sock_buff.data;
			FD_SET(ts->sockfd, &readfds);
		} else {
			FD_SET(ts->ptyfd, &writefds);
		}
		if (!ts->pty_buff.count) {
			/* Read to buf only if it is empty */
			ts->pty_buff.p = ts->pty_buff.data;
			FD_SET(ts->ptyfd, &readfds);
		} else {
			FD_SET(ts->sockfd, &writefds);
		}
	}
	/* Here we find maximal fd among all sessions for select(). */
	telnet_find_new_maxfd();

	/* We also select on server socket for new incoming connections. */
	FD_SET(server_sock, &readfds);
	if (server_sock > telnet_maxfd) {
		telnet_maxfd = server_sock;
	}

	fd_count = select(telnet_maxfd + 1, &readfds, &writefds, NULL, NULL);
	if (fd_count <= 0) {
		/* Try again */
		goto main_loop;
	}

	if (FD_ISSET(server_sock, &readfds)) {
		int client_sock;
		struct sockaddr_in client_sockaddr;
		pid_t pid;
		socklen_t client_socket_len = sizeof client_sockaddr;

		client_sock = accept(server_sock,
				(struct sockaddr *)&client_sockaddr, &client_socket_len);
		if (client_sock < 0) {
			DPRINTF("accept() failed. code=%d\n", -errno);
			goto main_loop;
		}
		/* It's used to do not pass socket to children. */
		fcntl(client_sock, F_SETFD, FD_CLOEXEC);
		DPRINTF("Connection from address %s:%d\n",
				inet_ntoa(client_sockaddr.sin_addr),
				ntohs(client_sockaddr.sin_port));

		pid = telnet_create_new_session(client_sock, &ts);
		if (pid < 0) {
			/* Close current connection and process other below. */
			DPRINTF("Failed to create new session\n");
			close(client_sock);
		} else {
			ts->pid = pid;
		}
	}

	/* For each telnet session */
	for (i = 0; i < TELNETD_MAX_CONNECTIONS; i++) {
		int len;

		ts = tsessions[i];
		if (!ts) {
			continue;
		}

		if (FD_ISSET(ts->sockfd, &writefds)) {
			len = write(ts->sockfd, ts->pty_buff.p, ts->pty_buff.count);
			if (len > 0) {
				ts->pty_buff.count -= len;
				ts->pty_buff.p += len;
			} else {
				DPRINTF("write to server sock failed\n");
				goto kill_session;
			}
		}

		if (FD_ISSET(ts->ptyfd, &readfds)) {
			ts->pty_buff.count = read(ts->ptyfd,
	                                  ts->pty_buff.data, DATA_BUF_LEN);
			if (ts->pty_buff.count <= 0) {
				DPRINTF("read from pty master failed\n");
				goto kill_session;
			}
		}

		if (FD_ISSET(ts->ptyfd, &writefds)) {
			len = write(ts->ptyfd, ts->sock_buff.p, ts->sock_buff.count);
			if (len > 0) {
				ts->sock_buff.count -= len;
				ts->sock_buff.p += len;
			} else {
				/* this means that pipe was closed by shell */
				DPRINTF("write to pty master failed\n");
				goto kill_session;
			}
		}

		if (FD_ISSET(ts->sockfd, &readfds)) {
			ts->sock_buff.count = read(ts->sockfd,
	                                   ts->sock_buff.data, DATA_BUF_LEN);
			if (ts->sock_buff.count <= 0) {
				goto kill_session;
			}
			ts->sock_buff.count = telnet_handle_client_data(ts->sock_buff.p,
	                                   ts->sock_buff.count, ts->sockfd);
		}
		continue;
kill_session:
		telnet_free_session(ts);
	}
	goto main_loop;
}

int main(int argc, char **argv) {
	struct sockaddr_in server_sockaddr;
	int server_sock;

	server_sockaddr.sin_family      = AF_INET;
	server_sockaddr.sin_port        = htons(TELNETD_PORT);
	server_sockaddr.sin_addr.s_addr = htonl(TELNETD_ADDR);

	if ((server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("telnet: Can't create main server socket\n");
		return -errno;
	}
	if (bind(server_sock, (struct sockaddr *)&server_sockaddr,
			sizeof server_sockaddr) < 0) {
		printf("telnet: bind() failed on main server socket (%s)\n",
			strerror(errno));
		goto listen_failed;
	}
	if (listen(server_sock, TELNETD_MAX_CONNECTIONS) < 0) {
		printf("telnet: listen() failed on main server socket\n");
		goto listen_failed;
	}

	telnet_main_loop(server_sock);

listen_failed:
	close(server_sock);

	return -errno;
}
