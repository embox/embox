/**
 * @file
 *
 * @date 03.08.2011
 * @author Gerald Hoch
 */

#ifndef KBD_H_
#define KBD_H_

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define PORT_B   	0x61	/* I/O port for 8255 port B (kbd, beeper...) */

/* AT keyboard. */
#define KB_COMMAND	0x64	/* I/O port for commands on AT */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_ACK		0xFA	/* keyboard ack response */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_IN_FULL	0x02	/* status bit set when not ready to receive */
#define LED_CODE	0xED	/* command to keyboard to set LEDs */
#define MAX_KB_ACK_RETRIES 0x1000	/* max #times to wait for kb ack */
#define MAX_KB_BUSY_RETRIES 0x1000	/* max #times to loop while kb busy */
#define KBIT		0x80	/* bit used to ack characters to keyboard */

/* Miscellaneous. */
#define ESC_SCAN	0x01	/* reboot key when panicking */
#define SLASH_SCAN	0x35	/* to recognize numeric slash */
#define RSHIFT_SCAN	0x36	/* to distinguish left and right shift */
#define HOME_SCAN	0x47	/* first key on the numeric keypad */
#define INS_SCAN	0x52	/* INS for use in CTRL-ALT-INS reboot */
#define DEL_SCAN	0x53	/* DEL for use in CTRL-ALT-DEL reboot */

#define CONSOLE		   0	/* line number for console */
#define KB_IN_BYTES	  32	/* size of keyboard input buffer */

/* Lock key active bits.  Chosen to be equal to the keyboard LED bits. */
#define SCROLL_LOCK	0x01
#define NUM_LOCK	0x02
#define CAPS_LOCK	0x04

static char numpad_map[] = {'H', 'Y', 'A', 'B', 'D', 'C', 'V', 'U', 'G', 'S', 'T', '@'};

extern int tty_posix_kbd_diag_init(void);

#define KEYBOARD_IRQ 1

#endif /* KBD_H_ */
