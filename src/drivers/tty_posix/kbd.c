/**
 * @file
 *
 * @brief Embox Posix keyboard device interface.
 *
 * @date 02.08.2011
 * @author Gerald Hoch
 *  - Initial implementation
 */

/**
 * CONFIG parameters
 *
 * - TTY_POSIX_CONSOLE
 * - KEYBOARD_MAPPING
 *
 */

/**
 * @brief external function used
 *
 * - tty_in_process()		located in tty.c
 * - tty_select_console() 	located in screen.c
 */

/**
 * @brief external vars used
 *
 *	- tty_table		located in (tty.c)
 *	- tty_ccurrent	located in (tty.c)
 */

/**
 * @brief include system headers
 */
#include <types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
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
 * @brief include TTY specific headers
 */
#include <drivers/tty_posix/keymap.h>
#include <drivers/tty_posix/termios.h>
#include <drivers/tty_posix/tty.h>
#include <drivers/tty_posix/kbd.h>

/**
 * @brief Types
 */

/* Variables and definition for observed function keys. */
typedef struct observer { int proc_nr; int events; } obs_t;
static obs_t  fkey_obs[12];	/* observers for F1-F12 */
static obs_t sfkey_obs[12];	/* observers for SHIFT F1-F12 */

/**
 * @brief static functions
 */
static unsigned map_key					(int scode);
static int 		scan_keyboard			(void);
static void 	set_leds				(void);
static int 		kb_wait					(void);
static int 		kb_ack					(void);
static int 		func_key				(int scode);
static int 		make_break				(int scode);
static irq_return_t	keyboard_int_handler(irq_nr_t irq_num, void *data);
static int 		keyboard_work			( tty_t *tp, int try);
static void 	*open					(const char *fname, const char *mode);
static int 		close					(void *file);
static size_t 	read					(void *buf, size_t size, size_t count, void *file);
static size_t 	write					(const void *buff, size_t size, size_t count, void *file);
static int		ioctl					(void *file, int request, va_list args);
static void 	diag_timer_handler		(sys_timer_t* timer, void *param);
static int 		diag_waiting_key		(int sec);
static tty_t	*getTty					(void* file);
/**
 * @brief static vars
 */
static char ibuf[KB_IN_BYTES];	/* input buffer */
static char *ihead = ibuf;	/* next free spot in input buffer */
static char *itail = ibuf;	/* scan code to return to TTY */
static int 	icount;		/* # codes in buffer */
static int 	esc;		/* escape scan code detected? */
static int 	alt_l;		/* left alt key state */
static int 	alt_r;		/* right alt key state */
static int 	alt;		/* either alt key */
static int 	ctrl_l;		/* left control key state */
static int 	ctrl_r;		/* right control key state */
static int 	ctrl;		/* either control key */
static int 	shift_l;		/* left shift key state */
static int 	shift_r;		/* right shift key state */
static int 	shift;		/* either shift key */
static int 	num_down;		/* num lock key depressed */
static int 	caps_down;		/* caps lock key depressed */
static int 	scroll_down;	/* scroll lock key depressed */
static int 	locks[CONFIG_TTY_POSIX_CONSOLE];	/* per console lock keys state */
static bool diagMode = false;
static int 	diagKey = -1;

static file_operations_t file_op = {
	.fread 	= read,
	.fopen 	= open,
	.fclose = close,
	.fwrite = write,
	.ioctl  = ioctl
};

/**
 * @brief kbd is a device
 */
EMBOX_DEVICE("kbd", &file_op);

/**
 *@brief diag test for keyboard
 */
int tty_posix_kbd_diag_init(void)
{
	static FILE *f;
	int rc;
	const int timeout = 10;

	diagMode = true;
	diagKey = -1;

	f = fopen("/dev/kbd", "r");
	if (NULL == f) {
			LOG_ERROR("/dev/kbd has not registered!\n");
			diagMode = false;
			diagKey = -1;
			return -1;
	}

	rc = diag_waiting_key(timeout);

	fclose(f);

	diagMode = false;

	return rc;
}

