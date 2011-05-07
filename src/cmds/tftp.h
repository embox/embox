/**
 * @file
 * @brief Definitions for the TFTP protocol.
 * @details RFC 1350
 *
 * @date 16.03.09
 * @author Aleksandr Batyukov
 */

#ifndef TFTP_H_
#define TFTP_H_

#include <types.h>
#include <net/ip.h>

/*
 * Trivial File Transfer Protocol (IEN-133)
 */
#define SEGSIZE 512       /* data segment size */
#define PKTSIZE SEGSIZE+4 /* full packet size */

#define TFTP_TRANSFER_PORT 69 /* default well known port */

/*
 * Packet types.
 */
#define RRQ     1	/* read request */
#define WRQ     2	/* write request */
#define DATA    3	/* data packet */
#define ACK     4	/* acknowledgement */
#define ERROR   5	/* error code */
#define OACK    6	/* options acknowledgment */

typedef struct	tftphdr {
	short              th_opcode;   /* packet type */
	union {
		unsigned short tu_block;    /* block # */
#define th_block       th_u.tu_block
		short          tu_code;     /* error code */
#define th_code        th_u.tu_code
		char           tu_stuff[1]; /* request packet stuff */
#define th_stuff       th_u.tu_stuff
	} __attribute__ ((packed)) th_u;
	char               th_data[1];  /* data or error string */
#define th_msg         th_data
} __attribute__ ((packed)) tftphdr_t;

/*
 * File transfer modes
 */
#define TFTP_NETASCII   0	/* Text files */
#define TFTP_OCTET      1	/* Binary files */

/*
 * Errors
 */

/* These initial 7 are passed across the net in "ERROR" packets. */
#define	TFTP_EUNDEF      0	/* not defined */
#define	TFTP_ENOTFOUND   1	/* file not found */
#define	TFTP_EACCESS     2	/* access violation */
#define	TFTP_ENOSPACE    3	/* disk full or allocation exceeded */
#define	TFTP_EBADOP      4	/* illegal TFTP operation */
#define	TFTP_EBADID      5	/* unknown transfer ID */
#define	TFTP_EEXISTS     6	/* file already exists */
#define	TFTP_ENOUSER     7	/* no such user */
/* These extensions are return codes in our API, *never* passed on the net. */
#define TFTP_TIMEOUT     8	/* operation timed out */
#define TFTP_NETERR      9	/* some sort of network error */
#define TFTP_INVALID    10	/* invalid parameter */
#define TFTP_PROTOCOL   11	/* protocol violation */
#define TFTP_TOOLARGE   12	/* file is larger than buffer */

#endif /* TFTP_H_ */
