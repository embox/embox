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
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <kernel/sched.h>
#include <kernel/thread.h>
#include <debug/gdbstub.h>
#include <framework/cmd/api.h>
#include <framework/cmd/types.h>
#include <kernel/thread/thread_sched_wait.h>

extern int diag_fd(void);

static int gdb_fd = -1;

static bool gdb_running = false;
static bool gdb_interrupted = false;

static struct gdbstub_env *env_ptr;

static struct thread *monitored_thread;
static struct thread *gdbserver_thread;

static ssize_t (*read_packet)(char *dst, size_t nbyte);

static ssize_t read_packet1(char *dst, size_t nbyte) {
	ssize_t nread;
	char *ptr;

	for (;;) {
		nread = read(gdb_fd, dst, nbyte);
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
	char tmp[4];

	do {
		nread = read(gdb_fd, dst, 1);
	} while ((nread == 1) && (*dst != '$'));

	if (nread == 1) {
		for (; nread < nbyte; nread++) {
			read(gdb_fd, ++dst, 1);
			if (*dst == '#') {
				read(gdb_fd, tmp, sizeof(tmp));
				break;
			}
		}
	}
	return nread;
}

static ssize_t write_packet(const char *src, size_t nbyte) {
	return write(gdb_fd, src, nbyte);
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

	gdb_fd = accept(listen_fd, NULL, NULL);
	if (gdb_fd == -1) {
		ret = -errno;
	}

out:
	close(listen_fd);
	return ret;
}

static int prepare_serial(const char *tty) {
	struct termios t;

	printf("Remote debugging using %s\n", tty);

	gdb_fd = open(tty, O_RDWR);
	if (gdb_fd == -1) {
		return -errno;
	}

	if (-1 == tcgetattr(gdb_fd, &t)) {
		return -errno;
	}

	t.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	t.c_oflag &= ~(OPOST);
	t.c_cflag |= (CS8);
	t.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;

	if (-1 == tcsetattr(gdb_fd, TCSANOW, &t)) {
		return -errno;
	}

	return 0;
}

static void gdb_handle_debug_excpt(struct gdb_regs *regs) {
	monitored_thread = thread_self();
	env_ptr->regs = regs;

	if (gdb_running | gdb_interrupted) {
		if (gdb_interrupted) {
			env_ptr->arch->remove_bpt(sched_finish_switch, 0);
		}

		gdb_running = false;
		gdb_interrupted = false;

		env_ptr->packet.buf[1] = '?';
		gdb_process_packet(env_ptr);
		write_packet(env_ptr->packet.buf, env_ptr->packet.size);
	}
	else {
		env_ptr->arch->remove_all_bpts();
	}

	for (;;) {
		read_packet(env_ptr->packet.buf, sizeof(env_ptr->packet.buf));
		write_packet("+", 1);

		gdb_process_packet(env_ptr);

		if (env_ptr->cmd != GDBSTUB_SEND_REPLY) {
			if (env_ptr->cmd == GDBSTUB_DETACH) {
				env_ptr->arch->remove_all_bpts();
			}
			break;
		}

		write_packet(env_ptr->packet.buf, env_ptr->packet.size);
	}

	gdb_running = true;
	sched_wakeup(&gdbserver_thread->schedee);
}

static void *gdb_handle_interrupt(void *arg) {
	struct gdbstub_env env;
	struct gdb_arch arch;
	struct pollfd pfd;

	pfd.fd = gdb_fd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	gdb_arch_init(&arch);
	env.arch = &arch;
	env_ptr = &env;

	for (;;) {
		SCHED_WAIT(gdb_running);

		if (-1 == poll(&pfd, 1, -1)) {
			break;
		}

		sched_lock();
		{
			if (gdb_running) {
				gdb_running = false;
				gdb_interrupted = true;
				env_ptr->arch->insert_bpt(sched_finish_switch, 0);
			}
		}
		sched_unlock();
	}
	return 0;
}

int main(int argc, char *argv[]) {
	const struct cmd *cmd;
	int ret;

	if (argc < 3) {
		ret = -EINVAL;
		goto out;
	}

	cmd = cmd_lookup(argv[2]);
	if (cmd == NULL) {
		ret = -ENOENT;
		goto out;
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

	gdbserver_thread = thread_create(0, gdb_handle_interrupt, NULL);
	gdb_arch_prepare(gdb_handle_debug_excpt);

	while (!env_ptr) {
	}

	env_ptr->arch->insert_bpt(cmd->exec, 0);
	cmd_exec(cmd, argc - 2, &argv[2]);

	gdb_arch_cleanup();
	thread_delete(gdbserver_thread);

out:
	if (ret == -EINVAL) {
		printf("Usage: %s [HOST]:[PORT] [PROG] [PROG_ARGS ...]\n", argv[0]);
		printf("       %s [TTY_DEV] [PROG] [PROG_ARGS ...]\n", argv[0]);
	}
	close(gdb_fd);
	return ret;
}