static int diag_waiting_key(int sec) {
	sys_timer_t timer;
	bool timer_elapsed = false;
	int ch;

	printf("press 'k' ");

	if (timer_init(&timer, sec*1000, diag_timer_handler, (void *)&timer_elapsed)) {
		LOG_ERROR("Failed to install timer!\n");
	}

	// timer installed and started, now wait....
	for (int i = 0; i < sec*2; i++) {

		printf(".");
		usleep(1000);

		// wait a sec and proof
		if (timer_elapsed) {
			break;
		}
		if (diagKey != -1)
			break;
	}
	timer_close(&timer);

	if (timer_elapsed)
		return -1;

	if (diagKey == -1) {
		return -2;
	}

	ch = map_key(diagKey &= ASCII_MASK);		/* map to ASCII */
	if (ch == 'k')
		return 0;

	return -3;
}
static void diag_timer_handler(sys_timer_t* timer, void *param) {
	*(bool *)param = true;
}

/**
 * @brief get tty structure from device
 *
 * @param file node_t*
 * @retval tty_t*
 */
static tty_t * getTty(void* file) {
	node_t 	*nod;
	tty_t *tp;

	nod = (node_t *) file;
	tp	= (tty_t*) nod->attr;

	return tp;
}

/*
 * file_operation
 */
static void *open(const char *fname, const char *mode) {

	if (strcmp(strupr((char *)mode),"R") != 0)
		return NULL;

	irq_attach((irq_nr_t) KEYBOARD_IRQ,keyboard_int_handler, 0, NULL, "keyboard");
	set_leds();			/* turn off numlock led */
	scan_keyboard();		/* discard leftover keystroke */
	diagKey = -1;

	// Clear the function key observers array. Also see func_key().
	for (int i=0; i<12; i++) {
	  fkey_obs[i].proc_nr 	= 0;	/* F1-F12 observers */
	  fkey_obs[i].events 	= 0;		/* F1-F12 observers */
	  sfkey_obs[i].proc_nr 	= 0;	/* Shift F1-F12 observers */
	  sfkey_obs[i].events 	= 0;		/* Shift F1-F12 observers */
	}

	return (void *)&file_op;
}

static int close(void *file) {
	//tty_t *tp = getTty(file);
	irq_detach((irq_nr_t) KEYBOARD_IRQ,NULL);
	return 0;
}

static size_t read(void *buf, size_t size, size_t count, void *file) {
	tty_t *tp = getTty(file);
	tp = &tty_table[tty_ccurrent]; //overrule

	return keyboard_work(tp,false);
}

static size_t write(const void *buff, size_t size, size_t count, void *file) {
	//tty_t *tp = getTty(file);
	return 0;
}

static int ioctl(void *file, int request, va_list ar) {
	va_list 	args;
	tty_t *tp_out;
	tty_t *tp_base = getTty(file);

	va_copy(args, ar);
	tp_out = (tty_t *) va_arg(args, unsigned long);
	va_end(args);

	*tp_out = *tp_base;

	return 0;
}
/**
 * @brief Keyboard Interrupt handler
 */
irq_return_t keyboard_int_handler(irq_nr_t irq_num, void *data) {
	int scode;

	/**
	 * A keyboard interrupt has occurred.  Process it.
	 * Fetch the character from the keyboard hardware and acknowledge it.
	 */
	scode = scan_keyboard();

	if (!diagMode) {
		/* Store the scancode in memory so the task can get at it later. */
		if (icount < KB_IN_BYTES) {
			*ihead++ = scode;
			if (ihead == ibuf + KB_IN_BYTES) ihead = ibuf;
			icount++;

			tty_table[tty_ccurrent].tty_events = 1;
		}
	} else
	{
		diagKey = scode;
	}

	//TODO send a message to TTY
	return IRQ_HANDLED;
}
/**
 * @note Keyboard read, called from read(), fills the character into the TTY Queues
 * @see read()
 *
 * @param tp
 * @param try, test if chars are waiting, do nothing
 *
 * @retval 0 no character available
 * @retval 1 character available
 */
 static int keyboard_work(tty_t *tp, bool try) {
/* Process characters from the circular keyboard buffer. */
  char buf[3];
  int scode;
  unsigned ch;

  if (try) {
  	if (icount > 0) return 1;
  	return 0;
  }

  while (icount > 0) {
	scode = *itail++;			/* take one key scan code */
	if (itail == ibuf + KB_IN_BYTES) itail = ibuf;
	icount--;

	/* Function keys are being used for debug dumps. */
	if (func_key(scode)) continue;

	/* Perform make/break processing. */
	ch = make_break(scode);

	if (ch <= 0xFF) {
		/* A normal character. */
		buf[0] = ch;
		tty_in_process(tp, buf, 1);
	} else
	if (HOME <= ch && ch <= INSRT) {
		/* An ASCII escape sequence generated by the numeric pad. */
		buf[0] = ESC;
		buf[1] = '[';
		buf[2] = numpad_map[ch - HOME];
		tty_in_process(tp, buf, 3);
	} else
	if (ch == ALEFT) {
		/* Choose lower numbered console as current console. */
		tty_select_console(tty_ccurrent - 1);
		set_leds();
	} else
	if (ch == ARIGHT) {
		/* Choose higher numbered console as current console. */
		tty_select_console(tty_ccurrent + 1);
		set_leds();
	} else
	if (AF1 <= ch && ch <= AF12) {
		/* Alt-F1 is console, Alt-F2 is ttyc1, etc. */
		tty_select_console(ch - AF1);
		set_leds();
	}
  }

  return 1;
}


