/**
 * @file
 *
 * @date 16.03.2009
 * @author Aleksandr Batyukov
 */
#ifndef TFTP_H_
#define TFTP_H_

#include <types.h>
#include <net/ip.h>

/*
 * Trivial File Transfer Protocol (IEN-133)
 */
#define	SEGSIZE		512		/* data segment size */

/*
 * Packet types.
 */
#define	RRQ	01			/* read request */
#define	WRQ	02			/* write request */
#define	DATA	03			/* data packet */
#define	ACK	04			/* acknowledgement */
#define	ERROR	05			/* error code */

struct	tftphdr {
	short	th_opcode;		/* packet type */
	union {
		unsigned short	tu_block;	/* block # */
		short	tu_code;	/* error code */
		char	tu_stuff[1];	/* request packet stuff */
	} __attribute__ ((packed)) th_u;
	char	th_data[1];		/* data or error string */
} __attribute__ ((packed));

#define	th_block	th_u.tu_block
#define	th_code		th_u.tu_code
#define	th_stuff	th_u.tu_stuff
#define	th_msg		th_data

/*
 * Error codes.
 */
#define	EUNDEF		0		/* not defined */
#define	ENOTFOUND	1		/* file not found */
#define	EACCESS		2		/* access violation */
#define	ENOSPACE	3		/* disk full or allocation exceeded */
#define	EBADOP		4		/* illegal TFTP operation */
#define	EBADID		5		/* unknown transfer ID */
#define	EEXISTS		6		/* file already exists */
#define	ENOUSER		7		/* no such user */

/*
 * File transfer modes
 */
#define TFTP_NETASCII   0              // Text files
#define TFTP_OCTET      1              // Binary files

/*
 * Errors
 */

// These initial 7 are passed across the net in "ERROR" packets.
#define	TFTP_ENOTFOUND   1   /* file not found */
#define	TFTP_EACCESS     2   /* access violation */
#define	TFTP_ENOSPACE    3   /* disk full or allocation exceeded */
#define	TFTP_EBADOP      4   /* illegal TFTP operation */
#define	TFTP_EBADID      5   /* unknown transfer ID */
#define	TFTP_EEXISTS     6   /* file already exists */
#define	TFTP_ENOUSER     7   /* no such user */
// These extensions are return codes in our API, *never* passed on the net.
#define TFTP_TIMEOUT     8   /* operation timed out */
#define TFTP_NETERR      9   /* some sort of network error */
#define TFTP_INVALID    10   /* invalid parameter */
#define TFTP_PROTOCOL   11   /* protocol violation */
#define TFTP_TOOLARGE   12   /* file is larger than buffer */

/**
 * Read a file from a host into a local buffer.  Returns the
 * number of bytes actually read, or (-1) if an error occurs.
 * On error, *err will hold the reason.
 * This version uses the server name. This can be a name for DNS lookup
 * or a dotty or colony number format for IPv4 or IPv6.
*/
int tftp_client_get(const char * const filename,
                              const in_addr_t server,
                              const int port,
                              char * buff,
                              int len,
                              const int mode,
                              int * const err);

#endif /* TFTP_H_ */
