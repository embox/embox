/* This file contains the terminal driver, both for the IBM console and regular
 * ASCII terminals.  It handles only the device-independent part of a TTY, the
 * device dependent parts are in console.c, rs232.c, etc.  This file contains
 * two main entry points, tty_task() and tty_wakeup(), and several minor entry
 * points for use by the device-dependent code.
 *
 * The device-independent part accepts "keyboard" input from the device-
 * dependent part, performs input processing (special key interpretation),
 * and sends the input to a process reading from the TTY.  Output to a TTY
 * is sent to the device-dependent code for output processing and "screen"
 * display.  Input processing is done by the device by calling 'in_process'
 * on the input characters, output processing may be done by the device itself
 * or by calling 'out_process'.  The TTY takes care of input queuing, the
 * device does the output queuing.  If a device receives an external signal,
 * like an interrupt, then it causes tty_wakeup() to be run by the CLOCK task
 * to, you guessed it, wake up the TTY to check if input or output can
 * continue.
 *
 * The valid messages and their parameters are:
 *
 *   HARD_INT:     output has been completed or input has arrived
 *   DEV_READ:     a process wants to read from a terminal
 *   DEV_WRITE:    a process wants to write on a terminal
 *   DEV_IOCTL:    a process wants to change a terminal's parameters
 *   DEV_OPEN:     a tty line has been opened
 *   DEV_CLOSE:    a tty line has been closed
 *   CANCEL:       terminate a previous incomplete system call immediately
 *
 *    m_type      TTY_LINE   PROC_NR    COUNT   TTY_SPEK  TTY_FLAGS  ADDRESS
 * ---------------------------------------------------------------------------
 * | HARD_INT    |         |         |         |         |         |         |
 * |-------------+---------+---------+---------+---------+---------+---------|
 * | DEV_READ    |minor dev| proc nr |  count  |         O_NONBLOCK| buf ptr |
 * |-------------+---------+---------+---------+---------+---------+---------|
 * | DEV_WRITE   |minor dev| proc nr |  count  |         |         | buf ptr |
 * |-------------+---------+---------+---------+---------+---------+---------|
 * | DEV_IOCTL   |minor dev| proc nr |func code|erase etc|  flags  |         |
 * |-------------+---------+---------+---------+---------+---------+---------|
 * | DEV_OPEN    |minor dev| proc nr | O_NOCTTY|         |         |         |
 * |-------------+---------+---------+---------+---------+---------+---------|
 * | DEV_CLOSE   |minor dev| proc nr |         |         |         |         |
 * |-------------+---------+---------+---------+---------+---------+---------|
 * | CANCEL      |minor dev| proc nr |         |         |         |         |
 * ---------------------------------------------------------------------------
 */

/**
 * @file
 *
 *  changed by Gerald Hoch 01.08.2011
 */

/**
 * include system headers
 */
#include <types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <embox/unit.h>
#include <embox/device.h>
#include <embox/service/callback.h>
#include <asm/io.h>
#include <kernel/panic.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/thread/api.h>
#include <kernel/thread/sync/message.h>
#include <hal/reg.h>
#include <fs/node.h>
#include <kernel/file.h>

/**
 * include TTY specific headers
 */
#include <drivers/tty_posix/const.h>
#include <drivers/tty_posix/termios.h>
#include <drivers/tty_posix/tty.h>

/*
 * Types
 */

/*
 * static functions
 */
#if 0
static void 	*open		(const char *fname, const char *mode);
static int 		close		(void *file);
static size_t 	read		(void *buf, size_t size, size_t count, void *file);
static size_t 	write		(const void *buff, size_t size, size_t count, void *file);
static int 		ioctl		(void *file, int request, va_list args);
#endif


#define ENABLE_SRCCOMPAT 0
#define ENABLE_BINCOMPAT 0


/* RS232 lines or pseudo terminals can be completely configured out. */
#if CONFIG_NR_RS_LINES == 0
	#define rs_init(tp)	((void) 0)
#endif
#if CONFIG_NR_PTYS == 0
	#define pty_init(tp)	((void) 0)
	#define do_pty(tp, mp)	((void) 0)
#endif

#if 1
FORWARD _PROTOTYPE( void do_cancel, (tty_t *tp, tty_msg_t *m_ptr)		);
FORWARD _PROTOTYPE( void do_ioctl, (tty_t *tp, tty_msg_t *m_ptr)		);
FORWARD _PROTOTYPE( void do_open, (tty_t *tp, tty_msg_t *m_ptr)		);
FORWARD _PROTOTYPE( void do_close, (tty_t *tp, tty_msg_t *m_ptr)		);
FORWARD _PROTOTYPE( void do_read, (tty_t *tp, tty_msg_t *m_ptr)		);
FORWARD _PROTOTYPE( void do_write, (tty_t *tp, tty_msg_t *m_ptr)		);
#endif
#if 1
FORWARD _PROTOTYPE( void in_transfer, (tty_t *tp)			);
FORWARD _PROTOTYPE( int echo, (tty_t *tp, int ch)			);
FORWARD _PROTOTYPE( void rawecho, (tty_t *tp, int ch)			);
FORWARD _PROTOTYPE( int back_over, (tty_t *tp)				);
FORWARD _PROTOTYPE( void reprint, (tty_t *tp)				);
FORWARD _PROTOTYPE( void dev_ioctl, (tty_t *tp)				);
FORWARD _PROTOTYPE( void setattr, (tty_t *tp)				);
FORWARD _PROTOTYPE( void tty_icancel, (tty_t *tp)			);
FORWARD _PROTOTYPE( void tty_init, (tty_t *tp)				);
FORWARD _PROTOTYPE( void tty_devnop,(tty_t *tp)				);
FORWARD _PROTOTYPE( void settimer, (tty_t *tp, int on)			);
#if ENABLE_SRCCOMPAT || ENABLE_BINCOMPAT
FORWARD _PROTOTYPE( int compat_getp, (tty_t *tp, struct sgttyb *sg)	);
FORWARD _PROTOTYPE( int compat_getc, (tty_t *tp, struct tchars *sg)	);
FORWARD _PROTOTYPE( int compat_setp, (tty_t *tp, struct sgttyb *sg)	);
FORWARD _PROTOTYPE( int compat_setc, (tty_t *tp, struct tchars *sg)	);
FORWARD _PROTOTYPE( int tspd2sgspd, (speed_t tspd)			);
FORWARD _PROTOTYPE( speed_t sgspd2tspd, (int sgspd)			);
#if ENABLE_BINCOMPAT
FORWARD _PROTOTYPE( void do_ioctl_compat, (tty_t *tp, tty_msg *m_ptr)	);
#endif
#endif

/* Default attributes. */
PRIVATE struct termios termios_defaults = {
  TINPUT_DEF, TOUTPUT_DEF, TCTRL_DEF, TLOCAL_DEF, TSPEED_DEF, TSPEED_DEF,
  {
	TEOF_DEF, TEOL_DEF, TERASE_DEF, TINTR_DEF, TKILL_DEF, TMIN_DEF,
	TQUIT_DEF, TTIME_DEF, TSUSP_DEF, TSTART_DEF, TSTOP_DEF,
	TREPRINT_DEF, TLNEXT_DEF, TDISCARD_DEF,
  },
};
PRIVATE struct winsize winsize_defaults;	/* = all zeroes */
#endif

PUBLIC int current = 0;		/* currently visible console */
PUBLIC tty_t tty_table[CONFIG_NR_CONS + CONFIG_NR_RS_LINES + CONFIG_NR_PTYS];
PUBLIC struct thread *tty_thread;
#define TTY &tty_thread
clock_t tty_timeout;	/* time to wake up the TTY task */
static void *tty_task(void *data);
PUBLIC void handle_events(tty_t *tp);


EMBOX_UNIT_INIT(tty_task_init);
static int tty_task_init(void)
{
	// create thread
	thread_create(	&tty_thread,
					THREAD_FLAG_PRIORITY_INHERIT | THREAD_FLAG_DETACHED,
					tty_task,
					NULL);
	// TODO create timer
	return 0;
}
/*===========================================================================*
 *				tty_task				     *
 *===========================================================================*/