/**
 * @brief scan the keyboard, called from INT
 */
 int scan_keyboard(void) {
	/* Fetch the character from the keyboard hardware and acknowledge it. */
	u8_t readBuffer, statusBuffer;

	readBuffer 	= inb(KEYBD);
	statusBuffer= inb(PORT_B);

	outb(PORT_B,statusBuffer | KBIT);	/* strobe bit high */
	outb(PORT_B,statusBuffer);			/* then strobe low */

	return readBuffer;		/* return scan code */
}
/**
 * @brief map the keys
 * @param scode rawdata from keyboard
 * @retval keycode
 */
 unsigned map_key(int scode) {
/* Map a scan code to an ASCII code. */

  int caps, column, lk;
  u16_t *keyrow;

  if (scode == SLASH_SCAN && esc) return '/';	/* don't map numeric slash */

  keyrow = (u16_t *)&keymap[scode * MAP_COLS];

  caps = shift;
  lk = locks[tty_ccurrent];
  if ((lk & NUM_LOCK) && HOME_SCAN <= scode && scode <= DEL_SCAN) caps = !caps;
  if ((lk & CAPS_LOCK) && (keyrow[0] & HASCAPS)) caps = !caps;

  if (alt) {
	column = 2;
	if (ctrl || alt_r) column = 3;	/* Ctrl + Alt == AltGr */
	if (caps) column = 4;
  } else {
	column = 0;
	if (caps) column = 1;
	if (ctrl) column = 5;
  }
  return keyrow[column] & ~HASCAPS;
}
/**
 * @brief Set the Leds on the Keyboard
 */
 void set_leds(void) {
/* Set the LEDs on the caps, num, and scroll lock keys */

  kb_wait();			/* wait for buffer empty  */
  outb(KEYBD, LED_CODE);
  kb_ack();				/* wait for ack response  */

  kb_wait();			/* wait for buffer empty  */
  outb(KEYBD, locks[tty_ccurrent]);
  kb_ack();				/* wait for ack response  */
}

/**
 * @brief Wait until the keyboard free
 */
 int kb_wait(void) {
/* Wait until the controller is ready; return zero if this times out. */

  int retries, status, temp;

  retries = MAX_KB_BUSY_RETRIES + 1;	/* wait until not busy */
  do {
	  status = inb(KB_STATUS);
      if (status & KB_OUT_FULL) {
    	  temp = inb(KEYBD);	/* discard value */
      }
      if (! (status & (KB_IN_FULL|KB_OUT_FULL)) )
          break;			/* wait until ready */
  } while (--retries != 0);		/* continue unless timeout */
  return(retries);		/* zero on timeout, positive if ready */
}

/**
 * @brief send an acknoledge to the keyboard
 */
 int kb_ack(void) {
/* Wait until kbd acknowledges last command; return zero if this times out. */

  int retries;
  u8_t u8val;

  retries = MAX_KB_ACK_RETRIES + 1;
  do {
	  u8val = inb(KEYBD);
      if (u8val == KB_ACK)
          break;		/* wait for ack */
  } while(--retries != 0);	/* continue unless timeout */

  return(retries);		/* nonzero if ack received */
}


