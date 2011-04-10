/**
 * @file
 * @brief tty action lib
 *
 * @date 07.12.2010
 * @author Fedor Burdun
 */

#ifndef TTY_ACTION_H_
#define TTY_ACTION_H_

#include <drivers/tty.h>

static const struct vt_token TOKEN_UP[1] = {{
	.action = VT_ACTION_CSI_DISPATCH,
	.ch = 'A'
}};

static const struct vt_token TOKEN_DOWN[1] = {{
	.action = VT_ACTION_CSI_DISPATCH,
	.ch = 'B'
}};

static const struct vt_token TOKEN_RIGHT[1] = {{
	.action = VT_ACTION_CSI_DISPATCH,
	.ch = 'C'
}};

static const struct vt_token TOKEN_LEFT[1] = {{
	.action = VT_ACTION_CSI_DISPATCH,
	.ch = 'D'
}};

extern struct vt_token char_token(char ch);

#if 0
/* auxilary functions */ /* inner for tty */
/* TODO may be remove from header file */
extern void tty_write_space(tty_device_t *tty, uint32_t cnt);
extern void tty_go_left(tty_device_t *tty, uint32_t cnt);
extern void tty_go_right(tty_device_t *tty, uint32_t cnt);
extern void tty_go_cursor_position(tty_device_t *tty,
		uint32_t cur_before, uint32_t cur_after);
extern void tty_clean_line(tty_device_t *tty,
		uint32_t cur_before, uint32_t length, uint32_t cur_after);
extern void tty_write_line(tty_device_t *tty, uint8_t *line,
		uint32_t size, uint32_t cur_after);
extern void tty_rewrite_line(tty_device_t *tty, uint8_t *line, uint32_t size,
		uint32_t old_size, uint32_t cur_before, uint32_t cur_after);

extern void copy_forward(uint8_t *s, uint8_t *d, uint32_t size);
extern void copy_backward(uint8_t *s, uint8_t *d, uint32_t size);
#endif

/* tac -- tty action */ /* outer interface */
extern void tac_key_alpha(tty_device_t *tty, struct vt_token *token);
extern void tac_key_backspace(tty_device_t *tty);
extern void tac_key_del(tty_device_t *tty);
extern void tac_key_left(tty_device_t *tty);
extern void tac_key_right(tty_device_t *tty);
extern void tac_key_home(tty_device_t *tty);
extern void tac_key_end(tty_device_t *tty);
extern void tac_key_ins(tty_device_t *tty);
extern void tac_key_enter(tty_device_t *tty, struct vt_token *token);
extern void tac_remove_word(tty_device_t *tty);
extern void tac_remove_line(tty_device_t *tty);
extern void tac_switch_console(tty_device_t *tty, uint32_t c_after);

extern void tac_goto00(tty_device_t *tty);
extern void tac_clear(tty_device_t *tty);
extern void tac_reprint(tty_device_t *tty);

/* functions for users */
extern void console_putchar(char ch);
extern char console_getchar(void);
extern void console_clear(void);
extern void console_gotoXY(uint8_t x, uint8_t y);
extern void console_reprint(void);

#endif /* TTY_ACTION_H_ */

