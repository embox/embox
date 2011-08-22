/*
 *  Keyboard driver for PC's and AT's.
 *
 * Changed by Marcus Hampel	(04/02/1994)
 *  - Loadable keymaps
 */

/**
 * @file
 *
 *  @brief Embox Posix keyboard device interface.
 *  Changed by Gerald Hoch (20.08.2011)
 *
 */

/**
 * CONFIG parameters used
 *
 * - KEYBOARD_MAPPING
 * - CONFIG_NR_CONS
 *
 */

/**
 * include system headers
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
 * include TTY specific headers
 */
#include <drivers/tty_posix/const.h>
#include <drivers/tty_posix/keymap.h>
#include <drivers/tty_posix/termios.h>
#include <drivers/tty_posix/tty.h>
#include <drivers/tty_posix/keyboard.h>

/**
 * Types
 */

/**
 * static functions
 */

static irq_return_t	keyboard_int_handler(irq_nr_t irq_num, void *data);
static void 	*open					(const char *fname, const char *mode);
static int 		close					(void *file);
static size_t 	read					(void *buf, size_t size, size_t count, void *file);
static size_t 	write					(const void *buff, size_t size, size_t count, void *file);
static int		ioctl					(void *file, int request, va_list args);
static void 	diag_timer_handler		(sys_timer_t* timer, void *param);
static int 		diag_waiting_key		(int sec);
static tty_t	*getTty					(void* file);

/**
 * vars
 */
volatile static bool	diagMode 	= false;		// dont optimize
volatile static int 	diagKey 	= -1;
volatile static int 	ignore_chars= -1;


static file_operations_t file_op = {
	.fread 	= read,
	.fopen 	= open,
	.fclose = close,
	.fwrite = write,
	.ioctl  = ioctl
};

/**
 * keyboard is a device
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

	f = fopen("/dev/kbd", "r");
	if (NULL == f) {
			LOG_ERROR("/dev/kbd has not registered!\n");
			diagMode = false;
			diagKey = -1;
			return -1;
	}

	diagMode = true;
	diagKey = -1;

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

	struct tty *tp = tty_addr(CONSOLE);

	if (strcmp(strupr((char *)mode),"R") != 0)
		return NULL;

	kb_init(tp);

   /* Set interrupt handler and enable keyboard IRQ. */
	irq_attach((irq_nr_t) KEYBOARD_IRQ,keyboard_int_handler, 0, NULL, "keyboard");

	return (void *)&file_op;
}

static int close(void *file) {
	//tty_t *tp = getTty(file);
	irq_detach((irq_nr_t) KEYBOARD_IRQ,NULL);

	return 0;
}

