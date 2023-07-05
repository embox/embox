/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 30.01.23
 */
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <util/err.h>
#include <cmd/shell.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <debug/gdbstub.h>
#include <framework/cmd/api.h>
#include <framework/cmd/types.h>
#include <kernel/thread/thread_sched_wait.h>

static int gdbfd;

static void *extra_bpt;

static bool prog_running;

static struct gdbstub_env *env_ptr;

static struct thread *interrupt_handler;

static ssize_t (*read_packet)(char *dst, size_t nbyte);

static ssize_t read_packet1(char *dst, size_t nbyte) {
	ssize_t nread;
	char *ptr;

	for (;;) {
		nread = read(gdbfd, dst, nbyte);
		if (nread > 0) {
			ptr = memchr(dst, '$', nread);
			if (!ptr) {
				continue;
			}
			nread -= ptr - dst;
			memcpy(dst, ptr, nread);
		}
		break;
	}
	return nread;
}

static ssize_t read_packet2(char *dst, size_t nbyte) {
	ssize_t nread;

	do {
		nread = read(gdbfd, dst, 1);
	} while ((nread == 1) && (*dst != '$'));

	if (nread == 1) {
		while (nread < nbyte) {
			if (1 != read(gdbfd, ++dst, 1)) {
				break;
			}
			nread++;
			if ((*dst == '#') && (nread + 2 < nbyte)) {
				nbyte = nread + 2;
			}
		}
	}
	return nread;
}

static ssize_t write_packet(const char *src, size_t nbyte) {
	return write(gdbfd, src, nbyte);
}

static int prepare_tcp(const char *host_port) {
	struct sockaddr_in sockaddr;
	in_addr_t addr;
	in_port_t port;
	char *delim;
	char *endptr;
	int listen_fd;
	int ret;

	listen_fd = -1;
	ret = 0;

	delim = strchr(host_port, ':');
	if ((delim == NULL) || (delim[1] == '\n')) {
		ret = -EINVAL;
		goto out;
	}
	*delim = '\0';

	addr = inet_addr(host_port);
	if (addr == INADDR_BROADCAST) {
		ret = -EINVAL;
		goto out;
	}

	port = strtoul(delim + 1, &endptr, 10);
	if (*endptr != '\0') {
		ret = -EINVAL;
		goto out;
	}

	listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_fd == -1) {
		ret = -errno;
		goto out;
	}

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = addr;

	if (-1 == bind(listen_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr))) {
		ret = -errno;
		goto out;
	}

	if (-1 == listen(listen_fd, 1)) {
		ret = -errno;
		goto out;
	}

	printf("Listening on port %hu\n", port);

	gdbfd = accept(listen_fd, NULL, NULL);
	if (gdbfd == -1) {
		ret = -errno;
	}

out:
	close(listen_fd);
	return ret;
}

static int prepare_serial(const char *tty) {
	struct termios t;
	int ret;

	ret = 0;

	gdbfd = open(tty, O_RDWR);
	if (gdbfd == -1) {
		ret = -errno;
		goto out;
	}

	if (-1 == tcgetattr(gdbfd, &t)) {
		ret = -errno;
		goto out;
	}

	t.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	t.c_oflag &= ~(OPOST);
	t.c_cflag |= (CS8);
	t.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;

	if (-1 == tcsetattr(gdbfd, TCSANOW, &t)) {
		ret = -errno;
		goto out;
	}

	printf("Remote debugging using %s\n", tty);

out:
	return ret;
}

static void handle_breakpoint(struct gdb_regs *regs) {
	struct gdb_packet *pkt;

	pkt = &env_ptr->packet;
	env_ptr->regs = regs;

	if (extra_bpt) {
		env_ptr->arch->remove_bpt(extra_bpt, 0);
		extra_bpt = NULL;
	}

	if (prog_running) {
		prog_running = false;

		pkt->buf[1] = 'c';
		gdb_process_packet(env_ptr);
		write_packet(pkt->buf, pkt->size);
	}

	for (;;) {
		if (0 >= read_packet(pkt->buf, sizeof(pkt->buf))) {
			env_ptr->arch->remove_all_bpts();
			break;
		}

		write_packet("+", 1);
		gdb_process_packet(env_ptr);

		if (env_ptr->cmd == GDBSTUB_DETACH) {
			env_ptr->arch->remove_all_bpts();
			break;
		}

		if (env_ptr->cmd == GDBSTUB_CONT) {
			break;
		}

		write_packet(pkt->buf, pkt->size);
	}

	prog_running = true;

	if (thread_self() != interrupt_handler) {
		sched_wakeup(&interrupt_handler->schedee);
	}
}

static void trigger_breakpoint(void) {
}

static void *handle_gdb_interrupt(void *pipefd) {
	struct pollfd pfd[2];

	pfd[0].fd = *(int *)pipefd;
	pfd[0].events = POLLIN;

	pfd[1].fd = gdbfd;
	pfd[1].events = POLLIN;

	for (;;) {
		SCHED_WAIT(prog_running);

		if (-1 == poll(pfd, 2, -1)) {
			env_ptr->arch->remove_all_bpts();
			break;
		}

		if (pfd[0].revents == POLLIN) {
			break;
		}

		if (prog_running) {
			extra_bpt = trigger_breakpoint;
			env_ptr->arch->insert_bpt(extra_bpt, 0);

			trigger_breakpoint();
		}
	}

	return NULL;
}

int main(int argc, char *argv[]) {
	const struct shell *sh;
	const struct cmd *cmd;
	struct gdbstub_env env;
	struct gdb_arch arch;
	int pipefd[2];
	pid_t pid;
	int ret;

	gdbfd = -1;
	env_ptr = &env;
	prog_running = false;

	pipefd[0] = -1;
	pipefd[1] = -1;

	if (argc < 3) {
		ret = -EINVAL;
		goto out;
	}

	sh = shell_lookup(argv[2]);
	if (sh) {
		extra_bpt = sh->run;
	}
	else {
		cmd = cmd_lookup(argv[2]);
		if (!cmd) {
			ret = -ENOENT;
			goto out;
		}
		extra_bpt = cmd->exec;
	}

	if (isdigit(*argv[1])) {
		read_packet = read_packet1;
		ret = prepare_tcp(argv[1]);
	}
	else {
		read_packet = read_packet2;
		ret = prepare_serial(argv[1]);
	}
	if (ret < 0) {
		goto out;
	}

	if (-1 == pipe(pipefd)) {
		ret = -errno;
		goto out;
	}

	interrupt_handler = thread_create(0, handle_gdb_interrupt, pipefd);
	if (err(interrupt_handler)) {
		ret = err(interrupt_handler);
		goto out;
	}

	gdb_arch_init(&arch);
	env.arch = &arch;

	gdb_arch_prepare(handle_breakpoint);
	arch.insert_bpt(extra_bpt, 0);

	pid = vfork();
	if (pid < 0) {
		ret = -errno;
	}
	else if (pid == 0) {
		execv(argv[2], &argv[2]);
		exit(1);
	}
	else {
		waitpid(pid, NULL, 0);
	}

	arch.remove_all_bpts();
	gdb_arch_cleanup();

	write(pipefd[1], "\x03", 1);
	thread_join(interrupt_handler, NULL);

out:
	if (ret == -EINVAL) {
		printf("Usage: %s [HOST]:[PORT] [PROG] [PROG_ARGS ...]\n", argv[0]);
		printf("       %s [TTY_DEV] [PROG] [PROG_ARGS ...]\n", argv[0]);
	}

	close(gdbfd);
	close(pipefd[0]);
	close(pipefd[1]);

	return ret;
}
