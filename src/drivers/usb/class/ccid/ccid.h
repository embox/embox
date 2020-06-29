/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    10.06.2020
 */

#ifndef USB_CLASS_CCID_H_
#define USB_CLASS_CCID_H_

#include <drivers/usb/usb.h>

#define USB_CLASS_CCID     11

/*
 * BULK_OUT messages from PC to Reader
 * Defined in CCID Rev 1.1 6.1 (page 26)
 */
#define CCID_MESSAGE_TYPE_PC_to_RDR_IccPowerOn              0x62
#define CCID_MESSAGE_TYPE_PC_to_RDR_IccPowerOff             0x63
#define CCID_MESSAGE_TYPE_PC_to_RDR_GetSlotStatus           0x65
#define CCID_MESSAGE_TYPE_PC_to_RDR_XfrBlock                0x6f
#define CCID_MESSAGE_TYPE_PC_to_RDR_GetParameters           0x6c
#define CCID_MESSAGE_TYPE_PC_to_RDR_ResetParameters         0x6d
#define CCID_MESSAGE_TYPE_PC_to_RDR_SetParameters           0x61
#define CCID_MESSAGE_TYPE_PC_to_RDR_Escape                  0x6b
#define CCID_MESSAGE_TYPE_PC_to_RDR_IccClock                0x6e
#define CCID_MESSAGE_TYPE_PC_to_RDR_T0APDU                  0x6a
#define CCID_MESSAGE_TYPE_PC_to_RDR_Secure                  0x69
#define CCID_MESSAGE_TYPE_PC_to_RDR_Mechanical              0x71
#define CCID_MESSAGE_TYPE_PC_to_RDR_Abort                   0x72
#define CCID_MESSAGE_TYPE_PC_to_RDR_SetDataRateAndClockFrequency 0x73

/*
 * BULK_IN messages from Reader to PC
 * Defined in CCID Rev 1.1 6.2 (page 48)
 */
#define CCID_MESSAGE_TYPE_RDR_to_PC_DataBlock               0x80
#define CCID_MESSAGE_TYPE_RDR_to_PC_SlotStatus              0x81
#define CCID_MESSAGE_TYPE_RDR_to_PC_Parameters              0x82
#define CCID_MESSAGE_TYPE_RDR_to_PC_Escape                  0x83
#define CCID_MESSAGE_TYPE_RDR_to_PC_DataRateAndClockFrequency 0x84

/* Slot Status (6.2.6) */
#define CCID_SLOT_STATUS_ACTIVE           0
#define CCID_SLOT_STATUS_INACTIVE         1
#define CCID_SLOT_STATUS_NOT_PRESENT      2

#define CCID_SLOT_STATUS_MASK             0x3

/* 6 CCID Messages */
#define CCID_MSG_HEADER_SIZE              10

struct ccid_msg_hdr {
	uint8_t b_message_type;
	uint32_t dw_length;
	uint8_t b_slot;
	uint8_t b_seq;
} __attribute__((packed));

struct ccid_msg_hdr_bulk_in {
	struct ccid_msg_hdr hdr;
	uint8_t b_status;
	uint8_t b_error;
} __attribute__((packed));

struct ccid_msg_hdr_icc_power_on {
	struct ccid_msg_hdr hdr;
	uint8_t b_power_select;
	uint16_t ab_rfu;
} __attribute__((packed));

/* Sends msg and receives reply */
extern int ccid_handle_msg(struct usb_interface *udev, const void *out,
		void *in, int in_cnt);

/* Read the rest (data) of the message, if not all bytes were read with
 * ccid_handle_msg(). For example, you can use ccid_handle_msg()
 * to send CCID_MESSAGE_TYPE_PC_to_RDR_XfrBlock and get only
 * message header of the reply (that is, in_cnt = CCID_MSG_HEADER_SIZE
 * for ccid_handle_msg()).
 * Then you read an actual data size (ccid_msg_hdr.dw_length) from
 * the received header and call ccid_read_msg_data() to read
 * the rest of the message. */
extern int ccid_read_msg_data(struct usb_interface *udev, void *in, int in_cnt);

#endif /* USB_CLASS_CCID_H_ */
