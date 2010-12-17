/**
 * @file
 * @brief VGA console driver
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */
#include <drivers/vga.h>
#include <asm/regs.h>

typedef struct vga_console {
	unsigned width, height;
	unsigned x, y;
	char     attr;
} vga_console_t;

typedef struct vchar {
	char c;
	char a;
} vchar_t __attribute__((packed));

/**
 * The CRT Controller (CRTC) Registers are accessed via a pair of registers,
 * the CRTC Address Register and the CRTC Data Register.
 */
typedef struct crtc_regs {
	uint32_t addr;
	uint32_t data;
} crtc_regs_t;

static volatile struct crtc_regs *dev_reg;
static vga_console_t con;
/** Point to the video memory. */
static volatile vchar_t *video;

static int crtc_init(void) {
	dev_reg = 0x3b4;
	video = (unsigned char *) VIDEO;
}

static void vga_clear(void);

void vga_console_init(unsigned width, unsigned height) {
	con.width = width;
	con.height = height;
	con.x = 0;
	con.y = 0;
	con.attr = 0x7;
	crtc_init();
	vga_clear();
}

static void vga_scroll(unsigned n) {
	size_t i;
	if (n && n <= con.height) {
		for (i = n * con.width; i < con.width * con.height; ++i) {
			video[i - n * con.width] = video[i];
		}
		for(i= (con.height - n) * con.width; i < con.width * con.height; ++i) {
			video[i] = (vchar_t) { c: 0x20, a: con.attr };
		}
	}
}

static void vga_clear(void) {
	size_t i;
	for(i = 0; i < con.width * con.height; ++i) {
		video[i] = (vchar_t) { c: 0x20, a: con.attr };
	}
}

static void vga_printchar(unsigned x, unsigned y, unsigned char c, unsigned char a) {
	video[x + (y * con.width)] = (vchar_t) { c: c, a: a };
}

void vga_putc(int c) {
	if (c == '\n' || c == '\r') {
newline:
		con.x = 0;
		con.y++;
		if (con.y >= con.height) {
			con.y = 0;
		}
		return;
	}

	vga_printchar(con.x, con.y, c & 0xFF, 0x7);

	con.x++;
	if (con.x >= con.width) {
		goto newline;
	}
}

static void blink_cursor(unsigned x, unsigned y) {
	unsigned pos = 80 * y + x;

	out16((pos & 0xff00) | 0x0e, dev_reg);
	out16((pos <<     8) | 0x0f, dev_reg);
}

int vga_getc(void) {
        return 0;
}

void diag_init(void) {

}

void diag_putc(int c) {
	vga_putc(c);
}

int diag_getc(void) {
	return vga_getc();
}

/*TODO: temporary */
void uart_putc(int c) {
	vga_putc(c);
}

int uart_getc(void) {
	return vga_getc();
}