static size_t read(void *buf, size_t size, size_t count, void *file) {
	//tty_t *tp = getTty(file);
	return 0;
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
	int ch;

   /* Fetch the character from the keyboard hardware and acknowledge it. */
   scode = scan_keyboard();
   ch = make_break(scode);

   // key release!
   if (ch == -1) return IRQ_HANDLED;

   if (ignore_chars++ < 0) {
	   return IRQ_HANDLED;
   } else
	   ignore_chars = 0;


   //printf("\r\nINT kbd %d use\r\n",ch);

	if (!diagMode) {
		/* Store the scancode in memory so the task can get at it later. */
		if (icount < KB_IN_BYTES) {
			*ihead++ = scode;
			if (ihead == ibuf + KB_IN_BYTES) ihead = ibuf;
			icount++;
			tty_table[current].tty_events = 1;
			force_timeout();
		}
	} else
	{
		diagKey = scode;
	}

	return IRQ_HANDLED;	/* Reenable keyboard interrupt */
}


 /*===========================================================================*
  *				map_key0				     *
  *===========================================================================*/
 /* Map a scan code to an ASCII code ignoring modifiers. */
 #define map_key0(scode)	 \
 	((unsigned) keymap[(scode) * MAP_COLS])


 /*===========================================================================*
  *				map_key					     *
  *===========================================================================*/
 PRIVATE unsigned map_key(scode)
 int scode;
 {
 /* Map a scan code to an ASCII code. */

   int caps, column, lk;
   u16_t *keyrow;

   if (scode == SLASH_SCAN && esc) return '/';	/* don't map numeric slash */

   keyrow = (u16_t *) &keymap[scode * MAP_COLS];

   caps = shift;
   lk = locks[current];
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



 /*==========================================================================*
  *				kb_read					    *
  *==========================================================================*/
 PRIVATE void kb_read(tp)
 tty_t *tp;
 {
 /* Process characters from the circular keyboard buffer. */

   char buf[3];
   int scode;
   unsigned ch;

   tp = &tty_table[current];		/* always use the current console */

   while (icount > 0) {
 	scode = *itail++;			/* take one key scan code */
 	if (itail == ibuf + KB_IN_BYTES) itail = ibuf;
 	lock();
 	icount--;
 	unlock();
#if FUNC_KEY_USED
 	//TODO actual no function keys implemented
 	/* Function keys are being used for debug dumps. */
 	if (func_key(scode)) continue;
#endif
 	/* Perform make/break processing. */
 	ch = make_break(scode);

 	if (ch <= 0xFF) {
 		/* A normal character. */
 		buf[0] = ch;
 		(void) in_process(tp, buf, 1);
 	} else
 	if (HOME <= ch && ch <= INSRT) {
 		/* An ASCII escape sequence generated by the numeric pad. */
 		buf[0] = ESC;
 		buf[1] = '[';
 		buf[2] = numpad_map[ch - HOME];
 		(void) in_process(tp, buf, 3);
 	} else
 	if (ch == ALEFT) {
 		/* Choose lower numbered console as current console. */
 		select_console(current - 1);
 		set_leds();
 	} else
 	if (ch == ARIGHT) {
 		/* Choose higher numbered console as current console. */
 		select_console(current + 1);
 		set_leds();
 	} else
 	if (AF1 <= ch && ch <= AF12) {
 		/* Alt-F1 is console, Alt-F2 is ttyc1, etc. */
 		select_console(ch - AF1);
 		set_leds();
 	}
   }
 }


 /*===========================================================================*
  *				make_break				     *
  *===========================================================================*/
 PRIVATE unsigned make_break(scode)
 int scode;			/* scan code of key just struck or released */
 {
 /* This routine can handle keyboards that interrupt only on key depression,
  * as well as keyboards that interrupt on key depression and key release.
  * For efficiency, the interrupt routine filters out most key releases.
  */
   int ch, make, escape;

   /* Check for CTRL-ALT-DEL, and if found, halt the computer. This would
    * be better done in keyboard() in case TTY is hung, except control and
    * alt are set in the high level code.
    */
   if (ctrl && alt && (scode == DEL_SCAN || scode == INS_SCAN))
   {
#if FUNC_KEY_USED
   static int CAD_count = 0;
	   //TODO actualy n reboot implemented
 	if (++CAD_count == 3) wreboot(RBT_HALT);
 	cause_sig(INIT_PROC_NR, SIGABRT);
 	return -1;
#endif
   }

   /* High-order bit set on key release. */
   make = (scode & 0200) == 0;		/* true if pressed */

   ch = map_key(scode &= 0177);		/* map to ASCII */

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
 			locks[current] ^= CAPS_LOCK;
 			set_leds();
 		}
 		caps_down = make;
 		break;
   	case NLOCK:		/* Num lock */
 		if (num_down < make) {
 			locks[current] ^= NUM_LOCK;
 			set_leds();
 		}
 		num_down = make;
 		break;
   	case SLOCK:		/* Scroll lock */
 		if (scroll_down < make) {
 			locks[current] ^= SCROLL_LOCK;
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


 /*===========================================================================*
  *				set_leds				     *
  *===========================================================================*/
 PRIVATE void set_leds()
 {
 /* Set the LEDs on the caps, num, and scroll lock keys */

   kb_wait();			/* wait for buffer empty  */
   outb(KEYBD, LED_CODE);	/* prepare keyboard to accept LED values */
   kb_ack();			/* wait for ack response  */

   kb_wait();			/* wait for buffer empty  */
   outb(KEYBD, locks[current]);	/* give keyboard LED values */
   kb_ack();			/* wait for ack response  */
 }


 /*==========================================================================*
  *				kb_wait					    *
  *==========================================================================*/
 PRIVATE int kb_wait()
 {
 /* Wait until the controller is ready; return zero if this times out. */

   int retries, status;

   retries = MAX_KB_BUSY_RETRIES + 1;	/* wait until not busy */
   while (--retries != 0
 		&& (status = inb(KB_STATUS)) & (KB_IN_FULL|KB_OUT_FULL)) {
 	if (status & KB_OUT_FULL) (void) inb(KEYBD);	/* discard */
   }
   return(retries);		/* nonzero if ready */
 }


 /*==========================================================================*
  *				kb_ack					    *
  *==========================================================================*/
 PRIVATE int kb_ack()
 {
 /* Wait until kbd acknowledges last command; return zero if this times out. */

   int retries;

   retries = MAX_KB_ACK_RETRIES + 1;
   while (--retries != 0 && inb(KEYBD) != KB_ACK)
 	;			/* wait for ack */
   return(retries);		/* nonzero if ack received */
 }

 /*===========================================================================*
  *				kb_init					     *
  *===========================================================================*/
PUBLIC void kb_init(tp)
tty_t *tp;
{
	tp->tty_devread = kb_read;	/* Input function */

	set_leds();			/* Turn off numlock led */

	scan_keyboard();		/* Discard leftover keystroke */

}

 /*==========================================================================*
   *				scan_keyboard				    *
   *==========================================================================*/
  PRIVATE int scan_keyboard()
  {
  /* Fetch the character from the keyboard hardware and acknowledge it. */

    int code;

    code = inb(KEYBD);		/* get the scan code for the key struck */
    return code;
  }


 /*===========================================================================*
  *				kbd_loadmap				     *
  *===========================================================================*/
#if 0
 PUBLIC int kbd_loadmap(user_phys)
 phys_bytes user_phys;
 {
 /* Load a new keymap. */

   phys_copy(user_phys, vir2phys(keymap), (phys_bytes) sizeof(keymap));
   return(OK);
 }
#endif

 /*===========================================================================*
  *				func_key				     *
  *===========================================================================*/
#if FUNC_KEY_USED
 PRIVATE int func_key(scode)
 int scode;			/* scan code for a function key */
 {
 /* This procedure traps function keys for debugging and control purposes. */

   unsigned code;

   if (scode & 0200) return(FALSE);		/* key release */
   code = map_key0(scode);			/* first ignore modifiers */
   if (code < F1 || code > F12) return(FALSE);	/* not our job */

   switch (map_key(scode)) {			/* include modifiers */

   case F1:	p_dmp(); break;		/* print process table */
   case F2:	map_dmp(); break;	/* print memory map */
   case F3:	toggle_scroll(); break;	/* hardware vs. software scrolling */

   case F5:				/* network statistics */
 #if ENABLE_DP8390
 		dp8390_dump();
 #endif
 #if ENABLE_RTL8139
 		rtl8139_dump();
 #endif
 		break;
   case CF7:	sigchar(&tty_table[CONSOLE], SIGQUIT); break;
   case CF8:	sigchar(&tty_table[CONSOLE], SIGINT); break;
   case CF9:	sigchar(&tty_table[CONSOLE], SIGKILL); break;
   default:	return(FALSE);
   }
   return(TRUE);
 }
#endif
 /*==========================================================================*
  *				wreboot					    *
  *==========================================================================*/
#if FUNC_KEY_USED
 PUBLIC void wreboot(how)
 int how;		/* 0 = halt, 1 = reboot, 2 = panic!, ... */
 {
 /* Wait for keystrokes for printing debugging info and reboot. */

   int quiet, code;
   static u16_t magic = MEMCHECK_MAG;
   struct tasktab *ttp;

   /* Mask all interrupts. */
   outb(INT_CTLMASK, ~0);

   /* Tell several tasks to stop. */
   cons_stop();
 #if ENABLE_DP8390
   dp8390_stop();
 #endif
 #if ENABLE_RTL8139
   rtl8139_stop();
 #endif
   floppy_stop();
   clock_stop();
 #if ENABLE_DOSFILE
   dosfile_stop();
 #endif

   if (how == RBT_HALT) {
 	printf("System Halted\n");
 	if (!mon_return) how = RBT_PANIC;
   }

   if (how == RBT_PANIC) {
 	/* A panic! */
 	printf("Hit ESC to reboot, F-keys for debug dumps\n");

 	(void) scan_keyboard();	/* ack any old input */
 	quiet = scan_keyboard();/* quiescent value (0 on PC, last code on AT)*/
 	for (;;) {
 		milli_delay(100);	/* pause for a decisecond */
 		code = scan_keyboard();
 		if (code != quiet) {
 			/* A key has been pressed. */
 			if (code == ESC_SCAN) break; /* reboot if ESC typed */
 			(void) func_key(code);	     /* process function key */
 			quiet = scan_keyboard();
 		}
 	}
 	how = RBT_REBOOT;
   }

   if (how == RBT_REBOOT) printf("Rebooting\n");

   if (mon_return && how != RBT_RESET) {
 	/* Reinitialize the interrupt controllers to the BIOS defaults. */
 	intr_init(0);
 	outb(INT_CTLMASK, 0);
 	outb(INT2_CTLMASK, 0);

 	/* Return to the boot monitor. */
 	if (how == RBT_HALT) {
 		phys_copy(vir2phys(""), mon_params, 2);
 	} else
 	if (how == RBT_REBOOT) {
 		phys_copy(vir2phys("delay;boot"), mon_params, 11);
 	}
 	level0(monitor);
   }

   /* Stop BIOS memory test. */
   phys_copy(vir2phys(&magic), (phys_bytes) MEMCHECK_ADR,
 						(phys_bytes) sizeof(magic));

   /* Reset the system by jumping to the reset address (real mode), or by
    * forcing a processor shutdown (protected mode).
    */
   level0(reset);
 }

#endif
