/*
 * ttyx.h
 *
 *  Created on: 03.08.2011
 *      Author: Gerald Hoch
 */

#ifndef TTYX_H_
#define TTYX_H_

#define TTY_IN_BYTES     256	/* tty input queue size */
#define TAB_SIZE           8	/* distance between tab stops */
#define TAB_MASK           7	/* mask to compute a tab stop position */

#define ESC             '\33'	/* escape */
#define BYTE 0xff

//#define O_NOCTTY       00400	/* from <fcntl.h>, or cc will choke */
//#define O_NONBLOCK     04000
struct tty;
typedef int 	(*devfun_t)		(struct tty *tp);
typedef void 	(*devfunarg_t)	(struct tty *tp, int c);

/* Type definitions. */
typedef unsigned int vir_clicks; /* virtual  addresses and lengths in clicks */
typedef unsigned long phys_bytes;/* physical addresses and lengths in bytes */
typedef unsigned int phys_clicks;/* physical addresses and lengths in clicks */

/* Global variables used by the console driver and assembly support. */
//typedef unsigned int vir_bytes;	/* virtual addresses and lengths in bytes */
typedef char* vir_bytes;	/* virtual addresses and lengths in bytes */

typedef struct tty {
  int tty_events;		/* set when TTY should inspect this line */
  int tty_index;		/* index into TTY table */

  /* Input queue.  Typed characters are stored here until read by a program. */
  u16_t 	*tty_inhead;		/* pointer to place where next char goes */
  u16_t 	*tty_intail;		/* pointer to next char to be given to prog */
  int 		tty_incount;		/* # chars in the input queue */
  int 		tty_eotct;		/* number of "line breaks" in input queue */
  int 		tty_min;			/* minimum requested #chars in input queue */
  //clock_t 	tty_time;		/* time when the input is available */
  struct tty *tty_timenext;	/* for a list of ttys with active timers */

  devfun_t 		tty_devread;		/* routine to read from low level buffers */
  devfun_t 		tty_icancel;		/* cancel any device input */
  devfun_t 		tty_devwrite;	/* routine to start actual device output */
  devfunarg_t 	tty_echo;		/* routine to echo characters input */
  devfun_t 		tty_ocancel;		/* cancel any ongoing device output */
  devfun_t 		tty_break;		/* let the device send a break */
  devfun_t 		tty_ioctl;		/* set line speed, etc. at the device level */
  devfun_t 		tty_close;		/* tell the device that the tty is closed */

  /* Terminal parameters and status. */
  int 	tty_position;		/* current position on the screen for echoing */
  char 	tty_reprint;		/* 1 when echoed input messed up, else 0 */
  char 	tty_escaped;		/* 1 when LNEXT (^V) just seen, else 0 */
  char 	tty_inhibited;		/* 1 when STOP (^S) just seen (stops output) */

  /* Information about incomplete I/O requests is stored here. */
  vir_bytes tty_in_vir;		/* virtual address where data is to go */
  int 	tty_inleft;		/* how many chars are still needed */
  int 	tty_incum;		/* # chars input so far */
  vir_bytes tty_out_vir;	/* virtual address where data comes from */
  int 	tty_outleft;		/* # chars yet to be output */
  int 	tty_outcum;		/* # chars output so far */
  vir_bytes tty_iovir;		/* virtual address of ioctl buffer */

  /* Miscellaneous. */
  void *tty_priv;		/* pointer to per device private data */
  struct termios tty_termios;	/* terminal attributes */
  struct winsize tty_winsize;	/* window size (#lines and #columns) */
  u16_t tty_inbuf[TTY_IN_BYTES];/* tty input buffer */
} tty_t;

/* Address of a tty structure. */
#define tty_addr(line)	(&tty_table[line])

/* Macros for magic tty types. */
#define isconsole(tp)	((tp) < tty_addr(NR_CONS))

/* Macros for magic tty structure pointers. */
#define FIRST_TTY	tty_addr(0)
#define END_TTY		tty_addr(sizeof(tty_table) / sizeof(tty_table[0]))

/* A device exists if at least its 'devread' function is defined. */
#define tty_active(tp)	((tp)->tty_devread != NULL)

extern tty_t tty_table[];
extern int tty_ccurrent;		/* currently visible console */


/* Values for the fields. */
#define NOT_ESCAPED        0	/* previous character is not LNEXT (^V) */
#define ESCAPED            1	/* previous character was LNEXT (^V) */
#define RUNNING            0	/* no STOP (^S) has been typed to stop output */
#define STOPPED            1	/* STOP (^S) has been typed to stop output */

/* Fields and flags on characters in the input queue. */
#define IN_CHAR       0x00FF	/* low 8 bits are the character itself */
#define IN_LEN        0x0F00	/* length of char if it has been echoed */
#define IN_LSHIFT          8	/* length = (c & IN_LEN) >> IN_LSHIFT */
#define IN_EOT        0x1000	/* char is a line break (^D, LF) */
#define IN_EOF        0x2000	/* char is EOF (^D), do not return to user */
#define IN_ESC        0x4000	/* escaped by LNEXT (^V), no interpretation */

/* Times and timeouts. */
#define TIME_NEVER	((clock_t) -1 < 0 ? (clock_t) LONG_MAX : (clock_t) -1)
#define force_timeout()	((void) (tty_timeout = 0))

extern tty_t tty_table[];

/* Number of elements and limit of a buffer. */
#define buflen(buf)	(sizeof(buf) / sizeof((buf)[0]))
#define bufend(buf)	((buf) + buflen(buf))

/* tty.c */
 extern int 	tty_in_process		(register tty_t *tp, char *buf, int count);
 extern void 	tty_select_console	(int cons_line);
 extern int 	tty_select_retry	(struct tty *tp);

extern void 	vid_vid_copy(unsigned src, unsigned dst, unsigned count);
extern void 	mem_vid_copy(u16_t *src, unsigned dst, unsigned count);
extern void 	phys_copy(phys_bytes source, phys_bytes dest,phys_bytes count);

#if 0

FORWARD _PROTOTYPE( void do_cancel, (tty_t *tp, message *m_ptr)		);
FORWARD _PROTOTYPE( void do_ioctl, (tty_t *tp, message *m_ptr)		);
FORWARD _PROTOTYPE( void do_open, (tty_t *tp, message *m_ptr)		);
FORWARD _PROTOTYPE( void do_close, (tty_t *tp, message *m_ptr)		);
FORWARD _PROTOTYPE( void do_read, (tty_t *tp, message *m_ptr)		);
FORWARD _PROTOTYPE( void do_write, (tty_t *tp, message *m_ptr)		);

#endif

#endif /* TTYX_H_ */
