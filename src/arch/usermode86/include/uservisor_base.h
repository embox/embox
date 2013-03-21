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

#define UV_IRQ 20

#ifdef EMVISOR
typedef pid_t host_pid_t;
#endif

enum emvisor_msg {
	EMVISOR_BUDDY_PID,
	EMVISOR_DIAG_OUT,
	EMVISOR_DIAG_IN,
	EMVISOR_TIMER_SET,
	EMVISOR_EOF_IRQ,
	EMVISOR_IRQ,
	EMVISOR_IRQ_DIAG_IN,
	EMVISOR_IRQ_TMR,
};

struct emvisor_msghdr {
	enum emvisor_msg type;
	int dlen;
};

struct emvisor_tmrset {
	int count_fq;
	int overfl_fq;
};

extern int emvisor_send(int fd, enum emvisor_msg type, const void *msg_data,
	       	int dlen);

extern int emvisor_sendirq(host_pid_t pid, char signal, int fd,
		enum emvisor_msg type, const void *msg_data, int dlen);

extern int emvisor_recv(int fd, struct emvisor_msghdr *msg, void *data, int dlen);

extern int emvisor_recvmsg(int fd, struct emvisor_msghdr *msg);

extern int emvisor_recvnbody(int fd, void *data, int dlen);

extern int emvisor_recvbody(int fd, const struct emvisor_msghdr *msg, void *data, int dlen);

extern int emvisor_recvn(int fd, void *data, int dlen);

#endif /* USERVISOR_BASE_H_ */

