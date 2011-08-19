/*
 * tty.c
 *
 *  Created on: 01.08.2011
 *      Author: Gerald Hoch
 */

/**
 * include system headers
 */
#include <types.h>
#include <unistd.h>
#include <string.h>
#include <embox/unit.h>
#include <embox/device.h>
#include <embox/service/callback.h>
#include <asm/io.h>
#include <kernel/panic.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <hal/reg.h>
#include <fs/node.h>
#include <fs/file.h>

/**
 * include TTY specific headers
 */
#include <drivers/tty_posix/termios.h>
#include <drivers/tty_posix/tty.h>

/*
 * Types
 */

/*
 * static functions
 */
static void 	*open		(const char *fname, const char *mode);
static int 		close		(void *file);
static size_t 	read		(void *buf, size_t size, size_t count, void *file);
static size_t 	write		(const void *buff, size_t size, size_t count, void *file);
static int 		ioctl		(void *file, int request, va_list args);

#if 0
static void 	in_transfer	(tty_t *tp);
static int 		echo		(tty_t *tp, int ch);
static void 	rawecho		(tty_t *tp, int ch)	;
static int 		back_over	(tty_t *tp);
static void 	reprint		(tty_t *tp);
static void 	dev_ioctl	(tty_t *tp);
static void 	setattr		(tty_t *tp);
static void 	tty_icancel	(tty_t *tp);
static void 	tty_init	(tty_t *tp);
static 	void 	settimer	(tty_t *tp, int on);
static void 	tty_devnop	(tty_t *tp) ;
#endif

/*
 * static vars
 */

/* Default attributes. */
#if 0
static struct termios termios_defaults = {
  TINPUT_DEF, TOUTPUT_DEF, TCTRL_DEF, TLOCAL_DEF, TSPEED_DEF, TSPEED_DEF,
  {
	TEOF_DEF, TEOL_DEF, TERASE_DEF, TINTR_DEF, TKILL_DEF, TMIN_DEF,
	TQUIT_DEF, TTIME_DEF, TSUSP_DEF, TSTART_DEF, TSTOP_DEF,
	TREPRINT_DEF, TLNEXT_DEF, TDISCARD_DEF,
  },
};
static struct winsize winsize_defaults;	/* = all zeroes */
static tty_t *tty_timelist;	/* list of ttys with active timers */
#endif

static file_operations_t file_op = {
	.fread 	= read,
	.fopen 	= open,
	.fclose = close,
	.fwrite = write,
	.ioctl  = ioctl
};


/*
 * global vars
 */
tty_t tty_table[CONFIG_TTY_POSIX_CONSOLE + CONFIG_TTY_POSIX_RS232 + CONFIG_TTY_POSIX_PSEUDO];
int tty_ccurrent = -1;		/* currently visible console */

//EMBOX_DEVICE("tty", &file_op);
/*
 * device file_operation
 * @return console_t*
 *
 * nod->attr - will be used for console_t* !!!
 */

/////////////////////////////////////////////////////////////////////////////////
// only for testing
int 	tty_in_process		(register tty_t *tp, char *buf, int count) { return 0;}
void tty_select_console(int cons_line) {}
// only for testing
/////////////////////////////////////////////////////////////////////////////////

static void *open(const char *fname, const char *mode) {
#if 0
	register tty_t *tp;
	/* Initialize the terminal lines. */
	for (tp = FIRST_TTY; tp < END_TTY; tp++) {

		tty_init(tp);
	}
#endif
	return (void *)&file_op;
}

static int close(void *file) {
	return 0;
}

static size_t read(void *buf, size_t size, size_t count, void *file) {
	return 0;
}

static size_t write(const void *buff, size_t size, size_t count, void *file) {
	return 0;
}

static int ioctl(void *file, int request, va_list args) {
	return 0;
}
#if 0
/**
 *  Initialize tty structure and call device initialization routines.
 */
static void tty_init(tty_t *tp) {
/*
  tp->tty_intail 	= tp->tty_inhead = tp->tty_inbuf;
  tp->tty_min 		= 1;
  tp->tty_termios 	= termios_defaults;
  tp->tty_icancel 	= tp->tty_ocancel = tp->tty_ioctl = tp->tty_close = tty_devnop;

  if (tp < tty_addr(CONFIG_TTY_POSIX_CONSOLE)) {
	scr_init(tp);
  } else
  if (tp < tty_addr(CONFIG_TTY_POSIX_CONSOLE + CONFIG_TTY_POSIX_RS232)) {
#if CONFIG_TTY_POSIX_RS232 >= 0
	rs_init(tp);
#endif
  } else {
#if CONFIG_TTY_POSIX_PSEUDO >= 0
	pty_init(tp);
#endif
  }
  */
}

static void tty_devnop(tty_t *tp) {
	/* Some functions need not be implemented at the device level. */
}



int 	tty_select_retry	(struct tty *tp) {
	return 0;
}

/*===========================================================================*
 *				in_transfer				     *
 *===========================================================================*/
static void in_transfer(register tty_t *tp) {
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
			phys_copy(buf_phys, user_base + tp->tty_in_vir,(phys_bytes) buflen(buf));
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
	tp->tty_inleft = tp->tty_incum = 0;
  }
}


/**
 * Characters have just been typed in.  Process, save, and echo them.  Return
 * the number of characters processed.
 */
int 	tty_in_process		(register tty_t *tp, char *buf, int count) {

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


static int echo(register tty_t *tp,register int ch) {
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



static void rawecho(register tty_t *tp, int ch) {
/* Echo without interpretation if ECHO is set. */
  int rp = tp->tty_reprint;
  if (tp->tty_termios.c_lflag & ECHO) (*tp->tty_echo)(tp, ch);
  tp->tty_reprint = rp;
}



static int back_over(register tty_t *tp) {
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


static void reprint(register tty_t *tp) {
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

void tty_out_process(tty_t *tp, char *bstart, char *bpos, char *bend, int *icount, int *ocount) {
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

void tty_setattr(tty_t *tp) {
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


void tty_sigchar(register tty_t *tp, int sig) {
/* SIGINT, SIGQUIT, SIGKILL or SIGHUP */
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


static void icancel(register tty_t *tp) {
/* Discard all pending input, tty buffer or device. */

  tp->tty_incount = tp->tty_eotct = 0;
  tp->tty_intail = tp->tty_inhead;
  (*tp->tty_icancel)(tp);
}

void tty_wakeup(clock_t now) {
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

static void settimer(tty_t *tp, int on) {
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

#endif