static void *tty_task(void *data)
{
/* Main routine of the terminal task. */

  tty_msg_t *tty_mess;		/* buffer for all incoming messages */
  struct message *pMsg;
  register tty_t *tp;
  unsigned line;
  FILE *f;
  int i;

  // create all console devices
  f = fopen("/dev/console", "w");
  if (NULL == f) {
	  LOG_ERROR("/dev/console has not registered!\n");
	  return -1;
  }

  /* Initialize the terminal lines. */
  line  = 0;
  idx 	= 0;
  int uLineCon 	= 1;
  int dLineCon 	= CONFIG_NR_CONS;

  int uLineRs 	= CONFIG_NR_CONS + 1;
  int dLineRs 	= CONFIG_NR_CONS + 1 + CONFIG_NR_RS_LINES;

  int uLinePty 	= CONFIG_NR_CONS + 1 + CONFIG_NR_RS_LINES + 1;
  int dLinePty	= CONFIG_NR_PTYS;

  for (tp = FIRST_TTY; tp < END_TTY; tp++) {

	  if (line <  CONFIG_NR_CONS)
	  {
		  idx = line;
	  }
	  if (line >= uLineCon && line <= dLineCon)
	  {
		  idx = line - uLineCon;
	  }
	  if (line >= uLineRs && line <= dLineRs)
	  {
		  idx = line - uLineRs;
	  }
	  if (line >= uLinePty && line <= dLinePty)
	  {
		  idx = line - uLinePty;
	  }

	  tty_init(tp,idx);

	  i = ++line;
  }

  while (TRUE) {
	/* Check if a timer expired. */
	#if 0
	  if (cproc_addr(TTY)->p_exptimers != NULL) tmr_exptimers();
	#endif
	/* Handle any events on any of the ttys. */
	for (tp = FIRST_TTY; tp < END_TTY; tp++) {
		if (tp->tty_events) handle_events(tp);
	}
	pMsg = message_receive();
	tty_mess = (tty_msg_t *)pMsg->data;

	/* A hardware interrupt is an invitation to check for events. */
	if (tty_mess->m_type == HARD_INT) continue;

	/* Check the minor device number. */
	line = tty_mess->TTY_LINE;
	if ((line - CONS_MINOR) < CONFIG_NR_CONS) {
		tp = tty_addr(line - CONS_MINOR);
	} else
	if (line == LOG_MINOR) {
		tp = tty_addr(0);
	} else
	if ((line - RS232_MINOR) < CONFIG_NR_RS_LINES) {
		tp = tty_addr(line - RS232_MINOR + CONFIG_NR_CONS);
	} else
	if ((line - TTYPX_MINOR) < CONFIG_NR_PTYS) {
		tp = tty_addr(line - TTYPX_MINOR + CONFIG_NR_CONS + CONFIG_NR_RS_LINES);
	} else
	if ((line - PTYPX_MINOR) < CONFIG_NR_PTYS) {
		tp = tty_addr(line - PTYPX_MINOR + CONFIG_NR_CONS + CONFIG_NR_RS_LINES);
		if (tty_mess->m_type != DEV_IOCTL) {
			do_pty(tp, tty_mess);
			free(tty_mess);
			continue;
		}
	} else {
		tp = NULL;
	}

	/* If the device doesn't exist or is not configured return ENXIO. */
	if (tp == NULL || !tty_active(tp)) {
		tty_reply(TASK_REPLY, tty_mess->m_source,tty_mess->PROC_NR, ENXIO);
		free(tty_mess);
		continue;
	}

	/* Execute the requested function. */
	switch (tty_mess->m_type) {
	    case DEV_READ:	do_read(tp, tty_mess);		break;
	    case DEV_WRITE:	do_write(tp, tty_mess);	break;
	    case DEV_IOCTL:	do_ioctl(tp, tty_mess);	break;
	    case DEV_OPEN:	do_open(tp, tty_mess);		break;
	    case DEV_CLOSE:	do_close(tp, tty_mess);	break;
	    case CANCEL:	do_cancel(tp, tty_mess);	break;
	    default:		tty_reply(TASK_REPLY, tty_mess->m_source,tty_mess->PROC_NR, EINVAL);
	}
	free(tty_mess);
  }
  return 0;
}

/*==========================================================================*
 *				tty_init				    *
 *==========================================================================*/
PRIVATE void tty_init(tty_t *tp,int line)
/* TTY line to initialize.
 * instance line */
{
/* Initialize tty structure and call device initialization routines. */
	FILE *fc,*fk;
	node_t* node;

	tp->tty_intail = tp->tty_inhead = tp->tty_inbuf;
	tp->tty_min = 1;
	tp->tty_termios = termios_defaults;
	tp->tty_icancel = tp->tty_ocancel = tp->tty_ioctl = tp->tty_close = tty_devnop;

	if (tp < tty_addr(CONFIG_NR_CONS)) {

		fc = fopen(sprintf("/dev/console/%d",line), "w");
		if (NULL == fc) {
			LOG_ERROR("/dev/console/0 has not registered!\n");
			return -1;
		}
		node = (node_t*) fc;
		cons = (console_t *)node->attr;

		if (tp == tty_addr(CONFIG_NR_CONS)) {
			fk = fopen("/dev/kbd", "r");
			if (NULL == fk) {
				LOG_ERROR("/dev/kbd has not registered!\n");
				return -1;
			}
		}

	} else {
		if (tp < tty_addr(CONFIG_NR_CONS+CONFIG_NR_RS_LINES)) {
			rs_init(tp);
		} else {
			pty_init(tp);
		}
	}
}

/*===========================================================================*
 *				do_read					     *
 *===========================================================================*/
PRIVATE void do_read(tp, m_ptr)
register tty_t *tp;		/* pointer to tty struct */
tty_msg_t *m_ptr;			/* pointer to tty_msg sent to the task */
{
/* A process wants to read from a terminal. */
  int r;

  /* Check if there is already a process hanging in a read, check if the
   * parameters are correct, do I/O.
   */
  if (tp->tty_inleft > 0) {
	r = EIO;
  } else
  if (m_ptr->COUNT <= 0) {
	r = EINVAL;
  } else
  if (numap(m_ptr->PROC_NR, (vir_bytes) m_ptr->ADDRESS, m_ptr->COUNT) == 0) {
	r = EFAULT;
  } else {
	/* Copy information from the tty_msg to the tty struct. */
	tp->tty_inrepcode = TASK_REPLY;
	tp->tty_incaller = m_ptr->m_source;
	tp->tty_inproc = m_ptr->PROC_NR;
	tp->tty_in_vir = (vir_bytes) m_ptr->ADDRESS;
	tp->tty_inleft = m_ptr->COUNT;

	if (!(tp->tty_termios.c_lflag & ICANON)
					&& tp->tty_termios.c_cc[VTIME] > 0) {
		if (tp->tty_termios.c_cc[VMIN] == 0) {
			/* MIN & TIME specify a read timer that finishes the
			 * read in TIME/10 seconds if no bytes are available.
			 */
			lock();
			settimer(tp, TRUE);
			tp->tty_min = 1;
			unlock();
		} else {
			/* MIN & TIME specify an inter-byte timer that may
			 * have to be cancelled if there are no bytes yet.
			 */
			if (tp->tty_eotct == 0) {
				lock();
				settimer(tp, FALSE);
				unlock();
				tp->tty_min = tp->tty_termios.c_cc[VMIN];
			}
		}
	}

	/* Anything waiting in the input buffer? Clear it out... */
	in_transfer(tp);
	/* ...then go back for more */
	handle_events(tp);
	if (tp->tty_inleft == 0) return;		/* already done */

	/* There were no bytes in the input queue available, so either suspend
	 * the caller or break off the read if nonblocking.
	 */
	if (m_ptr->TTY_FLAGS & O_NONBLOCK) {
		r = EAGAIN;				/* cancel the read */
		tp->tty_inleft = tp->tty_incum = 0;
	} else {
		r = SUSPEND;				/* suspend the caller */
		tp->tty_inrepcode = REVIVE;
	}
  }
  tty_reply(TASK_REPLY, m_ptr->m_source, m_ptr->PROC_NR, r);
}


/*===========================================================================*
 *				do_write				     *
 *===========================================================================*/
