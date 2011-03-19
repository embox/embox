/**
 * @file
 * @brief Defines virtual console interface.
 * A virtual console is an instance of the tty device.
 *
 * @date 23.11.2010
 * @author Anton Bondarev
 */

#ifndef VCONSOLE_H_
#define VCONSOLE_H_

#ifndef CONFIG_SCR_BUF_S
#define CONFIG_SCR_BUFF_S (80 * 25)
#endif

#ifndef CONFIG_CMDLINE_S
#define CONFIG_CMDLINE_S 80
#endif

#ifndef CONFIG_ESH_STACK_S
#define CONFIG_ESH_STACK_S (4096 * 10)
#endif

struct tty_device;

typedef struct vconsole {
	volatile struct tty_device *tty;
	size_t   height;
	size_t   width;
	uint32_t mode;
	size_t   lines;
	size_t   columns;

	uint8_t  scr_buff[CONFIG_SCR_BUFF_S];
	uint8_t  scr_line;
	uint8_t  scr_column;

	uint32_t cl_cnt;
	uint32_t cl_cur;
	uint8_t  cl_buff[CONFIG_CMDLINE_S];

	uint8_t  esh_stack[CONFIG_ESH_STACK_S];
} vconsole_t;

extern vconsole_t const *sys_console;
extern vconsole_t *cur_console;

extern int vconsole_activate(vconsole_t *con);
extern int vconsole_deactivate(vconsole_t *con);

extern void vconsole_putchar(struct vconsole *vc, char ch);
extern char vconsole_getchar(struct vconsole *vc);
extern void vconsole_clear(struct vconsole *vc);
extern void vconsole_reprint(struct vconsole *vc);

#include <drivers/tty.h>

#endif /* VCONSOLE_H_ */
