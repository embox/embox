/**
 * @file
 *
 * @date 03.08.2011
 * @author Gerald Hoch
 */

#ifndef TTY_H_
#define TTY_H_

#define TTY_IN_BYTES     256	/* tty input queue size */
#define TAB_SIZE           8	/* distance between tab stops */
#define TAB_MASK           7	/* mask to compute a tab stop position */

#define ESC             '\33'	/* escape */

#define O_NOCTTY       00400	/* from <fcntl.h>, or cc will choke */
#define O_NONBLOCK     04000

struct tty;

typedef void (*devfun_t) 	(struct tty *tp) ;
typedef void (*devfunarg_t) (struct tty *tp, int c) ;

typedef struct tty {
  int tty_events;		/* set when TTY should inspect this line */

  /* Input queue.  Typed characters are stored here until read by a program. */
  u16_t *tty_inhead;		/* pointer to place where next char goes */
  u16_t *tty_intail;		/* pointer to next char to be given to prog */
  int tty_incount;		/* # chars in the input queue */
  int tty_eotct;		/* number of "line breaks" in input queue */
  devfun_t tty_devread;		/* routine to read from low level buffers */
  devfun_t tty_icancel;		/* cancel any device input */
  int tty_min;			/* minimum requested #chars in input queue */
  clock_t tty_time;		/* time when the input is available */
  struct tty *tty_timenext;	/* for a list of ttys with active timers */

  /* Output section. */
  devfun_t tty_devwrite;	/* routine to start actual device output */
  devfunarg_t tty_echo;		/* routine to echo characters input */
  devfun_t tty_ocancel;		/* cancel any ongoing device output */
  devfun_t tty_break;		/* let the device send a break */

  /* Terminal parameters and status. */
  int tty_position;		/* current position on the screen for echoing */
  char tty_reprint;		/* 1 when echoed input messed up, else 0 */
  char tty_escaped;		/* 1 when LNEXT (^V) just seen, else 0 */
  char tty_inhibited;		/* 1 when STOP (^S) just seen (stops output) */
  char tty_pgrp;		/* slot number of controlling process */
  char tty_openct;		/* count of number of opens of this tty */

  /* Information about incomplete I/O requests is stored here. */
  char tty_inrepcode;		/* reply code, TASK_REPLY or REVIVE */
  char tty_incaller;		/* process that made the call (usually FS) */
  char tty_inproc;		/* process that wants to read from tty */
  vir_bytes tty_in_vir;		/* virtual address where data is to go */
  int tty_inleft;		/* how many chars are still needed */
  int tty_incum;		/* # chars input so far */
  char tty_outrepcode;		/* reply code, TASK_REPLY or REVIVE */
  char tty_outcaller;		/* process that made the call (usually FS) */
  char tty_outproc;		/* process that wants to write to tty */
  vir_bytes tty_out_vir;	/* virtual address where data comes from */
  int tty_outleft;		/* # chars yet to be output */
  int tty_outcum;		/* # chars output so far */
  char tty_iocaller;		/* process that made the call (usually FS) */
  char tty_ioproc;		/* process that wants to do an ioctl */
  int tty_ioreq;		/* ioctl request code */
  vir_bytes tty_iovir;		/* virtual address of ioctl buffer */

  /* Miscellaneous. */
  devfun_t tty_ioctl;		/* set line speed, etc. at the device level */
  devfun_t tty_close;		/* tell the device that the tty is closed */
  void *tty_priv;		/* pointer to per device private data */
  struct termios tty_termios;	/* terminal attributes */
  struct winsize tty_winsize;	/* window size (#lines and #columns) */

  u16_t tty_inbuf[TTY_IN_BYTES];/* tty input buffer */
} tty_t;

EXTERN tty_t tty_table[CONFIG_NR_CONS + CONFIG_NR_RS_LINES + CONFIG_NR_PTYS];
EXTERN int current;				/* currently visible console */

/* Address of a tty structure. */
#define tty_addr(line)	(&tty_table[line])
/* Macros for magic tty types. */
#define isconsole(tp)	((tp) < tty_addr(CONFIG_NR_CONS))

/* Macros for magic tty structure pointers. */
#define FIRST_TTY	tty_addr(0)
#define END_TTY		tty_addr(sizeof(tty_table) / sizeof(tty_table[0]))

/* A device exists if at least its 'devread' function is defined. */
#define tty_active(tp)	((tp)->tty_devread != NULL)


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

EXTERN tty_t *tty_timelist;	/* list of ttys with active timers */

/* Number of elements and limit of a buffer. */
#define buflen(buf)	(sizeof(buf) / sizeof((buf)[0]))
#define bufend(buf)	((buf) + buflen(buf))

EXTERN int in_process		(register tty_t *tp, char *buf, int count);
EXTERN void select_console	(int cons_line);

#endif /* TTY_H_ */