PRIVATE void do_write(tp, m_ptr)
register tty_t *tp;
register tty_msg_t *m_ptr;	/* pointer to tty_msg sent to the task */
{
/* A process wants to write on a terminal. */
  int r;

  /* Check if there is already a process hanging in a write, check if the
   * parameters are correct, do I/O.
   */
  if (tp->tty_outleft > 0) {
	r = EIO;
  } else
  if (m_ptr->COUNT <= 0) {
	r = EINVAL;
  } else
  if (numap(m_ptr->PROC_NR, (vir_bytes) m_ptr->ADDRESS, m_ptr->COUNT) == 0) {
	r = EFAULT;
  } else {
	/* Copy tty_msg parameters to the tty structure. */
	tp->tty_outrepcode = TASK_REPLY;
	tp->tty_outcaller = m_ptr->m_source;
	tp->tty_outproc = m_ptr->PROC_NR;
	tp->tty_out_vir = (vir_bytes) m_ptr->ADDRESS;
	tp->tty_outleft = m_ptr->COUNT;

	/* Try to write. */
	handle_events(tp);
	if (tp->tty_outleft == 0) return;		/* already done */

	/* None or not all the bytes could be written, so either suspend the
	 * caller or break off the write if nonblocking.
	 */
	if (m_ptr->TTY_FLAGS & O_NONBLOCK) {		/* cancel the write */
		r = tp->tty_outcum > 0 ? tp->tty_outcum : EAGAIN;
		tp->tty_outleft = tp->tty_outcum = 0;
	} else {
		r = SUSPEND;				/* suspend the caller */
		tp->tty_outrepcode = REVIVE;
	}
  }
  tty_reply(TASK_REPLY, m_ptr->m_source, m_ptr->PROC_NR, r);
}


/*===========================================================================*
 *				do_ioctl				     *
 *===========================================================================*/
PRIVATE void do_ioctl(tp, m_ptr)
register tty_t *tp;
tty_msg_t *m_ptr;			/* pointer to tty_msg sent to task */
{
/* Perform an IOCTL on this terminal. Posix termios calls are handled
 * by the IOCTL system call
 */

  int r;
  union {
	int i;
#if ENABLE_SRCCOMPAT
	struct sgttyb sg;
	struct tchars tc;
#endif
  } param;
  phys_bytes user_phys;
  size_t size;

  /* Size of the ioctl parameter. */
  switch (m_ptr->TTY_REQUEST) {
    case TCGETS:        /* Posix tcgetattr function */
    case TCSETS:        /* Posix tcsetattr function, TCSANOW option */
    case TCSETSW:       /* Posix tcsetattr function, TCSADRAIN option */
    case TCSETSF:	/* Posix tcsetattr function, TCSAFLUSH option */
        size = sizeof(struct termios);
        break;

    case TCSBRK:        /* Posix tcsendbreak function */
    case TCFLOW:        /* Posix tcflow function */
    case TCFLSH:        /* Posix tcflush function */
    case TIOCGPGRP:     /* Posix tcgetpgrp function */
    case TIOCSPGRP:	/* Posix tcsetpgrp function */
        size = sizeof(int);
        break;

    case TIOCGWINSZ:    /* get window size (not Posix) */
    case TIOCSWINSZ:	/* set window size (not Posix) */
        size = sizeof(struct winsize);
        break;

#if ENABLE_SRCCOMPAT
    case TIOCGETP:      /* BSD-style get terminal properties */
    case TIOCSETP:	/* BSD-style set terminal properties */
	size = sizeof(struct sgttyb);
	break;

    case TIOCGETC:      /* BSD-style get terminal special characters */
    case TIOCSETC:	/* BSD-style get terminal special characters */
	size = sizeof(struct tchars);
	break;
#endif
#if (MACHINE == IBM_PC)
    case KIOCSMAP:	/* load keymap (Minix extension) */
        size = sizeof(keymap_t);
        break;

    case TIOCSFON:	/* load font (Minix extension) */
        size = sizeof(u8_t [8192]);
        break;

#endif
    case TCDRAIN:	/* Posix tcdrain function -- no parameter */
    default:		size = 0;
  }

  if (size != 0) {
	user_phys = numap(m_ptr->PROC_NR, (vir_bytes) m_ptr->ADDRESS, size);
	if (user_phys == 0) {
		tty_reply(TASK_REPLY, m_ptr->m_source, m_ptr->PROC_NR, EFAULT);
		return;
	}
  }

  r = OK;
  switch (m_ptr->TTY_REQUEST) {
    case TCGETS:
	/* Get the termios attributes. */
	phys_copy(vir2phys(&tp->tty_termios), user_phys, (phys_bytes) size);
	break;

    case TCSETSW:
    case TCSETSF:
    case TCDRAIN:
	if (tp->tty_outleft > 0) {
		/* Wait for all ongoing output processing to finish. */
		tp->tty_iocaller = m_ptr->m_source;
		tp->tty_ioproc = m_ptr->PROC_NR;
		tp->tty_ioreq = m_ptr->REQUEST;
		tp->tty_iovir = (vir_bytes) m_ptr->ADDRESS;
		r = SUSPEND;
		break;
	}
	if (m_ptr->TTY_REQUEST == TCDRAIN) break;
	if (m_ptr->TTY_REQUEST == TCSETSF) tty_icancel(tp);
	/*FALL THROUGH*/
    case TCSETS:
	/* Set the termios attributes. */
	phys_copy(user_phys, vir2phys(&tp->tty_termios), (phys_bytes) size);
	setattr(tp);
	break;

    case TCFLSH:
	phys_copy(user_phys, vir2phys(&param.i), (phys_bytes) size);
	switch (param.i) {
	    case TCIFLUSH:	tty_icancel(tp);			break;
	    case TCOFLUSH:	(*tp->tty_ocancel)(tp);			break;
	    case TCIOFLUSH:	tty_icancel(tp); (*tp->tty_ocancel)(tp);break;
	    default:		r = EINVAL;
	}
	break;

    case TCFLOW:
	phys_copy(user_phys, vir2phys(&param.i), (phys_bytes) size);
	switch (param.i) {
	    case TCOOFF:
	    case TCOON:
		tp->tty_inhibited = (param.i == TCOOFF);
		tp->tty_events = 1;
		break;
	    case TCIOFF:
		(*tp->tty_echo)(tp, tp->tty_termios.c_cc[VSTOP]);
		break;
	    case TCION:
		(*tp->tty_echo)(tp, tp->tty_termios.c_cc[VSTART]);
		break;
	    default:
		r = EINVAL;
	}
	break;

    case TCSBRK:
	if (tp->tty_break != NULL) (*tp->tty_break)(tp);
	break;

    case TIOCGWINSZ:
	phys_copy(vir2phys(&tp->tty_winsize), user_phys, (phys_bytes) size);
	break;

    case TIOCSWINSZ:
	phys_copy(user_phys, vir2phys(&tp->tty_winsize), (phys_bytes) size);
	/* SIGWINCH... */
	break;

#if ENABLE_SRCCOMPAT
    case TIOCGETP:
	compat_getp(tp, &param.sg);
	phys_copy(vir2phys(&param.sg), user_phys, (phys_bytes) size);
	break;

    case TIOCSETP:
	phys_copy(user_phys, vir2phys(&param.sg), (phys_bytes) size);
	compat_setp(tp, &param.sg);
	break;

    case TIOCGETC:
	compat_getc(tp, &param.tc);
	phys_copy(vir2phys(&param.tc), user_phys, (phys_bytes) size);
	break;

    case TIOCSETC:
	phys_copy(user_phys, vir2phys(&param.tc), (phys_bytes) size);
	compat_setc(tp, &param.tc);
	break;
#endif

#if (MACHINE == IBM_PC)
    case KIOCSMAP:
	/* Load a new keymap (only /dev/console). */
	if (isconsole(tp)) r = kbd_loadmap(user_phys);
	break;

    case TIOCSFON:
	/* Load a font into an EGA or VGA card (hs@hck.hr) */
	if (isconsole(tp)) r = con_loadfont(user_phys);
	break;
#endif

#if (MACHINE == ATARI)
    case VDU_LOADFONT:
	r = vdu_loadfont(m_ptr);
	break;
#endif

/* These Posix functions are allowed to fail if _POSIX_JOB_CONTROL is
 * not defined.
 */
    case TIOCGPGRP:
    case TIOCSPGRP:
    default:
#if ENABLE_BINCOMPAT
	do_ioctl_compat(tp, m_ptr);
	return;
#else
	r = ENOTTY;
#endif
  }

  /* Send the reply. */
  tty_reply(TASK_REPLY, m_ptr->m_source, m_ptr->PROC_NR, r);
}


/*===========================================================================*
 *				do_open					     *
 *===========================================================================*/
PRIVATE void do_open(tp, m_ptr)
register tty_t *tp;
tty_msg_t *m_ptr;			/* pointer to tty_msg sent to task */
{
/* A tty line has been opened.  Make it the callers controlling tty if
 * O_NOCTTY is *not* set and it is not the log device.  1 is returned if
 * the tty is made the controlling tty, otherwise OK or an error code.
 */
  int r = OK;

  if (m_ptr->TTY_LINE == LOG_MINOR) {
	/* The log device is a write-only diagnostics device. */
	if (m_ptr->COUNT & R_BIT) r = EACCES;
  } else {
	if (!(m_ptr->COUNT & O_NOCTTY)) {
		tp->tty_pgrp = m_ptr->PROC_NR;
		r = 1;
	}
	tp->tty_openct++;
  }
  tty_reply(TASK_REPLY, m_ptr->m_source, m_ptr->PROC_NR, r);
}


