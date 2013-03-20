/**
 * @file
 * @brief
 *
 * @date 07.02.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_IODEV_H_
#define DRIVERS_IODEV_H_

#include <drivers/video_term.h>

struct iodev_ops {
	int (*init)(void);
	char (*getc)(void);
	void (*putc)(char ch);
	int (*kbhit)(void);
};

extern struct vterm diag_vterm;

extern const struct iodev_ops *const iodev_diag_ops;
extern const struct iodev_ops *const iodev_video_ops;
extern struct vga_vterm_video diag_vga;

extern void iodev_setup(const struct iodev_ops *iodev);
extern struct iodev_ops const *iodev_current(void);
//extern int iodev_setup_diag(void);
extern int iodev_setup_video(void);

extern int iodev_init(void);
extern char iodev_getc(void);
extern void iodev_putc(char ch);
extern int iodev_kbhit(void);

#endif /* DRIVERS_IODEV_H_ */
