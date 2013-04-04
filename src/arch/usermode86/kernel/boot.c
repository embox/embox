/**
 * @file
 * @brief <CURSOR>
 *
 * @author  Anton Kozlov
 * @date    09.03.2013
 */

#include <assert.h>
#include <string.h>
#include <kernel/host.h>

extern void kernel_start(void);
extern void ipl_hnd(int signal);

static char *pipeopen(char *argv, int pipe, int flags) {
	int fd;

	fd = host_open(argv, flags, 0);
	assert(fd >= 0);

	assert(0 <= host_dup2(fd, pipe));

	return argv + strlen(argv) + 1;
}

void _start2(int argc, char *argv) {
	host_pid_t thispid = host_getpid();

	assert(argc == 1 + 2);

	argv += strlen(argv) + 1;

	argv = pipeopen(argv, UV_PRDDOWNSTRM, HOST_O_RDWR|HOST_O_NONBLOCK);
	host_dup2(UV_PRDDOWNSTRM, UV_PWRDOWNSTRM);
	argv = pipeopen(argv, UV_PWRUPSTRM, HOST_O_WRONLY);

	host_signal(UV_IRQ, ipl_hnd);

	emvisor_send(UV_PWRUPSTRM, EMVISOR_BUDDY_PID, &thispid,
			sizeof(thispid));

}