/*===========================================================================*
 *				do_close				     *
 *===========================================================================*/
PRIVATE void do_close(tp, m_ptr)
register tty_t *tp;
tty_msg_t *m_ptr;			/* pointer to tty_msg sent to task */
{
/* A tty line has been closed.  Clean up the line if it is the last close. */

  if (m_ptr->TTY_LINE != LOG_MINOR && --tp->tty_openct == 0) {
	tp->tty_pgrp = 0;
	tty_icancel(tp);
	(*tp->tty_ocancel)(tp);
	(*tp->tty_close)(tp);
	tp->tty_termios = termios_defaults;
	tp->tty_winsize = winsize_defaults;
	setattr(tp);
  }
  tty_reply(TASK_REPLY, m_ptr->m_source, m_ptr->PROC_NR, OK);
}


/*===========================================================================*
 *				do_cancel				     *
 *===========================================================================*/
PRIVATE void do_cancel(tp, m_ptr)
register tty_t *tp;
tty_msg_t *m_ptr;			/* pointer to tty_msg sent to task */
{
/* A signal has been sent to a process that is hanging trying to read or write.
 * The pending read or write must be finished off immediately.
 */

  int proc_nr;
  int mode;

  /* Check the parameters carefully, to avoid cancelling twice. */
  proc_nr = m_ptr->PROC_NR;
  mode = m_ptr->COUNT;
  if ((mode & R_BIT) && tp->tty_inleft != 0 && proc_nr == tp->tty_inproc) {
	/* Process was reading when killed.  Clean up input. */
	tty_icancel(tp);
	tp->tty_inleft = tp->tty_incum = 0;
  }
  if ((mode & W_BIT) && tp->tty_outleft != 0 && proc_nr == tp->tty_outproc) {
	/* Process was writing when killed.  Clean up output. */
	(*tp->tty_ocancel)(tp);
	tp->tty_outleft = tp->tty_outcum = 0;
  }
  if (tp->tty_ioreq != 0 && proc_nr == tp->tty_ioproc) {
	/* Process was waiting for output to drain. */
	tp->tty_ioreq = 0;
  }
  tp->tty_events = 1;
  tty_reply(TASK_REPLY, m_ptr->m_source, proc_nr, EINTR);
}

/*===========================================================================*
 *				handle_events				     *
 *===========================================================================*/
PUBLIC void handle_events(tp)
tty_t *tp;			/* TTY to check for events. */
{
/* Handle any events pending on a TTY.  These events are usually device
 * interrupts.
 *
 * Two kinds of events are prominent:
 *	- a character has been received from the console or an RS232 line.
 *	- an RS232 line has completed a write request (on behalf of a user).
 * The interrupt handler may delay the interrupt tty_msg at its discretion
 * to avoid swamping the TTY task.  Messages may be overwritten when the
 * lines are fast or when there are races between different lines, input
 * and output, because MINIX only provides single buffering for interrupt
 * messages (in proc.c).  This is handled by explicitly checking each line
 * for fresh input and completed output on each interrupt.
 */
  char *buf;
  unsigned count;

  do {
	tp->tty_events = 0;

	/* Read input and perform input processing. */
	(*tp->tty_devread)(tp);

	/* Perform output processing and write output. */
	(*tp->tty_devwrite)(tp);

	/* Ioctl waiting for some event? */
	if (tp->tty_ioreq != 0) dev_ioctl(tp);
  } while (tp->tty_events);

  /* Transfer characters from the input queue to a waiting process. */
  in_transfer(tp);

  /* Reply if enough bytes are available. */
  if (tp->tty_incum >= tp->tty_min && tp->tty_inleft > 0) {
	tty_reply(tp->tty_inrepcode, tp->tty_incaller, tp->tty_inproc,
								tp->tty_incum);
	tp->tty_inleft = tp->tty_incum = 0;
  }
}


/*===========================================================================*
 *				in_transfer				     *
 *===========================================================================*/
PRIVATE void in_transfer(tp)
register tty_t *tp;		/* pointer to terminal to read from */
{
/* Transfer bytes from the input queue to a process reading from a terminal. */

  int ch;
  int count;
  phys_bytes buf_phys, user_base;
  char buf[64], *bp;

  /* Force read to succeed if the line is hung up, looks like EOF to reader. */
  if (tp->tty_termios.c_ospeed == B0) tp->tty_min = 0;

  /* Anything to do? */
  if (tp->tty_inleft == 0 || tp->tty_eotct < tp->tty_min) return;

  buf_phys = vir2phys(buf);
  user_base = proc_vir2phys(proc_addr(tp->tty_inproc), 0);
  bp = buf;
  while (tp->tty_inleft > 0 && tp->tty_eotct > 0) {
	ch = *tp->tty_intail;

	if (!(ch & IN_EOF)) {
		/* One character to be delivered to the user. */
		*bp = ch & IN_CHAR;
		tp->tty_inleft--;
		if (++bp == bufend(buf)) {
			/* Temp buffer full, copy to user space. */
			phys_copy(buf_phys, user_base + tp->tty_in_vir,
						(phys_bytes) buflen(buf));
			tp->tty_in_vir += buflen(buf);
			tp->tty_incum += buflen(buf);
			bp = buf;
		}
	}

	/* Remove the character from the input queue. */
	if (++tp->tty_intail == bufend(tp->tty_inbuf))
		tp->tty_intail = tp->tty_inbuf;
	tp->tty_incount--;
	if (ch & IN_EOT) {
		tp->tty_eotct--;
		/* Don't read past a line break in canonical mode. */
		if (tp->tty_termios.c_lflag & ICANON) tp->tty_inleft = 0;
	}
  }

  if (bp > buf) {
	/* Leftover characters in the buffer. */
	count = bp - buf;
	phys_copy(buf_phys, user_base + tp->tty_in_vir, (phys_bytes) count);
	tp->tty_in_vir += count;
	tp->tty_incum += count;
  }

  /* Usually reply to the reader, possibly even if incum == 0 (EOF). */
  if (tp->tty_inleft == 0) {
	tty_reply(tp->tty_inrepcode, tp->tty_incaller, tp->tty_inproc,
								tp->tty_incum);
	tp->tty_inleft = tp->tty_incum = 0;
  }
}

/*===========================================================================*
 *				in_process				     *
 *===========================================================================*/
