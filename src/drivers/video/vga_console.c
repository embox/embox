/**
 * @file
 * @brief VGA console driver
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */
#include <drivers/vga.h>
#include <asm/io.h>

typedef struct vga_console {
	unsigned width, height;
	unsigned x, y;
	char     attr;
	int      esc_args[5];
	unsigned num_esc_args;
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
	keyboard_init();
}

static void vga_scroll(unsigned n) {
	size_t i;
	if (n && n <= con.height) {
		for (i = n * con.width; i < con.width * con.height; ++i) {
			video[i - n * con.width] = video[i];
		}
		for (i = (con.height - n) * con.width;
				i < con.width * con.height; ++i) {
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

static void vga_printchar(unsigned x, unsigned y,
		unsigned char c, unsigned char a) {
	video[x + (y * con.width)] = (vchar_t) { c: c, a: a };
}

static int esc_state = 0;

void ansi_attrib(int a) {
	char const colors[] = {0, 4, 2, 6, 1, 5, 3, 7};
	switch(a) {
	case 0:
		con.attr = 0x07;
		break;
	case 1:
		con.attr |= 0x0808;
		break;
	case 22:
		con.attr &= ~0x0808;
		break;
	case 5:
		con.attr |= 0x8080;
	default:
		if (30 <= a && a <= 37) {
			con.attr = (con.attr & 0x0f0) | colors[a - 30] |
					((con.attr >> 8) & 0x08);
		} else if (40 <= a && a <= 47) {
			con.attr = (con.attr & 0x0f) | (colors[a - 40] << 4) |
					((con.attr >> 8) & 0x80);
		}
		break;
	};
}

static void blink_cursor(unsigned x, unsigned y) {
	unsigned pos = 80 * y + x;

	out16((pos & 0xff00) | 0x0e, dev_reg);
	out16((pos <<     8) | 0x0f, dev_reg);
}

static void vga_esc_putc(char c) {
	int p;
	if (c == '[') {
		con.esc_args[0] = 0;
		con.esc_args[1] = 0;
		con.num_esc_args = 0;
		return;
	}
	if (c == ';') {
		con.esc_args[++con.num_esc_args] = 0;
	} else if (isdigit(c) && con.num_esc_args < 5) {
		con.esc_args[con.num_esc_args] = con.esc_args[con.num_esc_args] * 10  + (c - '0');
	}
	switch (c) {
	case 'f':
		con.x = con.esc_args[1];
		if (con.x)
			--con.y;
		con.y = con.esc_args[0];
		if (con.y)
			--con.x;
		blink_cursor(con.x, con.y);
		break;
	case 'H':
		con.x = con.esc_args[1];
		if(con.x) --con.x;
		con.y = con.esc_args[0];
		if(con.y) --con.y;
		if (con.x >= con.width) con.x = con.width  - 1;
		if (con.y >= con.height) con.y = con.height - 1;
		break;
	case 'm': /* color */
		p = 0;
		do {
			ansi_attrib(con.esc_args[p++]);
		} while (con.num_esc_args--);
		esc_state = 0;
		break;
	}
}

void vga_putc(char c) {
	size_t i;
	if (esc_state == 1) {
		vga_esc_putc(c);
		return;
	}
	switch(c) {
	case '\n':
	case '\f':
		con.x = 0;
		con.y++;
		if (con.y >= con.height) {
			vga_scroll(con.y - con.height + 1);
			con.y = con.height - 1;
		}
		break;
	case '\r':
		con.x = 0;
		break;
	case '\t':
		con.x = (con.x & ~7) + 8;
	case 27: /* ESC */
		esc_state = 1;
		return;
	case 6: /* cursor */
		con.y = con.y + 1;
		con.x = con.x + 2;
		if (con.y >= con.height) {
			con.y = con.height - 1;
		}
		if (con.x >= con.width) {
			con.x = con.width - 1;
		}
		break;
	case 1: /* home */
		con.x = 0;
		con.y = 0;
		break;
	case 5: /* clear to end of line */
		for (i = 0; i < con.width - con.x; ++i) {
			vga_printchar(con.x, con.y * con.width + i, 0x20, con.attr);
		}
		break;
	case 8: /* back space */
		if (con.x)
			--con.x;
		break;
	default:
		if ((unsigned) c >= 32) {
			if (con.x >= con.width) {
				++con.y;
				con.x = 0;
			}
			if (con.y >= con.height) {
				vga_scroll(con.y - con.height + 1);
				con.y = con.height - 1;
			}
			if (con.x < con.width) {
				vga_printchar(con.x, con.y, (c == '\265' ? '\346' : c), con.attr);
			}
			++con.x;
		}
		break;
	}
}

int vga_getc(void) {
	//TODO: workaround
	//blink_cursor(con.x, con.y);
	extern int keyboard_getchar(void);
	return keyboard_getchar();
	//return 0;
}

void diag_init(void) {

}

void diag_putc(int c) {
	vga_putc((char) c);
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

