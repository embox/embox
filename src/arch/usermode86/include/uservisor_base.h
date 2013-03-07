/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.03.2013
 */

#ifndef USERVISOR_BASE_H_
#define USERVISOR_BASE_H_

#define UV_PRDDOWNSTRM 0
#define UV_PWRUPSTRM 1
#define UV_PWRDOWNSTRM 3

#define UV_IRQ 10

#ifdef EMVISOR
typedef pid_t host_pid_t;
#endif

enum emvisor_msg {
	EMVISOR_DIAG_OUT = 0,
	EMVISOR_DIAG_IN,
	EMVISOR_IRQ,
	EMVISOR_IRQ_DIAG_IN,
};

struct emvisor_msghdr {
	enum emvisor_msg type;
	int dlen;
};

extern int emvisor_send(int fd, enum emvisor_msg type, const void *msg_data, int dlen);

extern int emvisor_sendirq(host_pid_t pid, int fd, enum emvisor_msg type,
		const void *msg_data, int dlen);

extern int emvisor_recv(int fd, struct emvisor_msghdr *msg, void *data, int dlen);

extern int emvisor_recvmsg(int fd, struct emvisor_msghdr *msg);

extern int emvisor_recvbody(int fd, const struct emvisor_msghdr *msg, void *data, int dlen);

#endif /* USERVISOR_BASE_H_ */