PUBLIC int in_process(tp, buf, count)
register tty_t *tp;		/* terminal on which character has arrived */
char *buf;			/* buffer with input characters */
int count;			/* number of input characters */
{
/* Characters have just been typed in.  Process, save, and echo them.  Return
 * the number of characters processed.
 */

  int ch, sig, ct;
  int timeset = FALSE;
  static unsigned char csize_mask[] = { 0x1F, 0x3F, 0x7F, 0xFF };

  for (ct = 0; ct < count; ct++) {
	/* Take one character. */
	ch = *buf++ & BYTE;

	/* Strip to seven bits? */
	if (tp->tty_termios.c_iflag & ISTRIP) ch &= 0x7F;

	/* Input extensions? */
	if (tp->tty_termios.c_lflag & IEXTEN) {

		/* Previous character was a character escape? */
		if (tp->tty_escaped) {
			tp->tty_escaped = NOT_ESCAPED;
			ch |= IN_ESC;	/* protect character */
		}

		/* LNEXT (^V) to escape the next character? */
		if (ch == tp->tty_termios.c_cc[VLNEXT]) {
			tp->tty_escaped = ESCAPED;
			rawecho(tp, '^');
			rawecho(tp, '\b');
			continue;	/* do not store the escape */
		}

		/* REPRINT (^R) to reprint echoed characters? */
		if (ch == tp->tty_termios.c_cc[VREPRINT]) {
			reprint(tp);
			continue;
		}
	}

	/* _POSIX_VDISABLE is a normal character value, so better escape it. */
	if (ch == _POSIX_VDISABLE) ch |= IN_ESC;

	/* Map CR to LF, ignore CR, or map LF to CR. */
	if (ch == '\r') {
		if (tp->tty_termios.c_iflag & IGNCR) continue;
		if (tp->tty_termios.c_iflag & ICRNL) ch = '\n';
	} else
	if (ch == '\n') {
		if (tp->tty_termios.c_iflag & INLCR) ch = '\r';
	}

	/* Canonical mode? */
	if (tp->tty_termios.c_lflag & ICANON) {

		/* Erase processing (rub out of last character). */
		if (ch == tp->tty_termios.c_cc[VERASE]) {
			(void) back_over(tp);
			if (!(tp->tty_termios.c_lflag & ECHOE)) {
				(void) echo(tp, ch);
			}
			continue;
		}

		/* Kill processing (remove current line). */
		if (ch == tp->tty_termios.c_cc[VKILL]) {
			while (back_over(tp)) {}
			if (!(tp->tty_termios.c_lflag & ECHOE)) {
				(void) echo(tp, ch);
				if (tp->tty_termios.c_lflag & ECHOK)
					rawecho(tp, '\n');
			}
			continue;
		}

		/* EOF (^D) means end-of-file, an invisible "line break". */
		if (ch == tp->tty_termios.c_cc[VEOF]) ch |= IN_EOT | IN_EOF;

		/* The line may be returned to the user after an LF. */
		if (ch == '\n') ch |= IN_EOT;

		/* Same thing with EOL, whatever it may be. */
		if (ch == tp->tty_termios.c_cc[VEOL]) ch |= IN_EOT;
	}

	/* Start/stop input control? */
	if (tp->tty_termios.c_iflag & IXON) {

		/* Output stops on STOP (^S). */
		if (ch == tp->tty_termios.c_cc[VSTOP]) {
			tp->tty_inhibited = STOPPED;
			tp->tty_events = 1;
			continue;
		}

		/* Output restarts on START (^Q) or any character if IXANY. */
		if (tp->tty_inhibited) {
			if (ch == tp->tty_termios.c_cc[VSTART]
					|| (tp->tty_termios.c_iflag & IXANY)) {
				tp->tty_inhibited = RUNNING;
				tp->tty_events = 1;
				if (ch == tp->tty_termios.c_cc[VSTART])
					continue;
			}
		}
	}

	if (tp->tty_termios.c_lflag & ISIG) {
		/* Check for INTR (^?) and QUIT (^\) characters. */
		if (ch == tp->tty_termios.c_cc[VINTR]
					|| ch == tp->tty_termios.c_cc[VQUIT]) {
			sig = SIGINT;
			if (ch == tp->tty_termios.c_cc[VQUIT]) sig = SIGQUIT;
			sigchar(tp, sig);
			(void) echo(tp, ch);
			continue;
		}
	}

	/* Is there space in the input buffer? */
	if (tp->tty_incount == buflen(tp->tty_inbuf)) {
		/* No space; discard in canonical mode, keep in raw mode. */
		if (tp->tty_termios.c_lflag & ICANON) continue;
		break;
	}

	if (!(tp->tty_termios.c_lflag & ICANON)) {
		/* In raw mode all characters are "line breaks". */
		ch |= IN_EOT;

		/* Start an inter-byte timer? */
		if (!timeset && tp->tty_termios.c_cc[VMIN] > 0
				&& tp->tty_termios.c_cc[VTIME] > 0) {
			lock();
			settimer(tp, TRUE);
			unlock();
			timeset = TRUE;
		}
	}

	/* Perform the intricate function of echoing. */
	if (tp->tty_termios.c_lflag & (ECHO|ECHONL)) ch = echo(tp, ch);

	/* Save the character in the input queue. */
	*tp->tty_inhead++ = ch;
	if (tp->tty_inhead == bufend(tp->tty_inbuf))
		tp->tty_inhead = tp->tty_inbuf;
	tp->tty_incount++;
	if (ch & IN_EOT) tp->tty_eotct++;

	/* Try to finish input if the queue threatens to overflow. */
	if (tp->tty_incount == buflen(tp->tty_inbuf)) in_transfer(tp);
  }
  return ct;
}
#if 0

/*===========================================================================*
 *				echo					     *
 *===========================================================================*/
PRIVATE int echo(tp, ch)
register tty_t *tp;		/* terminal on which to echo */
register int ch;		/* pointer to character to echo */
{
/* Echo the character if echoing is on.  Some control characters are echoed
 * with their normal effect, other control characters are echoed as "^X",
 * normal characters are echoed normally.  EOF (^D) is echoed, but immediately
 * backspaced over.  Return the character with the echoed length added to its
 * attributes.
 */
  int len, rp;

  ch &= ~IN_LEN;
  if (!(tp->tty_termios.c_lflag & ECHO)) {
	if (ch == ('\n' | IN_EOT) && (tp->tty_termios.c_lflag
					& (ICANON|ECHONL)) == (ICANON|ECHONL))
		(*tp->tty_echo)(tp, '\n');
	return(ch);
  }

  /* "Reprint" tells if the echo output has been messed up by other output. */
  rp = tp->tty_incount == 0 ? FALSE : tp->tty_reprint;

  if ((ch & IN_CHAR) < ' ') {
	switch (ch & (IN_ESC|IN_EOF|IN_EOT|IN_CHAR)) {
	    case '\t':
		len = 0;
		do {
			(*tp->tty_echo)(tp, ' ');
			len++;
		} while (len < TAB_SIZE && (tp->tty_position & TAB_MASK) != 0);
		break;
	    case '\r' | IN_EOT:
	    case '\n' | IN_EOT:
		(*tp->tty_echo)(tp, ch & IN_CHAR);
		len = 0;
		break;
	    default:
		(*tp->tty_echo)(tp, '^');
		(*tp->tty_echo)(tp, '@' + (ch & IN_CHAR));
		len = 2;
	}
  } else
  if ((ch & IN_CHAR) == '\177') {
	/* A DEL prints as "^?". */
	(*tp->tty_echo)(tp, '^');
	(*tp->tty_echo)(tp, '?');
	len = 2;
  } else {
	(*tp->tty_echo)(tp, ch & IN_CHAR);
	len = 1;
  }
  if (ch & IN_EOF) while (len > 0) { (*tp->tty_echo)(tp, '\b'); len--; }

  tp->tty_reprint = rp;
  return(ch | (len << IN_LSHIFT));
}


/*==========================================================================*
 *				rawecho					    *
 *==========================================================================*/
PRIVATE void rawecho(tp, ch)
register tty_t *tp;
int ch;
{
/* Echo without interpretation if ECHO is set. */
  int rp = tp->tty_reprint;
  if (tp->tty_termios.c_lflag & ECHO) (*tp->tty_echo)(tp, ch);
  tp->tty_reprint = rp;
}


/*==========================================================================*
 *				back_over				    *
 *==========================================================================*/
PRIVATE int back_over(tp)
register tty_t *tp;
{
/* Backspace to previous character on screen and erase it. */
  u16_t *head;
  int len;

  if (tp->tty_incount == 0) return(0);	/* queue empty */
  head = tp->tty_inhead;
  if (head == tp->tty_inbuf) head = bufend(tp->tty_inbuf);
  if (*--head & IN_EOT) return(0);		/* can't erase "line breaks" */
  if (tp->tty_reprint) reprint(tp);		/* reprint if messed up */
  tp->tty_inhead = head;
  tp->tty_incount--;
  if (tp->tty_termios.c_lflag & ECHOE) {
	len = (*head & IN_LEN) >> IN_LSHIFT;
	while (len > 0) {
		rawecho(tp, '\b');
		rawecho(tp, ' ');
		rawecho(tp, '\b');
		len--;
	}
  }
  return(1);				/* one character erased */
}


/*==========================================================================*
 *				reprint					    *
 *==========================================================================*/
PRIVATE void reprint(tp)
register tty_t *tp;		/* pointer to tty struct */
{
/* Restore what has been echoed to screen before if the user input has been
 * messed up by output, or if REPRINT (^R) is typed.
 */
  int count;
  u16_t *head;

  tp->tty_reprint = FALSE;

  /* Find the last line break in the input. */
  head = tp->tty_inhead;
  count = tp->tty_incount;
  while (count > 0) {
	if (head == tp->tty_inbuf) head = bufend(tp->tty_inbuf);
	if (head[-1] & IN_EOT) break;
	head--;
	count--;
  }
  if (count == tp->tty_incount) return;		/* no reason to reprint */

  /* Show REPRINT (^R) and move to a new line. */
  (void) echo(tp, tp->tty_termios.c_cc[VREPRINT] | IN_ESC);
  rawecho(tp, '\r');
  rawecho(tp, '\n');

  /* Reprint from the last break onwards. */
  do {
	if (head == bufend(tp->tty_inbuf)) head = tp->tty_inbuf;
	*head = echo(tp, *head);
	head++;
	count++;
  } while (count < tp->tty_incount);
}


