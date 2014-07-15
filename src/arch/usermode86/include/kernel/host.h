/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.03.2013
 */

#ifndef ARCH_USERMODE86_HOST_H_
#define ARCH_USERMODE86_HOST_H_

#define HOST_TIMER_IRQ 14
#define HOST_NET_IRQ 29

extern int host_write(int fd, const void *buf, int c);
extern int host_read(int fd, void *buf, int c);

extern int host_pause(void);

extern void host_ipl_init(void);
extern int host_ipl_save(void);
extern void host_ipl_restore(int ipl);

extern void host_signal_send_self(int sig_nr);

extern void host_timer_config(int usec);

struct host_net_adp {
	int fd;
};

enum host_net_op {
	HOST_NET_INIT,
	HOST_NET_START,
	HOST_NET_STOP,
};

extern int host_net_cfg(struct host_net_adp *hnet, enum host_net_op op);
extern int host_net_rx_count(struct host_net_adp *hnet);
extern int host_net_rx(struct host_net_adp *hnet, void *buf, int len);
extern void host_net_tx(struct host_net_adp *hnet, const void *buf, int len);

#define HOST_JMPBUF_LEN 156
#define HOST_CTX_LEN 1024

extern int host_getcontext(void *ucp);
extern void host_makecontext(void *ucp, void (*func)(), int argc);
extern int host_swapcontext(void *oucp, void *ucp);

extern void host_stackcontext(void *ucp, void *sp, int size);

#endif /* ARCH_USERMODE86_HOST_H_ */
