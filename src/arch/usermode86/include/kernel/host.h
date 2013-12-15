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

extern int host_putchar(int c);
extern int host_getchar(void);

extern int host_pause(void);

extern void host_ipl_init(void);
extern int host_ipl_save(void);
extern void host_ipl_restore(int ipl);

extern void host_signal_send_self(int sig_nr);

extern void host_timer_config(int usec);

#endif /* ARCH_USERMODE86_HOST_H_ */