/**
 * @brief Do a Func_keys call
 * @note This procedure traps function keys for debugging purposes. Observers of
 * function keys are kept in a global array. If a subject (a key) is pressed
 * the observer is notified of the event. Initialization of the arrays is done
 * in kb_init, where NONE is set to indicate there is no interest in the key.
 * Returns FALSE on a key release or if the key is not observable.
 *
 * @param scode rawdata from keyboard
 * @retval 0 not observable
 * @retval 1 observer registered
 */
 int func_key(int scode) {
  int key;
  int proc_nr;

  /* Ignore key releases. If this is a key press, get full key code. */
  if (scode & KEY_RELEASE) return 0;	/* key release */
  key = map_key(scode);		 		/* include modifiers */

  /* Key pressed, now see if there is an observer for the pressed key.
   *	       F1-F12	observers are in fkey_obs array.
   *	SHIFT  F1-F12	observers are in sfkey_req array.
   *	CTRL   F1-F12	reserved (see kb_read)
   *	ALT    F1-F12	reserved (see kb_read)
   * Other combinations are not in use. Note that Alt+Shift+F1-F12 is yet
   * defined in <minix/keymap.h>, and thus is easy for future extensions.
   */
  if (F1 <= key && key <= F12) {		/* F1-F12 */
      proc_nr = fkey_obs[key - F1].proc_nr;
      fkey_obs[key - F1].events ++ ;
  } else if (SF1 <= key && key <= SF12) {	/* Shift F2-F12 */
      proc_nr = sfkey_obs[key - SF1].proc_nr;
      sfkey_obs[key - SF1].events ++;
  }
  else {
      return 0;				/* not observable */
  }

  /* See if an observer is registered and send it a message. */
  if (proc_nr != 0) {
      //TODO send message to kernel
  }
  return 1;
}

 int make_break(int scode) {
/* This routine can handle keyboards that interrupt only on key depression,
 * as well as keyboards that interrupt on key depression and key release.
 * For efficiency, the interrupt routine filters out most key releases.
 */
  int ch, make, escape;
  static int CAD_count = 0;

  /* Check for CTRL-ALT-DEL, and if found, halt the computer. This would
   * be better done in keyboard() in case TTY is hung, except control and
   * alt are set in the high level code.
   */
  if (ctrl && alt && (scode == DEL_SCAN || scode == INS_SCAN))
  {
	if (++CAD_count == 3) //TODO sys_abort(RBT_HALT);
	//TODO sys_kill(INIT_PROC_NR, SIGABRT);
	return -1;
  }

  /* High-order bit set on key release. */
  make = (scode & KEY_RELEASE) == 0;		/* true if pressed */

  ch = map_key(scode &= ASCII_MASK);		/* map to ASCII */

  escape = esc;		/* Key is escaped?  (true if added since the XT) */
  esc = 0;

  switch (ch) {
  	case CTRL:		/* Left or right control key */
		*(escape ? &ctrl_r : &ctrl_l) = make;
		ctrl = ctrl_l | ctrl_r;
		break;
  	case SHIFT:		/* Left or right shift key */
		*(scode == RSHIFT_SCAN ? &shift_r : &shift_l) = make;
		shift = shift_l | shift_r;
		break;
  	case ALT:		/* Left or right alt key */
		*(escape ? &alt_r : &alt_l) = make;
		alt = alt_l | alt_r;
		break;
  	case CALOCK:		/* Caps lock - toggle on 0 -> 1 transition */
		if (caps_down < make) {
			locks[tty_ccurrent] ^= CAPS_LOCK;
			set_leds();
		}
		caps_down = make;
		break;
  	case NLOCK:		/* Num lock */
		if (num_down < make) {
			locks[tty_ccurrent] ^= NUM_LOCK;
			set_leds();
		}
		num_down = make;
		break;
  	case SLOCK:		/* Scroll lock */
		if (scroll_down < make) {
			locks[tty_ccurrent] ^= SCROLL_LOCK;
			set_leds();
		}
		scroll_down = make;
		break;
  	case EXTKEY:		/* Escape keycode */
		esc = 1;		/* Next key is escaped */
		return(-1);
  	default:		/* A normal key */
		if (make) return(ch);
  }

  /* Key release, or a shift type key. */
  return(-1);
}

