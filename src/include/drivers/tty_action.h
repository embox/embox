/**
 * @file
 *
 * @brief tty action lib
 * @date 07.12.2010
 * @author Fedor Burdun
 */

#ifndef _TTY_ACTION_H_
#define _TTY_ACTION_H_

#include <drivers/tty.h>

static const struct vt_token TOKEN_LEFT[1] = {{
    .action = VT_ACTION_CSI_DISPATCH,
    .ch = 'D'
}};

static const struct vt_token TOKEN_RIGHT[1] = {{
    .action = VT_ACTION_CSI_DISPATCH,
    .ch = 'C'
}};


extern void tty_write_space( tty_device_t *tty, uint32_t cnt );
extern void tty_go_left( tty_device_t *tty, uint32_t cnt );
extern void tty_go_right( tty_device_t *tty, uint32_t cnt );
extern void tty_go_cursor_position( tty_device_t *tty, uint32_t cur_before,  uint32_t cur_after );
extern void tty_clean_line( tty_device_t *tty, uint32_t cur_before, uint32_t length, uint32_t cur_after );
extern void tty_write_line( tty_device_t *tty, uint8_t *line, uint32_t size, uint32_t cur_after );
extern void tty_rewrite_line( tty_device_t *tty, uint8_t *line, uint32_t size, uint32_t old_size, uint32_t cur_before, uint32_t cur_after );

extern void copy_forward( uint8_t *s, uint8_t *d, uint32_t size );
extern void copy_backward( uint8_t *s, uint8_t *d, uint32_t size );

#endif /* _TTY_ACTION_H_ */
