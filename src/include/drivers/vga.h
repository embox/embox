/**
 * @file
 * @brief VGA framebuffer driver
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */
#ifndef VIDEO_VGA_H_
#define VIDEO_VGA_H_

/* The video memory address. */
#define VIDEO          0xB8000

extern void vga_console_init(unsigned width, unsigned height);

/**
 * Put the character C on the screen.
 */
//extern void vga_putc(char c);

/**
 * Get the character from the screen.
 */
extern int vga_getc(void);

#endif /* VIDEO_VGA_H_ */