/*==========================================================================*
 *				out_process				    *
 *==========================================================================*/
PUBLIC void out_process(tp, bstart, bpos, bend, icount, ocount)
tty_t *tp;
char *bstart, *bpos, *bend;	/* start/pos/end of circular buffer */
int *icount;			/* # input chars / input chars used */
int *ocount;			/* max output chars / output chars used */
{
/* Perform output processing on a circular buffer.  *icount is the number of
 * bytes to process, and the number of bytes actually processed on return.
 * *ocount is the space available on input and the space used on output.
 * (Naturally *icount < *ocount.)  The column position is updated modulo
 * the TAB size, because we really only need it for tabs.
 */

  int tablen;
  int ict = *icount;
  int oct = *ocount;
  int pos = tp->tty_position;

  while (ict > 0) {
	switch (*bpos) {
	case '\7':
		break;
	case '\b':
		pos--;
		break;
	case '\r':
		pos = 0;
		break;
	case '\n':
		if ((tp->tty_termios.c_oflag & (OPOST|ONLCR))
							== (OPOST|ONLCR)) {
			/* Map LF to CR+LF if there is space.  Note that the
			 * next character in the buffer is overwritten, so
			 * we stop at this point.
			 */
			if (oct >= 2) {
				*bpos = '\r';
				if (++bpos == bend) bpos = bstart;
				*bpos = '\n';
				pos = 0;
				ict--;
				oct -= 2;
			}
			goto out_done;	/* no space or buffer got changed */
		}
		break;
	case '\t':
		/* Best guess for the tab length. */
		tablen = TAB_SIZE - (pos & TAB_MASK);

		if ((tp->tty_termios.c_oflag & (OPOST|XTABS))
							== (OPOST|XTABS)) {
			/* Tabs must be expanded. */
			if (oct >= tablen) {
				pos += tablen;
				ict--;
				oct -= tablen;
				do {
					*bpos = ' ';
					if (++bpos == bend) bpos = bstart;
				} while (--tablen != 0);
			}
			goto out_done;
		}
		/* Tabs are output directly. */
		pos += tablen;
		break;
	default:
		/* Assume any other character prints as one character. */
		pos++;
	}
	if (++bpos == bend) bpos = bstart;
	ict--;
	oct--;
  }
out_done:
  tp->tty_position = pos & TAB_MASK;

  *icount -= ict;	/* [io]ct are the number of chars not used */
  *ocount -= oct;	/* *[io]count are the number of chars that are used */
}


/*===========================================================================*
 *				dev_ioctl				     *
 *===========================================================================*/
PRIVATE void dev_ioctl(tp)
tty_t *tp;
{
/* The ioctl's TCSETSW, TCSETSF and TCDRAIN wait for output to finish to make
 * sure that an attribute change doesn't affect the processing of current
 * output.  Once output finishes the ioctl is executed as in do_ioctl().
 */
  phys_bytes user_phys;

  if (tp->tty_outleft > 0) return;		/* output not finished */

  if (tp->tty_ioreq != TCDRAIN) {
	if (tp->tty_ioreq == TCSETSF) tty_icancel(tp);
	user_phys = proc_vir2phys(proc_addr(tp->tty_ioproc), tp->tty_iovir);
	phys_copy(user_phys, vir2phys(&tp->tty_termios),
					(phys_bytes) sizeof(tp->tty_termios));
	setattr(tp);
  }
  tp->tty_ioreq = 0;
  tty_reply(REVIVE, tp->tty_iocaller, tp->tty_ioproc, OK);
}


/*===========================================================================*
 *				setattr					     *
 *===========================================================================*/
PRIVATE void setattr(tp)
tty_t *tp;
{
/* Apply the new line attributes (raw/canonical, line speed, etc.) */
  u16_t *inp;
  int count;

  if (!(tp->tty_termios.c_lflag & ICANON)) {
	/* Raw mode; put a "line break" on all characters in the input queue.
	 * It is undefined what happens to the input queue when ICANON is
	 * switched off, a process should use TCSAFLUSH to flush the queue.
	 * Keeping the queue to preserve typeahead is the Right Thing, however
	 * when a process does use TCSANOW to switch to raw mode.
	 */
	count = tp->tty_eotct = tp->tty_incount;
	inp = tp->tty_intail;
	while (count > 0) {
		*inp |= IN_EOT;
		if (++inp == bufend(tp->tty_inbuf)) inp = tp->tty_inbuf;
		--count;
	}
  }

  /* Inspect MIN and TIME. */
  lock();
  settimer(tp, FALSE);
  unlock();
  if (tp->tty_termios.c_lflag & ICANON) {
	/* No MIN & TIME in canonical mode. */
	tp->tty_min = 1;
  } else {
	/* In raw mode MIN is the number of chars wanted, and TIME how long
	 * to wait for them.  With interesting exceptions if either is zero.
	 */
	tp->tty_min = tp->tty_termios.c_cc[VMIN];
	if (tp->tty_min == 0 && tp->tty_termios.c_cc[VTIME] > 0)
		tp->tty_min = 1;
  }

  if (!(tp->tty_termios.c_iflag & IXON)) {
	/* No start/stop output control, so don't leave output inhibited. */
	tp->tty_inhibited = RUNNING;
	tp->tty_events = 1;
  }

  /* Setting the output speed to zero hangs up the phone. */
  if (tp->tty_termios.c_ospeed == B0) sigchar(tp, SIGHUP);

  /* Set new line speed, character size, etc at the device level. */
  (*tp->tty_ioctl)(tp);
}

#endif
/*===========================================================================*
 *				tty_reply				     *
 *===========================================================================*/
PUBLIC void tty_reply(int code, struct thread *replyee, struct thread * proc_nr, int status)
//int code;			/* TASK_REPLY or REVIVE */
//int replyee;		/* destination address for the reply */
//int proc_nr;		/* to whom should the reply go? */
//int status;			/* reply code */
{
/* Send a reply to a process that wanted to read or write data. */

	  message msg;
	  tty_msg_t *pTTYMsg;

	  pTTYMsg = malloc (sizeof(tty_msg));
	  if (pTTYMsg == NULL)
		  return;

	  msg.data = pTTYMsg;
	  msg.m_type = code;

	  pTTYMsg->m_type 		= code;
	  pTTYMsg->REP_PROC_NR 	= proc_nr;
	  pTTYMsg->REP_STATUS 	= status;

	  message_send(&msg, replyee);
}
/*==========================================================================*
 *				tty_devnop				    *
 *==========================================================================*/
PUBLIC void tty_devnop(tp)
tty_t *tp;
{
  /* Some functions need not be implemented at the device level. */
}

/*===========================================================================*
 *				settimer				     *
 *===========================================================================*/
PRIVATE void settimer(tp, on)
tty_t *tp;			/* line to set or unset a timer on */
int on;				/* set timer if true, otherwise unset */
{
/* Set or unset a TIME inspired timer.  This function is interrupt sensitive
 * due to tty_wakeup(), so it must be called from within lock()/unlock().
 */
  tty_t **ptp;

  /* Take tp out of the timerlist if present. */
  for (ptp = &tty_timelist; *ptp != NULL; ptp = &(*ptp)->tty_timenext) {
	if (tp == *ptp) {
		*ptp = tp->tty_timenext;	/* take tp out of the list */
		break;
	}
  }
  if (!on) return;				/* unsetting it is enough */

  /* Timeout occurs TIME deciseconds from now. */
  tp->tty_time = get_uptime() + tp->tty_termios.c_cc[VTIME] * (HZ/10);

  /* Find a new place in the list. */
  for (ptp = &tty_timelist; *ptp != NULL; ptp = &(*ptp)->tty_timenext) {
	if (tp->tty_time <= (*ptp)->tty_time) break;
  }
  tp->tty_timenext = *ptp;
  *ptp = tp;
  if (tp->tty_time < tty_timeout) tty_timeout = tp->tty_time;
}


#if 0
/*===========================================================================*
 *				sigchar					     *
 *===========================================================================*/
PUBLIC void sigchar(tp, sig)
register tty_t *tp;
int sig;			/* SIGINT, SIGQUIT, SIGKILL or SIGHUP */
{
/* Process a SIGINT, SIGQUIT or SIGKILL char from the keyboard or SIGHUP from
 * a tty close, "stty 0", or a real RS-232 hangup.  MM will send the signal to
 * the process group (INT, QUIT), all processes (KILL), or the session leader
 * (HUP).
 */

  if (tp->tty_pgrp != 0) cause_sig(tp->tty_pgrp, sig);

  if (!(tp->tty_termios.c_lflag & NOFLSH)) {
	tp->tty_incount = tp->tty_eotct = 0;	/* kill earlier input */
	tp->tty_intail = tp->tty_inhead;
	(*tp->tty_ocancel)(tp);			/* kill all output */
	tp->tty_inhibited = RUNNING;
	tp->tty_events = 1;
  }
}


