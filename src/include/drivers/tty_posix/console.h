/**
 * @file
 *
 * @date 03.08.2011
 * @author Gerald Hoch
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

/* Definitions used by the console driver. */
#define MONO_BASE    0xB0000L	/* base of mono video memory */
#define COLOR_BASE   0xB8000L	/* base of color video memory */
#define MONO_SIZE     0x1000	/* 4K mono video memory */
#define COLOR_SIZE    0x4000	/* 16K color video memory */
#define EGA_SIZE      0x8000	/* EGA & VGA have at least 32K */
#define BLANK_COLOR   0x0700	/* determines cursor color on blank screen */
#define SCROLL_UP          0	/* scroll forward */
#define SCROLL_DOWN        1	/* scroll backward */
#define BLANK_MEM ((u16_t *) 0)	/* tells mem_vid_copy() to blank the screen */
#define CONS_RAM_WORDS    80	/* video ram buffer size */
#define MAX_ESC_PARMS      4	/* number of escape sequence params allowed */

/* Constants relating to the controller chips. */
#define M_6845         0x3B4	/* port for 6845 mono */
#define C_6845         0x3D4	/* port for 6845 color */
#define INDEX              0	/* 6845's index register */
#define DATA               1	/* 6845's data register */
#define VID_ORG           12	/* 6845's origin register */
#define CURSOR            14	/* 6845's cursor register */

/* Beeper. */
#define BEEP_FREQ     0x0533	/* value to put into timer to set beep freq */
#define B_TIME		   3	/* length of CTRL-G beep is ticks */

/* definitions used for font management */
#define GA_SEQUENCER_INDEX	0x3C4
#define GA_SEQUENCER_DATA	0x3C5
#define GA_GRAPHICS_INDEX	0x3CE
#define GA_GRAPHICS_DATA	0x3CF
#define GA_VIDEO_ADDRESS	0xA0000L
#define GA_FONT_SIZE		8192

/* Global variables used by the console driver and assembly support. */
PUBLIC u16_t vid_seg;
PUBLIC vir_bytes vid_off;	/* video ram is found at vid_seg:vid_off */
PUBLIC unsigned vid_size;	/* 0x2000 for color or 0x0800 for mono */
PUBLIC unsigned vid_mask;	/* 0x1FFF for color or 0x07FF for mono */
PUBLIC unsigned blank_color = BLANK_COLOR; /* display code for blank */

/* Private variables used by the console driver. */
PRIVATE int vid_port;		/* I/O port for accessing 6845 */
PRIVATE int wrap;		/* hardware can wrap? */
PRIVATE int softscroll;		/* 1 = software scrolling, 0 = hardware */
#if USE_BEEP
PRIVATE int beeping;		/* speaker is beeping? */
#endif
PRIVATE unsigned font_lines;	/* font lines per character */
PRIVATE unsigned scr_width;	/* # characters on a line */
PRIVATE unsigned scr_lines;	/* # lines on the screen */
PRIVATE unsigned scr_size;	/* # characters on the screen */

typedef struct crtc_regs {
	uint32_t addr;
	uint32_t data;
} crtc_regs_t;

/* Per console data. */
typedef struct console {
  tty_t *c_tty;			/* associated TTY struct */
  int c_column;			/* current column number (0-origin) */
  int c_row;			/* current row (0 at top of screen) */
  int c_rwords;			/* number of WORDS (not bytes) in outqueue */
  unsigned c_start;		/* start of video memory of this console */
  unsigned c_limit;		/* limit of this console's video memory */
  unsigned c_org;		/* location in RAM where 6845 base points */
  unsigned c_cur;		/* current position of cursor in video RAM */
  unsigned c_attr;		/* character attribute */
  unsigned c_blank;		/* blank attribute */
  char c_reverse;		/* reverse video */
  char c_esc_state;		/* 0=normal, 1=ESC, 2=ESC[ */
  char c_esc_intro;		/* Distinguishing character following ESC */
  int *c_esc_parmp;		/* pointer to current escape parameter */
  int c_esc_parmv[MAX_ESC_PARMS];	/* list of escape parameters */
  u16_t c_ramqueue[CONS_RAM_WORDS];	/* buffer for video RAM */
  int cons_line;			/* index of console */
} console_t;

PRIVATE int nr_cons= 1;		/* actual number of consoles */
PRIVATE console_t cons_table[CONFIG_NR_CONS];
PRIVATE console_t *curcons;	/* currently visible */

/* Color if using a color controller. */
#define color	(vid_port == C_6845)

/* Map from ANSI colors to the attributes used by the PC */
PRIVATE int ansi_colors[8] = {0, 4, 2, 6, 1, 5, 3, 7};

/* Structure used for font management */
struct sequence {
	unsigned short index;
	unsigned char port;
	unsigned char value;
};

FORWARD _PROTOTYPE( void cons_write, (struct tty *tp)			);
FORWARD _PROTOTYPE( void cons_echo, (tty_t *tp, int c)			);
FORWARD _PROTOTYPE( void out_char, (console_t *cons, int c)		);
#if USE_BEEP
FORWARD _PROTOTYPE( void beep, (void)					);
FORWARD _PROTOTYPE( void stop_beep, (sys_timer_t *tmrp)				);
#endif
FORWARD _PROTOTYPE( void do_escape, (console_t *cons, int c)		);
FORWARD _PROTOTYPE( void flush, (console_t *cons)			);
FORWARD _PROTOTYPE( void parse_escape, (console_t *cons, int c)		);
FORWARD _PROTOTYPE( void scroll_screen, (console_t *cons, int dir)	);
FORWARD _PROTOTYPE( void set_6845, (int reg, int val)		);
FORWARD _PROTOTYPE( void cons_org0, (void)				);
#if 0
FORWARD _PROTOTYPE( void ga_program, (struct sequence *seq)		);
#endif
FORWARD _PROTOTYPE( void cons_ioctl, (tty_t *tp)			);


#endif /* CONSOLE_H_ */
