/**
 * \file tftp.h
 *
 * \date May 16, 2009
 * \author sunnya
 */

#ifndef TFTP_H_
#define TFTP_H_

#define TFTP_RRQ	0x1
#define TFTP_WRQ	0x2
#define TFTP_DATA	0x3
#define TFTP_ACK	0x4
#define TFTP_ERR	0x5

#include "types.h"

typedef struct {
	// Message type
	HWRD op;
	// File name. Zero ending string.
	char *filename;
	// Mode. Zero ending string.
	char *mode;
}TFTP_RQ_PACKET;

typedef struct {
	// Message type
	HWRD op;
	// Block number
	HWRD block;
	// Data
	char data[512];
}TFTP_DATA_PACKET;

typedef struct {
	// Message type
	HWRD op;
	// Block number
	HWRD block;
}TFTP_ACK_PACKET;

typedef struct {
	// Message type
	HWRD op;
	// Error number
	HWRD errcode;
	// Error message. Zero ending string.
	char *msg;
}TFTP_ERR_PACKET;

#endif /* TFTP_H_ */