/*==========================================================================*
 *				tty_icancel				    *
 *==========================================================================*/
PRIVATE void tty_icancel(tp)
register tty_t *tp;
{
/* Discard all pending input, tty buffer or device. */

  tp->tty_incount = tp->tty_eotct = 0;
  tp->tty_intail = tp->tty_inhead;
  (*tp->tty_icancel)(tp);
}



/*==========================================================================*
 *				tty_wakeup				    *
 *==========================================================================*/
PUBLIC void tty_wakeup(now)
clock_t now;				/* current time */
{
/* Wake up TTY when something interesting is happening on one of the terminal
 * lines, like a character arriving on an RS232 line, a key being typed, or
 * a timer on a line expiring by TIME.
 */
  tty_t *tp;

  /* Scan the timerlist for expired timers and compute the next timeout time. */
  tty_timeout = TIME_NEVER;
  while ((tp = tty_timelist) != NULL) {
	if (tp->tty_time > now) {
		tty_timeout = tp->tty_time;	/* this timer is next */
		break;
	}
	tp->tty_min = 0;			/* force read to succeed */
	tp->tty_events = 1;
	tty_timelist = tp->tty_timenext;
  }

  /* Let TTY know there is something afoot. */
  interrupt(TTY);

}
void interrupt(struct thread *thread)
{
	  message msg;
	  tty_msg_t *pTTYMsg;

	  pTTYMsg = malloc (sizeof(tty_msg));
	  if (pTTYMsg == NULL)
		  return;

	  msg.data = pTTYMsg;
	  msg.m_type = code;

	  pTTYMsg->m_source = HARDWARE;
	  pTTYMsg->m_type = HARD_INT;

	  message_send(&msg,thread);
}
/*==========================================================================*
 *				tty_devnop				    *
 *==========================================================================*/
PUBLIC void tty_devnop(tp)
tty_t *tp;
{
  /* Some functions need not be implemented at the device level. */
}


#if ENABLE_SRCCOMPAT || ENABLE_BINCOMPAT
/*===========================================================================*
 *				compat_getp				     *
 *===========================================================================*/
PRIVATE int compat_getp(tp, sg)
tty_t *tp;
struct sgttyb *sg;
{
/* Translate an old TIOCGETP to the termios equivalent. */
  int flgs;

  sg->sg_erase = tp->tty_termios.c_cc[VERASE];
  sg->sg_kill = tp->tty_termios.c_cc[VKILL];
  sg->sg_ospeed = tspd2sgspd(cfgetospeed(&tp->tty_termios));
  sg->sg_ispeed = tspd2sgspd(cfgetispeed(&tp->tty_termios));

  flgs = 0;

  /* XTABS	- if OPOST and XTABS */
  if ((tp->tty_termios.c_oflag & (OPOST|XTABS)) == (OPOST|XTABS))
	flgs |= 0006000;

  /* BITS5..BITS8  - map directly to CS5..CS8 */
  flgs |= (tp->tty_termios.c_cflag & CSIZE) << (8-2);

  /* EVENP	- if PARENB and not PARODD */
  if ((tp->tty_termios.c_cflag & (PARENB|PARODD)) == PARENB)
	flgs |= 0000200;

  /* ODDP	- if PARENB and PARODD */
  if ((tp->tty_termios.c_cflag & (PARENB|PARODD)) == (PARENB|PARODD))
	flgs |= 0000100;

  /* RAW	- if not ICANON and not ISIG */
  if (!(tp->tty_termios.c_lflag & (ICANON|ISIG)))
	flgs |= 0000040;

  /* CRMOD	- if ICRNL */
  if (tp->tty_termios.c_iflag & ICRNL)
	flgs |= 0000020;

  /* ECHO	- if ECHO */
  if (tp->tty_termios.c_lflag & ECHO)
	flgs |= 0000010;

  /* CBREAK	- if not ICANON and ISIG */
  if ((tp->tty_termios.c_lflag & (ICANON|ISIG)) == ISIG)
	flgs |= 0000002;

  sg->sg_flags = flgs;
  return(OK);
}


/*===========================================================================*
 *				compat_getc				     *
 *===========================================================================*/
PRIVATE int compat_getc(tp, tc)
tty_t *tp;
struct tchars *tc;
{
/* Translate an old TIOCGETC to the termios equivalent. */

  tc->t_intrc = tp->tty_termios.c_cc[VINTR];
  tc->t_quitc = tp->tty_termios.c_cc[VQUIT];
  tc->t_startc = tp->tty_termios.c_cc[VSTART];
  tc->t_stopc = tp->tty_termios.c_cc[VSTOP];
  tc->t_brkc = tp->tty_termios.c_cc[VEOL];
  tc->t_eofc = tp->tty_termios.c_cc[VEOF];
  return(OK);
}


/*===========================================================================*
 *				compat_setp				     *
 *===========================================================================*/
PRIVATE int compat_setp(tp, sg)
tty_t *tp;
struct sgttyb *sg;
{
/* Translate an old TIOCSETP to the termios equivalent. */
  struct termios termios;
  int flags;

  termios = tp->tty_termios;

  termios.c_cc[VERASE] = sg->sg_erase;
  termios.c_cc[VKILL] = sg->sg_kill;
  cfsetispeed(&termios, sgspd2tspd(sg->sg_ispeed & BYTE));
  cfsetospeed(&termios, sgspd2tspd(sg->sg_ospeed & BYTE));
  flags = sg->sg_flags;

  /* Input flags */

  /* BRKINT	- not changed */
  /* ICRNL	- set if CRMOD is set and not RAW */
  /*		  (CRMOD also controls output) */
  termios.c_iflag &= ~ICRNL;
  if ((flags & 0000020) && !(flags & 0000040))
	termios.c_iflag |= ICRNL;

  /* IGNBRK	- not changed */
  /* IGNCR	- forced off (ignoring cr's is not supported) */
  termios.c_iflag &= ~IGNCR;

  /* IGNPAR	- not changed */
  /* INLCR	- forced off (mapping nl's to cr's is not supported) */
  termios.c_iflag &= ~INLCR;

  /* INPCK	- not changed */
  /* ISTRIP	- not changed */
  /* IXOFF	- not changed */
  /* IXON	- forced on if not RAW */
  termios.c_iflag &= ~IXON;
  if (!(flags & 0000040))
	termios.c_iflag |= IXON;

  /* PARMRK	- not changed */

  /* Output flags */

  /* OPOST	- forced on if not RAW */
  termios.c_oflag &= ~OPOST;
  if (!(flags & 0000040))
	termios.c_oflag |= OPOST;

  /* ONLCR	- forced on if CRMOD */
  termios.c_oflag &= ~ONLCR;
  if (flags & 0000020)
	termios.c_oflag |= ONLCR;

  /* XTABS	- forced on if XTABS */
  termios.c_oflag &= ~XTABS;
  if (flags & 0006000)
	termios.c_oflag |= XTABS;

  /* CLOCAL	- not changed */
  /* CREAD	- forced on (receiver is always enabled) */
  termios.c_cflag |= CREAD;

  /* CSIZE	- CS5-CS8 correspond directly to BITS5-BITS8 */
  termios.c_cflag = (termios.c_cflag & ~CSIZE) | ((flags & 0001400) >> (8-2));

  /* CSTOPB	- not changed */
  /* HUPCL	- not changed */
  /* PARENB	- set if EVENP or ODDP is set */
  termios.c_cflag &= ~PARENB;
  if (flags & (0000200|0000100))
	termios.c_cflag |= PARENB;

  /* PARODD	- set if ODDP is set */
  termios.c_cflag &= ~PARODD;
  if (flags & 0000100)
	termios.c_cflag |= PARODD;

  /* Local flags */

  /* ECHO		- set if ECHO is set */
  termios.c_lflag &= ~ECHO;
  if (flags & 0000010)
	termios.c_lflag |= ECHO;

  /* ECHOE	- not changed */
  /* ECHOK	- not changed */
  /* ECHONL	- not changed */
  /* ICANON	- set if neither CBREAK nor RAW */
  termios.c_lflag &= ~ICANON;
  if (!(flags & (0000002|0000040)))
	termios.c_lflag |= ICANON;

