/**
 * @file
 * @brief VGA framebuffer driver
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */

#ifndef VIDEO_VGA_H_
#define VIDEO_VGA_H_

#include <kernel/file.h>

typedef struct vchar {
	char c;
	char a;
} __attribute__((packed)) vchar_t ;

typedef struct vga_console {
	unsigned width, height;
	unsigned x, y;
	char     attr;
	int      esc_args[5];
	unsigned num_esc_args;
	int esc_state;
	volatile vchar_t *const video;
	file_operations_t file_op;
} vga_console_t;

/* The video memory address. */
#define VIDEO          0xB8000

extern void vga_console_init(vga_console_t *con, unsigned width, unsigned height);

/**
 * Put the character C on the screen.
 */
extern void vga_putc(vga_console_t *con, char c);


extern vga_console_t *vga_console_diag(void);
#if 0
/**
 * Get the character from the screen.
 */
extern int vga_putc(void);
#endif

#endif /* VIDEO_VGA_H_ */
