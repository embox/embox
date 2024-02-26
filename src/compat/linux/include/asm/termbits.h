/**
 * @file
 *
 * @date 26.12.12
 * @author Alexander Kalmuk
 */

#include <termios.h>

#ifndef COMPAT_LINUX_ASM_TERMBITS_H_
#define COMPAT_LINUX_ASM_TERMBITS_H_

/* c_cc characters */
#define VERASE   2
#define VSWTC    7
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE  14
#define VLNEXT   15
#define VEOL2    16

/* c_iflag bits */
#define IUCLC    0001000
//#define IXANY 0004000
#define IMAXBEL  0020000

/* c_oflag bits */
#define OLCUC    0000002
//#define ONLCR 0000004
#define OCRNL    0000010
#define ONOCR    0000020
#define ONLRET   0000040
#define OFILL    0000100
#define OFDEL    0000200
#define NLDLY    0000400
#define NL0      0000000
#define NL1      0000400
#define CRDLY    0003000
#define CR0      0000000
#define CR1      0001000
#define CR2      0002000
#define CR3      0003000
#define TABDLY   0014000
#define TAB0     0000000
#define TAB1     0004000
#define TAB2     0010000
#define TAB3     0014000
#define XTABS    0014000
#define BSDLY    0020000
#define BS0      0000000
#define BS1      0020000
#define VTDLY    0040000
#define VT0      0000000
#define VT1      0040000
#define FFDLY    0100000
#define FF0      0000000
#define FF1      0100000

/* c_cflag bit */
#define CBAUD    0xf000
#define CBAUDEX  0x0000
#define BOTHER   0x0000
#define B57600   0x1000
#define B115200  0x2000
#define B230400  0x3000
#define B460800  0x4000
#define B500000  0x5000
#define B576000  0x6000
#define B921600  0x7000
#define B1000000 0x8000
#define B1152000 0x9000
#define B1500000 0xa000
#define B2000000 0xb000
#define B2500000 0xc000
#define B3000000 0xd000
#define B3500000 0xe000
#define B4000000 0xf000

/* c_lflag bits */
#define XCASE    0000004
#define ECHOCTL  0001000
#define ECHOPRT  0002000
#define ECHOKE   0004000
#define FLUSHO   0010000
#define PENDIN   0040000

struct termios2 {
	tcflag_t c_iflag; /* input mode flags */
	tcflag_t c_oflag; /* output mode flags */
	tcflag_t c_cflag; /* control mode flags */
	tcflag_t c_lflag; /* local mode flags */
	cc_t c_cc[NCCS];  /* control characters */
	cc_t c_line;      /* line discipline */
	speed_t c_ispeed; /* input speed */
	speed_t c_ospeed; /* output speed */
};

#endif /* COMPAT_LINUX_ASM_TERMBITS_H_ */