  /* IEXTEN	- set if not RAW */
  /* ISIG	- set if not RAW */
  termios.c_lflag &= ~(IEXTEN|ISIG);
  if (!(flags & 0000040))
	termios.c_lflag |= (IEXTEN|ISIG);

  /* NOFLSH	- not changed */
  /* TOSTOP	- not changed */

  tp->tty_termios = termios;
  setattr(tp);
  return(OK);
}


/*===========================================================================*
 *				compat_setc				     *
 *===========================================================================*/
PRIVATE int compat_setc(tp, tc)
tty_t *tp;
struct tchars *tc;
{
/* Translate an old TIOCSETC to the termios equivalent. */
  struct termios termios;

  termios = tp->tty_termios;

  termios.c_cc[VINTR] = tc->t_intrc;
  termios.c_cc[VQUIT] = tc->t_quitc;
  termios.c_cc[VSTART] = tc->t_startc;
  termios.c_cc[VSTOP] = tc->t_stopc;
  termios.c_cc[VEOL] = tc->t_brkc;
  termios.c_cc[VEOF] = tc->t_eofc;

  tp->tty_termios = termios;
  setattr(tp);
  return(OK);
}


/* Table of termios line speed to sgtty line speed translations.   All termios
 * speeds are present even if sgtty didn't know about them.  (Now it does.)
 */
PRIVATE struct s2s {
  speed_t	tspd;
  u8_t		sgspd;
} ts2sgs[] = {
  { B0,		  0 },
  { B50,	 50 },
  { B75,	 75 },
  { B110,	  1 },
  { B134,	134 },
  { B200,	  2 },
  { B300,	  3 },
  { B600,	  6 },
  { B1200,	 12 },
  { B1800,	 18 },
  { B2400,	 24 },
  { B4800,	 48 },
  { B9600,	 96 },
  { B19200,	192 },
  { B38400,	195 },
  { B57600,	194 },
  { B115200,	193 },
};

/*===========================================================================*
 *				tspd2sgspd				     *
 *===========================================================================*/
PRIVATE int tspd2sgspd(tspd)
speed_t tspd;
{
/* Translate a termios speed to sgtty speed. */
  struct s2s *s;

  for (s = ts2sgs; s < ts2sgs + sizeof(ts2sgs)/sizeof(ts2sgs[0]); s++) {
	if (s->tspd == tspd) return(s->sgspd);
  }
  return 96;
}


/*===========================================================================*
 *				sgspd2tspd				     *
 *===========================================================================*/
PRIVATE speed_t sgspd2tspd(sgspd)
int sgspd;
{
/* Translate a sgtty speed to termios speed. */
  struct s2s *s;

  for (s = ts2sgs; s < ts2sgs + sizeof(ts2sgs)/sizeof(ts2sgs[0]); s++) {
	if (s->sgspd == sgspd) return(s->tspd);
  }
  return B9600;
}


#if ENABLE_BINCOMPAT
/*===========================================================================*
 *				do_ioctl_compat				     *
 *===========================================================================*/
PRIVATE void do_ioctl_compat(tp, m_ptr)
tty_t *tp;
tty_msg *m_ptr;
{
/* Handle the old sgtty ioctl's that packed the sgtty or tchars struct into
 * the Minix tty_msg.  Efficient then, troublesome now.
 */
  int minor, proc, func, result, r;
  long flags, erki, spek;
  u8_t erase, kill, intr, quit, xon, xoff, brk, eof, ispeed, ospeed;
  struct sgttyb sg;
  struct tchars tc;
  tty_msg reply_mess;

  minor = m_ptr->TTY_LINE;
  proc = m_ptr->PROC_NR;
  func = m_ptr->REQUEST;
  spek = m_ptr->m2_l1;
  flags = m_ptr->m2_l2;

  switch(func)
  {
    case (('t'<<8) | 8):	/* TIOCGETP */
	r = compat_getp(tp, &sg);
	erase = sg.sg_erase;
	kill = sg.sg_kill;
	ispeed = sg.sg_ispeed;
	ospeed = sg.sg_ospeed;
	flags = sg.sg_flags;
	erki = ((long)ospeed<<24) | ((long)ispeed<<16) | ((long)erase<<8) |kill;
	break;
    case (('t'<<8) | 18):	/* TIOCGETC */
	r = compat_getc(tp, &tc);
	intr = tc.t_intrc;
	quit = tc.t_quitc;
	xon = tc.t_startc;
	xoff = tc.t_stopc;
	brk = tc.t_brkc;
	eof = tc.t_eofc;
	erki = ((long)intr<<24) | ((long)quit<<16) | ((long)xon<<8) | xoff;
	flags = (eof << 8) | brk;
	break;
    case (('t'<<8) | 17):	/* TIOCSETC */
	tc.t_stopc = (spek >> 0) & 0xFF;
	tc.t_startc = (spek >> 8) & 0xFF;
	tc.t_quitc = (spek >> 16) & 0xFF;
	tc.t_intrc = (spek >> 24) & 0xFF;
	tc.t_brkc = (flags >> 0) & 0xFF;
	tc.t_eofc = (flags >> 8) & 0xFF;
	r = compat_setc(tp, &tc);
	break;
    case (('t'<<8) | 9):	/* TIOCSETP */
	sg.sg_erase = (spek >> 8) & 0xFF;
	sg.sg_kill = (spek >> 0) & 0xFF;
	sg.sg_ispeed = (spek >> 16) & 0xFF;
	sg.sg_ospeed = (spek >> 24) & 0xFF;
	sg.sg_flags = flags;
	r = compat_setp(tp, &sg);
	break;
    default:
	r = ENOTTY;
  }
  reply_mess.m_type = TASK_REPLY;
  reply_mess.REP_PROC_NR = m_ptr->PROC_NR;
  reply_mess.REP_STATUS = r;
  reply_mess.m2_l1 = erki;
  reply_mess.m2_l2 = flags;
  send(m_ptr->m_source, &reply_mess);
}
#endif /* ENABLE_BINCOMPAT */
#endif /* ENABLE_SRCCOMPAT || ENABLE_BINCOMPAT */
#else

PUBLIC tty_t* getTTY(int line) {
	tty_t* tp;

	if ((line - CONS_MINOR) < CONFIG_NR_CONS) {
		tp = tty_addr(line - CONS_MINOR);
	} else
	if (line == LOG_MINOR) {
		tp = tty_addr(0);
	} else
	if ((line - RS232_MINOR) < CONFIG_NR_RS_LINES) {
		tp = tty_addr(line - RS232_MINOR + CONFIG_NR_CONS);
	} else
	if ((line - TTYPX_MINOR) < CONFIG_NR_PTYS) {
		tp = tty_addr(line - TTYPX_MINOR + CONFIG_NR_CONS + CONFIG_NR_RS_LINES);
	} else
	if ((line - PTYPX_MINOR) < CONFIG_NR_PTYS) {
		tp = tty_addr(line - PTYPX_MINOR + CONFIG_NR_CONS + CONFIG_NR_RS_LINES);
	} else {
		tp = NULL;
	}
	return tp;
}

PUBLIC tty_t* getTTY_Cons(int line) {
	tty_t* tp;

	if ((line - CONS_MINOR) < CONFIG_NR_CONS) {
		tp = tty_addr(line - CONS_MINOR);
	} else {
		tp = NULL;
	}
	return tp;
}
PUBLIC tty_t* getTTY_Log(int line) {
	tty_t* tp;

	if (line == LOG_MINOR) {
		tp = tty_addr(0);
	} else {
		tp = NULL;
	}
	return tp;
}
PUBLIC tty_t* getTTY_Rs232(int line) {
	tty_t* tp;

	if ((line - RS232_MINOR) < CONFIG_NR_RS_LINES) {
		tp = tty_addr(line - RS232_MINOR + CONFIG_NR_CONS);
	} else {
		tp = NULL;
	}
	return tp;
}
PUBLIC tty_t* getTTY_TTYPX(int line) {
	tty_t* tp;

	if ((line - TTYPX_MINOR) < CONFIG_NR_PTYS) {
		tp = tty_addr(line - TTYPX_MINOR + CONFIG_NR_CONS + CONFIG_NR_RS_LINES);
	} else {
		tp = NULL;
	}
	return tp;
}
PUBLIC tty_t* getTTY_PTYPX(int line) {
	tty_t* tp;

	if ((line - PTYPX_MINOR) < CONFIG_NR_PTYS) {
		tp = tty_addr(line - PTYPX_MINOR + CONFIG_NR_CONS + CONFIG_NR_RS_LINES);
	} else {
		tp = NULL;
	}
	return tp;
}

#endif
