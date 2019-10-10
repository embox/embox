/**
 * @file tftp.h
 * @brief Interface for TFTP operations. Based on old version of cmds/net/tftp.c
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 * @version
 * @date 18.06.2019
 */

#ifndef LIB_TFTP_H_
#define LIB_TFTP_H_

#include <stdbool.h>
#include <stdint.h>

/*
 * Trivial File Transfer Protocol (IEN-133)
 */
#define TFTP_SEGSIZE 512                /* data segment size */

#define TFTP_TRANSFER_PORT 69 /* default well known port */

/*
 * Packet types.
 */
#define TFTP_RRQ     1	/* read request */
#define TFTP_WRQ     2	/* write request */
#define TFTP_DATA    3	/* data packet */
#define TFTP_ACK     4	/* acknowledgement */
#define TFTP_ERROR   5	/* error code */
#define TFTP_OACK    6	/* options acknowledgment */

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

#define TFTP_DIR_PUT 0x1
#define TFTP_DIR_GET 0x2

struct tftp_stream;

extern struct tftp_stream *tftp_new_stream(const char *host, const char *file, int dir, bool binary_mode);
extern int tftp_delete_stream(struct tftp_stream *s);

extern int tftp_stream_write(struct tftp_stream *s, uint8_t *buf, size_t len);
extern int tftp_stream_read(struct tftp_stream *s, uint8_t *buf);

extern const char *tftp_error(struct tftp_stream *s);

#endif /* LIB_TFTP_H_ */
