/**
 * @file
 * @brief VGA framebuffer driver
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */
#ifndef VIDEO_VGA_H_
#define VIDEO_VGA_H_

/* The number of columns. */
#define COLUMNS        80
/* The number of lines. */
#define LINES          24
/* The attribute of an character. */
#define ATTRIBUTE      7
/* The video memory address. */
#define VIDEO          0xB8000

/**
 * Clear the screen and initialize VIDEO, XPOS and YPOS.
 */
extern void vga_clear_screen(void);

/**
 * Put the character C on the screen.
 */
extern void vga_putc(int c);

/**
 * Get the character from the screen.
 */
extern int vga_getc(void);

#endif /* VIDEO_VGA_H_ */

