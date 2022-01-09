/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    10.06.2020
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <drivers/usb/usb.h>
#include <drivers/usb/class/ccid.h>

/* Global counter for all CCID requests. */
static int b_seq;

static void print_usage(void) {
	printf("Usage: ccid_cmd [-h] <usb bus> <usb device>\n");
	printf("\t[-h]      - print this help\n");
	printf("\n\t Example:\n");
	printf("\t    ccid_cmd 0 1: Works with CCID device on bus 0, addr 1.\n");
}

static void ccid_cmd_show_atr(struct usb_interface *udev) {
	struct ccid_msg_hdr_icc_power_on *pwr_on_hdr;
	struct ccid_msg_hdr *hdr;
	uint8_t buf[64];
	uint8_t msg_hdr_buf[CCID_MSG_HEADER_SIZE];
	int i;

	pwr_on_hdr = (struct ccid_msg_hdr_icc_power_on *) msg_hdr_buf;

	pwr_on_hdr->hdr.b_message_type = CCID_MESSAGE_TYPE_PC_to_RDR_IccPowerOn;
	pwr_on_hdr->hdr.dw_length = 0;
	pwr_on_hdr->hdr.b_slot = 0;
	pwr_on_hdr->hdr.b_seq = b_seq++;
	pwr_on_hdr->b_power_select = 0; /* automatic voltage selection */

	ccid_handle_msg(udev, pwr_on_hdr, buf, sizeof buf);

	hdr = (struct ccid_msg_hdr *) buf;
	assert(hdr->b_message_type == CCID_MESSAGE_TYPE_RDR_to_PC_DataBlock);

	printf("ATR: ");
	for (i = 0; i < hdr->dw_length; i++) {
		printf("%02x ", buf[CCID_MSG_HEADER_SIZE + i]);
	}
	printf("\n");
}

static int ccid_cmd_card_status(struct usb_interface *udev) {
	struct ccid_msg_hdr *hdr;
	struct ccid_msg_hdr_bulk_in *bulk_in_hdr;
	uint8_t buf[64];
	uint8_t msg_hdr_buf[CCID_MSG_HEADER_SIZE];

	hdr = (struct ccid_msg_hdr *) msg_hdr_buf;

	hdr->b_message_type = CCID_MESSAGE_TYPE_PC_to_RDR_GetSlotStatus;
	hdr->dw_length = 0;
	hdr->b_slot = 0;
	hdr->b_seq = b_seq++;

	ccid_handle_msg(udev, hdr, buf, sizeof buf);

	bulk_in_hdr = (struct ccid_msg_hdr_bulk_in *) buf;
	assert(bulk_in_hdr->hdr.b_message_type == CCID_MESSAGE_TYPE_RDR_to_PC_SlotStatus);

	return bulk_in_hdr->b_status & CCID_SLOT_STATUS_MASK;
}

static int ccid_cmd_card_cmd(struct usb_interface *udev, uint8_t *cmd,
		int len) {
	struct ccid_msg_hdr *hdr;
	struct ccid_msg_hdr_bulk_in *bulk_in_hdr;
	uint8_t buf[CCID_MSG_HEADER_SIZE];
	uint8_t *msg_hdr_buf, *data;
	int i;

	msg_hdr_buf = malloc(CCID_MSG_HEADER_SIZE + len);
	if (!msg_hdr_buf) {
		goto out_err;
	}

	hdr = (struct ccid_msg_hdr *) msg_hdr_buf;

	hdr->b_message_type = CCID_MESSAGE_TYPE_PC_to_RDR_XfrBlock;
	hdr->dw_length = len;
	hdr->b_slot = 0;
	hdr->b_seq = b_seq++;
	memcpy(msg_hdr_buf + CCID_MSG_HEADER_SIZE, cmd, len);

	ccid_handle_msg(udev, hdr, buf, CCID_MSG_HEADER_SIZE);

	bulk_in_hdr = (struct ccid_msg_hdr_bulk_in *) buf;
	assert(bulk_in_hdr->hdr.b_message_type == CCID_MESSAGE_TYPE_RDR_to_PC_DataBlock);

	data = malloc(bulk_in_hdr->hdr.dw_length);
	if (!data) {
		goto out_err_free_msg;
	}

	/* Now read the reset of data replied. */
	ccid_read_msg_data(udev, data, bulk_in_hdr->hdr.dw_length);

	printf("Reply: ");
	for (i = 0; i < bulk_in_hdr->hdr.dw_length; i++) {
		printf("0x%02x ", data[i]);
	}
	printf("\n");

	free(msg_hdr_buf);
	free(data);

	return 0;

out_err_free_msg:
	free(msg_hdr_buf);
out_err:
	fprintf(stderr, "ccid_cmd_card_cmd failed\n");
	return -1;
}

static void handle_ccid_commands(struct usb_interface *udev) {
	int card_status;

	printf("\n");

	/* Card status */
	card_status = ccid_cmd_card_status(udev);
	switch (card_status) {
	case CCID_SLOT_STATUS_ACTIVE:
		printf("Card status (slot 0): Inserted\n");
		break;
	case CCID_SLOT_STATUS_INACTIVE:
		printf("Card status (slot 0): Inserted, but inactive (probably not powered)\n");
		return;
	case CCID_SLOT_STATUS_NOT_PRESENT:
		printf("Card status (slot 0): No present\n");
		return;
	}

	/* ATR */
	ccid_cmd_show_atr(udev);

	/* Process user entered commands */
	printf("\nYou can enter command bytes, press enter to send it to smart card:\n"
		   "  For example:\n"
		   "    C0 20 00 00 03 31 32 33 - If you want to input PIN \"123\"\n"
		   "                              (\"C0 20 00 00\" - cmd, 03 - len, \"31 32 33\" - your PIN) \n"
	);

	while (1) {
		char user_input[64];
		uint8_t cmd_buf[64];
		int cmd_len;
		char *str;

		cmd_len = 0;

		fgets(user_input, 64, stdin);
		str = strtok(user_input, " ");
		while (str != NULL) {
			unsigned int c;

			sscanf(str, "%x", &c);
			cmd_buf[cmd_len++] = c;
			str = strtok(NULL, " ");
		}

		ccid_cmd_card_cmd(udev, cmd_buf, cmd_len);
	}
}

int main(int argc, char **argv) {
	struct usb_dev *usb_dev = NULL;
	int opt, ret;
	int bus, addr;

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case '?':
		case 'h':
		default:
			print_usage();
			return 0;
		}
	}

	if (argc < 3) {
		print_usage();
		return 0;
	}

	ret = sscanf(argv[argc - 2], "%d", &bus);
	if (ret < 0) {
		fprintf(stderr, "Bad usb bus number\n");
		print_usage();
		return -1;
	}
	ret = sscanf(argv[argc - 1], "%d", &addr);
	if (ret < 0) {
		fprintf(stderr, "Bad usb device number\n");
		print_usage();
		return -1;
	}

	while ((usb_dev = usb_dev_iterate(usb_dev))) {
		if ((usb_dev->bus_idx != bus) || (usb_dev->addr != addr)) {
			continue;
		}

		/* Make sure it's CCID device */
		if (usb_dev->current_config->usb_iface[0]->iface_desc[0]
			&& (usb_dev->current_config->usb_iface[0]->iface_desc[0]->b_interface_class == USB_CLASS_CCID)) {
			handle_ccid_commands(usb_dev->current_config->usb_iface[0]);
			return 0;
		}
	}

	fprintf(stderr, "No CCID device found: bus=%d, addr=%d\n", bus, addr);

	return -1;
}
