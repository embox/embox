/**
 * @file
 *
 * @date 03.08.2011
 * @author Gerald Hoch
 */

#ifndef TTY_H_
#define TTY_H_

/**
 * DEVICDE config
 */
/* First minor numbers for the various classes of TTY devices. */
#define CONS_MINOR	  0
#define LOG_MINOR	 15
#define RS232_MINOR	 16
#define TTYPX_MINOR	128
#define PTYPX_MINOR	192


#ifndef CONFIG_NR_CONS
#define CONFIG_NR_CONS 1
#endif
#ifndef CONFIG_NR_RS_LINES
#define CONFIG_NR_RS_LINES 0
#endif
#ifndef CONFIG_NR_PTYS
#define CONFIG_NR_PTYS 0
#endif

#define USE_BEEP 0				/* use the Beep or not */
#define FUNC_KEY_USED 0			/* use function keys */

#define TTY_IN_BYTES     256	/* tty input queue size */
#define TAB_SIZE           8	/* distance between tab stops */
#define TAB_MASK           7	/* mask to compute a tab stop position */

#define ESC             '\33'	/* escape */

#define REVIVE	 	  67	/* to FS: revive a sleeping process */
#define TASK_REPLY	  68	/* to FS: reply code from tty task */


#define O_NOCTTY       00400	/* from <fcntl.h>, or cc will choke */
#define O_NONBLOCK     04000

struct tty;

typedef void (*devfun_t) 	(struct tty *tp) ;
typedef void (*devfunarg_t) (struct tty *tp, int c) ;

typedef struct tty {
  int tty_events;		/* set when TTY should inspect this line */
  int	tty_instance;	/* instance of console,rs232 or pty */
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
  struct thread * tty_incaller;		/* process that made the call (usually FS) */
  struct thread * tty_inproc;		/* process that wants to read from tty */
  vir_bytes tty_in_vir;		/* virtual address where data is to go */
  int tty_inleft;		/* how many chars are still needed */
  int tty_incum;		/* # chars input so far */
  char tty_outrepcode;		/* reply code, TASK_REPLY or REVIVE */
  struct thread * tty_outcaller;		/* process that made the call (usually FS) */
  struct thread * tty_outproc;		/* process that wants to write to tty */
  vir_bytes tty_out_vir;	/* virtual address where data comes from */
  int tty_outleft;		/* # chars yet to be output */
  int tty_outcum;		/* # chars output so far */
  struct thread * tty_iocaller;		/* process that made the call (usually FS) */
  struct thread * tty_ioproc;		/* process that wants to do an ioctl */
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

EXTERN int	 	in_process		(register tty_t *tp, char *buf, int count);
EXTERN void 	select_console	(int cons_line);
EXTERN void 	tty_reply		(int code, struct thread *replyee, struct thread *proc_nr, int status);
EXTERN tty_t* 	getTTY			(int line);
EXTERN tty_t* 	getTTY_Cons		(int line);
EXTERN tty_t* 	getTTY_Log		(int line);
EXTERN tty_t* 	getTTY_Rs232	(int line);
EXTERN tty_t* 	getTTY_TTYPX	(int line);
EXTERN tty_t* 	getTTY_PTYPX	(int line);

//#####################################################################
// Message Types, defines
//#####################################################################
typedef struct {int m2i1, m2i2, m2i3; long m2l1, m2l2; char *m2p1;struct thread * m2p2;} mess_2;
typedef struct tty_msg {
	struct thread *  m_source;			/* who sent the message */
	int m_type;			/* what kind of message is it */
	union {
		mess_2 m_m2;
	} m_u;
} tty_msg_t;

#define m2_i1  m_u.m_m2.m2i1
#define m2_i2  m_u.m_m2.m2i2
#define m2_i3  m_u.m_m2.m2i3
#define m2_l1  m_u.m_m2.m2l1
#define m2_l2  m_u.m_m2.m2l2
#define m2_p1  m_u.m_m2.m2p1
#define m2_p2  m_u.m_m2.m2p2

/* Names of message fields used for messages to block and character tasks. */
#define DEVICE         m2_i1	/* major-minor device */
#define PROC_NR        m2_p2	/* which (proc) wants I/O? */
#define COUNT          m2_i3	/* how many bytes to transfer */
#define REQUEST        m2_i3	/* ioctl request code */
#define POSITION       m2_l1	/* file offset */
#define ADDRESS        m2_p1	/* core buffer address */

/* Names of messages fields used in reply messages from tasks. */
#define REP_PROC_NR    m2_i1	/* # of proc on whose behalf I/O was done */
#define REP_STATUS     m2_i2	/* bytes transferred or error number */

/* Names of message fields for messages to TTY task. */
#define TTY_LINE       DEVICE	/* message parameter: terminal line */
#define TTY_REQUEST    COUNT	/* message parameter: ioctl request code */
#define TTY_SPEK       POSITION	/* message parameter: ioctl speed, erasing */
#define TTY_FLAGS      m2_l2	/* message parameter: ioctl tty mode */
#define TTY_PGRP       m2_i3	/* message parameter: process group */

// possible m_type values
#define CANCEL       0	/* general req to force a task to cancel */
#define HARD_INT     2	/* fcn code for all hardware interrupts */
#define DEV_READ	    3	/* fcn code for reading from tty */
#define DEV_WRITE    4	/* fcn code for writing to tty */
#define DEV_IOCTL    5	/* fcn code for ioctl */
#define DEV_OPEN     6	/* fcn code for opening tty */
#define DEV_CLOSE    7	/* fcn code for closing tty */
#define DEV_SCATTER  8	/* fcn code for writing from a vector */
#define DEV_GATHER   9	/* fcn code for reading into a vector */
#define TTY_SETPGRP 10	/* fcn code for setpgroup */
#define TTY_EXIT	   11	/* a process group leader has exited */

#endif /* TTY_H_ */
