/**
 * \file tftp.h
 *
 * \date May 16, 2009
 * \author sunnya
 */

#ifndef TFTP_H_
#define TFTP_H_

#include "types.h"

#define TFTP_RRQ	0x1
#define TFTP_WRQ	0x2
#define TFTP_DATA	0x3
#define TFTP_ACK	0x4
#define TFTP_ERR	0x5

/*
 * Error codes.
 */
#define EUNDEF          0    /* not defined */
#define ENOTFOUND       1    /* file not found */
#define EACCESS         2    /* access violation */
#define ENOSPACE        3    /* disk full or allocation exceeded */
#define EBADOP          4    /* illegal TFTP operation */
#define EBADID          5    /* unknown transfer ID */
#define EEXISTS         6    /* file already exists */
#define ENOUSER         7    /* no such user */

typedef struct  _tftphdr {
        short   th_opcode;                   /* packet type */
        union {
                unsigned short  tu_block;    /* block # */                  //in data, ack
                short   tu_code;             /* error code */               //in error
                char    tu_stuff[1];         /* request packet stuff */     //?
        } th_u;
        char    th_data[1];                  /* data or error string */     //data
} __attribute__((packed)) tftphdr;

//typedef struct {
//	HWRD op;        /**< Message type */
//	char *filename; /**< File name. Zero ending string */
//	char *mode;     /**< Mode. Zero ending string. */
//}TFTP_RQ_PACKET;
//
//typedef struct {
//	HWRD op;        /**< Message type */
//	HWRD block;     /**< Block number */
//	char data[512]; /**< Data */
//}TFTP_DATA_PACKET;
//
//typedef struct {
//	HWRD op;        /**< Message type */
//	HWRD block;     /**< Block number */
//}TFTP_ACK_PACKET;
//
//typedef struct {
//	HWRD op;        /**< Message type */
//	HWRD errcode;   /**< Error number */
//	char *msg;      /**< Error message. Zero ending string. */
//}TFTP_ERR_PACKET;

#endif /* TFTP_H_ */
