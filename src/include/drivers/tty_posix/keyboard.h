/**
 * @file
 *
 * @date 03.08.2011
 * @author Gerald Hoch
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#define FUNC_KEY_USED 0

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */

/* AT keyboard. */
#define KB_COMMAND	0x64	/* I/O port for commands on AT */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_ACK		0xFA	/* keyboard ack response */
#define KB_DEACTIVATE 0xF5	/* deactivate the keyboard */
#define KB_ACTIVATE	  0xF4	/* activate the keyboard */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_IN_FULL	0x02	/* status bit set when not ready to receive */
#define LED_CODE	0xED	/* command to keyboard to set LEDs */
#define MAX_KB_ACK_RETRIES 0x1000	/* max #times to wait for kb ack */
#define MAX_KB_BUSY_RETRIES 0x1000	/* max #times to loop while kb busy */
#define KBIT		0x80	/* bit used to ack characters to keyboard */

/* Miscellaneous. */
#define ESC_SCAN	0x01	/* Reboot key when panicking */
#define SLASH_SCAN	0x35	/* to recognize numeric slash */
#define RSHIFT_SCAN	0x36	/* to distinguish left and right shift */
#define HOME_SCAN	0x47	/* first key on the numeric keypad */
#define INS_SCAN	0x52	/* INS for use in CTRL-ALT-INS reboot */
#define DEL_SCAN	0x53	/* DEL for use in CTRL-ALT-DEL reboot */
#define CONSOLE		   0	/* line number for console */
#define MEMCHECK_ADR   0x472	/* address to stop memory check after reboot */
#define MEMCHECK_MAG  0x1234	/* magic number to stop memory check */

#define KB_IN_BYTES	  32	/* size of keyboard input buffer */

#define KEYBOARD_IRQ 1

/* Lock key active bits.  Chosen to be equal to the keyboard LED bits. */
#define SCROLL_LOCK	0x01
#define NUM_LOCK	0x02
#define CAPS_LOCK	0x04

/**
 * @brief static functions
 */
FORWARD _PROTOTYPE( int kb_ack, (void) );
FORWARD _PROTOTYPE( int kb_wait, (void) );
#if FUNC_KEY_USED
FORWARD _PROTOTYPE( int func_key, (int scode) );
#endif
FORWARD _PROTOTYPE( int scan_keyboard, (void) );
FORWARD _PROTOTYPE( unsigned make_break, (int scode) );
FORWARD _PROTOTYPE( void set_leds, (void) );
FORWARD _PROTOTYPE( void kb_read, (struct tty *tp) );
FORWARD _PROTOTYPE( unsigned map_key, (int scode) );
FORWARD void kb_init(struct tty *tp);
//FORWARD void kb_reset(void);

/**
 * @brief static vars
 */
PRIVATE char ibuf[KB_IN_BYTES];	/* input buffer */
PRIVATE char *ihead = ibuf;		/* next free spot in input buffer */
PRIVATE char *itail = ibuf;		/* scan code to return to TTY */
PRIVATE int icount;				/* # codes in buffer */
PRIVATE int esc;				/* escape scan code detected? */
PRIVATE int alt_l;				/* left alt key state */
PRIVATE int alt_r;				/* right alt key state */
PRIVATE int alt;				/* either alt key */
PRIVATE int ctrl_l;				/* left control key state */
PRIVATE int ctrl_r;				/* right control key state */
PRIVATE int ctrl;				/* either control key */
PRIVATE int shift_l;			/* left shift key state */
PRIVATE int shift_r;			/* right shift key state */
PRIVATE int shift;				/* either shift key */
PRIVATE int num_down;			/* num lock key depressed */
PRIVATE int caps_down;			/* caps lock key depressed */
PRIVATE int scroll_down;		/* scroll lock key depressed */
PRIVATE int locks[CONFIG_NR_CONS];	/* per console lock keys state */

PRIVATE char numpad_map[] = {'H', 'Y', 'A', 'B', 'D', 'C', 'V', 'U', 'G', 'S', 'T', '@'};

#endif /* KEYBOARD_H_ */
