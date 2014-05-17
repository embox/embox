/**
 * @file
 *
 * @date 26.12.12
 * @author Alexander Kalmuk
 */

#include <termios.h>

#ifndef ASM_TERMBITS_H_
#define ASM_TERMBITS_H_

/* c_cc characters */
#define VERASE 2
#define VSWTC 7
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE 14
#define VLNEXT 15
#define VEOL2 16

/* c_iflag bits */
#define IUCLC 0001000
//#define IXANY 0004000
#define IMAXBEL 0020000

/* c_oflag bits */
#define OLCUC 0000002
//#define ONLCR 0000004
#define OCRNL 0000010
#define ONOCR 0000020
#define ONLRET 0000040
#define OFILL 0000100
#define OFDEL 0000200
#define NLDLY 0000400
#define NL0 0000000
#define NL1 0000400
#define CRDLY 0003000
#define CR0 0000000
#define CR1 0001000
#define CR2 0002000
#define CR3 0003000
#define TABDLY 0014000
#define TAB0 0000000
#define TAB1 0004000
#define TAB2 0010000
#define TAB3 0014000
#define XTABS 0014000
#define BSDLY 0020000
#define BS0 0000000
#define BS1 0020000
#define VTDLY 0040000
#define VT0 0000000
#define VT1 0040000
#define FFDLY 0100000
#define FF0 0000000
#define FF1 0100000

/* c_cflag bit */
#define CBAUD 0010017
#define EXTA B19200
#define EXTB B38400
#define CBAUDEX 0010000
#define B57600 0010001
#define B115200 0010002
#define B230400 0010003
#define B460800 0010004
#define B500000 0010005
#define B576000 0010006
#define B921600 0010007
#define B1000000 0010010
#define B1152000 0010011
#define B1500000 0010012
#define B2000000 0010013
#define B2500000 0010014
#define B3000000 0010015
#define B3500000 0010016
#define B4000000 0010017
#define CIBAUD 002003600000
#define CMSPAR 010000000000
#define CRTSCTS 020000000000

/* c_lflag bits */
#define XCASE 0000004
#define ECHOCTL 0001000
#define ECHOPRT 0002000
#define ECHOKE 0004000
#define FLUSHO 0010000
#define PENDIN 0040000

#endif /* ASM_TERMBITS_H_ */
