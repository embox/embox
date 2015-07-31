/**
 * @file usb_dwc_hcd.c
 * @ingroup usbhcd
 *
 * This file contains a USB Host Controller Driver for the Synopsys DesignWare
 * Hi-Speed USB 2.0 On-The-Go Controller.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

/**
 * @addtogroup usbhcd
 *
 * This is a USB Host Controller Driver (HCD) that interfaces with the Synopsys
 * DesignWare Hi-Speed USB 2.0 On-The-Go Controller, henceforth abbreviated as
 * "DWC".  This is the USB Host Controller used on the BCM2835 SoC used on the
 * Raspberry Pi.
 *
 * Please note that there is no publicly available official documentation for
 * this particular piece of hardware, and it uses its own custom host controller
 * interface rather than a standard one such as EHCI.  Therefore, this driver
 * was written on a best-effort basis using several sources to gleam the
 * necessary hardware details, including the extremely complicated and difficult
 * to understand vendor-provided Linux driver.
 *
 * This file implements the Host Controller Driver Interface defined in
 * usb_hcdi.h.  Most importantly, it implements a function to power on and start
 * the host controller (hcd_start()) and a function to send and receive messages
 * over the USB (hcd_submit_xfer_request()).
 *
 * The DWC is controlled by reading and writing to/from memory-mapped registers.
 * The most important registers are the host channel registers.  On this
 * particular hardware, a "host channel", or simply "channel", is a set of
 * registers to which software can read and write to cause transactions to take
 * place on the USB.  A fixed number of host channels exist; on the Raspberry Pi
 * there are 8.  From the software's perspective, transactions using different
 * host channels can be executed at the same time.
 *
 * Some of the host channel registers, as well as other registers, deal with
 * interrupts.  This driver makes use heavy of these and performs all USB
 * transfers in an interrupt-driven manner.  However, due to design flaws in
 * this hardware and in USB 2.0 itself, "interrupt" and "isochronous" transfers
 * still need to make use of software polling when checking for new data, even
 * though each individual transfer is itself interrupt-driven.  This means that,
 * for example, if your USB mouse specifies a polling rate of 100 times per
 * second, then it will, unfortunately, be polled 100 times per second in
 * software.  For more detail about how interrupts can be controlled on this
 * particular hardware, see the comment above dwc_setup_interrupts().
 *
 * Another important concept is the idea of "packets", "transactions", and
 * "transfers".  A USB transfer, such as a single control message or bulk
 * request, may need to be split into multiple packets if it exceeds the
 * endpoint's maximum packet size.  Unfortunately, this has to be dealt with
 * explicitly in this code, as this hardware doesn't do it for us.  But at
 * least, from the viewpoint of this software, a "transaction" is essentially
 * the same as a "packet".
 *
 * The "On-The-Go" in the name of this hardware means that it supports the USB
 * On-The-Go protocol, which allows it to act either as a host or a device.
 * However, we only are concerned with it acting as a host, which simplifies our
 * driver.
 *
 * To simplify the USB core software, a useful design technique (as recommended
 * by the USB 2.0 standard and used in other implementations such as Linux's) is
 * to have the HCD present the root hub as a standard USB hub, even if the root
 * hub is integrated with the host controller and does not appear as a standard
 * hub at the hardware level.  This is the case with the DWC, and we implement
 * this design.  Therefore, some code in this file deals with faking requests
 * sent to the root hub.
 */

#include <unistd.h>
#include <stdbool.h>

#include <drivers/usb/usb.h>
#include <drivers/usb/usb_desc.h>
#include <kernel/irq.h>
#include <kernel/thread/types.h>
#include <kernel/thread.h>
#include "usb_dwc.h"

#include <util/log.h>
#include <util/math.h>
#include <linux/types.h>
#include <hal/ipl.h>
#include <kernel/thread/sync/semaphore.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(usb_dwc_init);


#define IRQ_USB            9
#define USB_FRAMES_PER_MS  1
#define USB_UFRAMES_PER_MS 8

#define DIV_ROUND_UP(num, denom) (((num) + (denom) - 1) / (denom))

/** Round a number up to the next multiple of the word size.  */
#define WORD_ALIGN(n) (((n) + sizeof(ulong) - 1) & ~(sizeof(ulong) - 1))

/** Determines whether a pointer is word-aligned or not.  */
#define IS_WORD_ALIGNED(ptr) ((ulong)(ptr) % sizeof(ulong) == 0)

#define PERIPHERALS_BASE 0x20000000
#define DWC_REGS_BASE          (PERIPHERALS_BASE + 0x980000)
/** Pointer to the memory-mapped registers of the Synopsys DesignWare Hi-Speed
 * USB 2.0 OTG Controller.  */
static volatile struct dwc_regs * const regs = (void*)DWC_REGS_BASE;

/**
 * Maximum packet size of any USB endpoint.  1024 is the maximum allowed by USB
 * 2.0.  Most endpoints will provide maximum packet sizes much smaller than
 * this.
 */
#define USB_MAX_PACKET_SIZE 1024


/**
 * Stack size of USB transfer request scheduler thread (can be fairly small).
 */
#define XFER_SCHEDULER_THREAD_STACK_SIZE 4096

/**
 * Priority of USB transfer request scheduler thread (should be fairly high so
 * that USB transfers can be started as soon as possible).
 */
#define XFER_SCHEDULER_THREAD_PRIORITY 60

/** Name of USB transfer request scheduler thread.  */
#define XFER_SCHEDULER_THREAD_NAME "USB scheduler"

/** Stack size of USB deferred transfer threads (can be fairly small).  */
#define DEFER_XFER_THREAD_STACK_SIZE 4096

/**
 * Priority of USB deferred transfer threads (should be very high since these
 * threads are used for the necessary software polling of interrupt endpoints,
 * which are supposed to have guaranteed bandwidth).
 */
#define DEFER_XFER_THREAD_PRIORITY 100

/**
 * Name of USB defer transfer threads.  Note: including the null-terminator this
 * should be at most TNMLEN, otherwise it will be truncated.
 */
#define DEFER_XFER_THREAD_NAME "USB defer xfer"

/** TODO: remove this if appropriate */
#define START_SPLIT_INTR_TRANSFERS_ON_SOF 1

/** USB packet ID constants recognized by the DWC hardware.  */
enum dwc_usb_pid {
	DWC_USB_PID_DATA0 = 0,
	DWC_USB_PID_DATA1 = 2,
	DWC_USB_PID_DATA2 = 1,
	DWC_USB_PID_SETUP = 3,
};

/** Bitmap of channel free (1) or in-use (0) statuses.  */
static uint chfree;

#if START_SPLIT_INTR_TRANSFERS_ON_SOF
/** Bitmap of channels waiting for start-of-frame  */
static uint sofwait;
#endif

/** Semaphore that tracks the number of free channels in chfree bitmask.  */
static struct sem chfree_sema;

/**
 * Array that holds pointers to the USB transfer request (if any) currently
 * being completed on each hardware channel.
 */
static struct usb_request *channel_pending_xfers[DWC_NUM_CHANNELS];

/** Aligned buffers for DMA.  */
static uint8_t aligned_bufs[DWC_NUM_CHANNELS][WORD_ALIGN(USB_MAX_PACKET_SIZE)] __attribute__((aligned(4)));

/**
 * Returns TRUE if a given USB transfer request is a control request; false
 * otherwise.
 */
static inline bool usb_is_control_request(const struct usb_request *req) {
	return req->endp->type == USB_COMM_CONTROL;
}

/**
 * Returns TRUE if a given USB transfer request is an interrupt request; false
 * otherwise.
 */
static inline bool usb_is_interrupt_request(const struct usb_request *req) {
	return req->endp->type == USB_COMM_INTERRUPT;
}

/* Find index of first set bit in a nonzero word.  */
static inline ulong first_set_bit(ulong word) {
	return 31 - __builtin_clz(word);
}

/**
 * Finds and reserves an unused DWC USB host channel. This is blocking and
 * waits until a channel is available.
 *
 * @return
 *      Index of the free channel.
 */
static uint dwc_get_free_channel(void) {
	uint chan;

	ipl_t ipl = ipl_save();
	semaphore_enter(&chfree_sema);
	chan = first_set_bit(chfree);
	chfree ^= (1 << chan);
	ipl_restore(ipl);

	return chan;
}

/**
 * Marks a channel as free. This signals any thread that may be waiting for a
 * free channel.
 *
 * @param chan
 *      Index of DWC USB host channel to release.
 */
static void dwc_release_channel(uint chan) {
	ipl_t ipl = ipl_save();
	chfree |= (1 << chan);
	semaphore_leave(&chfree_sema);
	ipl_restore(ipl);
}

/**
 * Powers on the DWC hardware.
 */
static enum usb_request_status dwc_power_on(void) {
	// int retval;

	log_info("Powering on Synopsys DesignWare Hi-Speed  USB 2.0 On-The-Go "
			 "Controller");
	// XXX
	//retval = board_setpower(POWER_USB, TRUE);
	// return (retval == OK) ? USB_REQ_NOERR : USB_STATUS_HARDWARE_ERROR;
	return USB_REQ_NOERR;
}

static void dwc_power_off(void) {
	log_info("Powering off Synopsys DesignWare Hi-Speed USB 2.0 On-The-Go "
			 "Controller");
	// XXX
	//board_setpower(POWER_USB, false);
}

/**
 * Performs a software reset of the DWC hardware. Note: the DWC seems to be in
 * a reset state after the initial power on, so this is only strictly necessary
 * when hcd_start() is entered with the DWC already powered on (e.g. when
 * starting a new kernel directly in software with kexec()).
 */
static void dwc_soft_reset(void) {
	log_debug("Resetting USB controller");

	/* Set soft reset flag, then wait until it's cleared.  */
	regs->core_reset = DWC_SOFT_RESET;
	while (regs->core_reset & DWC_SOFT_RESET)
		;
}

/**
 * Set up the DWC OTG USB Host Controller for DMA (direct memory access). This
 * makes it possible for the Host Controller to directly access in-memory
 * buffers when performing USB transfers. Beware: all buffers accessed with DMA
 * must be 4-byte-aligned. Furthermore, if the L1 data cache is enabled, then
 * it must be explicitly flushed to maintain cache coherency since it is
 * internal to the ARM processor. (This is not currently handled by this driver
 * because Xinu does not enable the L1 data cache.)
 */
static void dwc_setup_dma_mode(void) {
	const uint32_t rx_words = 1024;  /* Size of Rx FIFO in 4-byte words */
	const uint32_t tx_words = 1024;  /* Size of Non-periodic Tx FIFO in 4-byte words */
	const uint32_t ptx_words = 1024; /* Size of Periodic Tx FIFO in 4-byte words */

	/* First configure the Host Controller's FIFO sizes. This is _required_
	 * because the default values (at least in Broadcom's instantiation of the
	 * Synopsys USB block) do not work correctly. If software fails to do this,
	 * receiving data will fail in virtually impossible to debug ways that
	 * cause memory corruption. This is true even though we are using DMA and
	 * not otherwise interacting with the Host Controller's FIFOs in this
	 * driver. */
	log_debug("%u words of RAM available for dynamic FIFOs",
		regs->hwcfg3 >> 16);
	log_debug("original FIFO sizes: rx 0x%08x,  tx 0x%08x, ptx 0x%08x",
			  regs->rx_fifo_size, regs->nonperiodic_tx_fifo_size,
			  regs->host_periodic_tx_fifo_size);

	regs->rx_fifo_size = rx_words;
	regs->nonperiodic_tx_fifo_size = (tx_words << 16) | rx_words;
	regs->host_periodic_tx_fifo_size = (ptx_words << 16) | (rx_words + tx_words);

	/* Actually enable DMA by setting the appropriate flag; also set an extra
	 * flag available only in Broadcom's instantiation of the Synopsys USB
	 * block that may or may not actually be needed.  */
	regs->ahb_configuration |= DWC_AHB_DMA_ENABLE | BCM_DWC_AHB_AXI_WAIT;
}

/**
 * Read the Host Port Control and Status register with the intention of
 * modifying it. Due to the inconsistent design of the bits in this register,
 * this requires zeroing the write-clear bits so they aren't unintentionally
 * cleared by writing back 1's to them.
 */
static union dwc_host_port_ctrlstatus dwc_get_host_port_ctrlstatus(void) {
	union dwc_host_port_ctrlstatus hw_status = regs->host_port_ctrlstatus;

	hw_status.enabled = 0;
	hw_status.connected_changed = 0;
	hw_status.enabled_changed = 0;
	hw_status.overcurrent_changed = 0;
	return hw_status;
}

/**
 * Powers on the DWC host port; i.e. the USB port that is logically attached to
 * the root hub.
 */
static void dwc_power_on_host_port(void) {
	union dwc_host_port_ctrlstatus hw_status;

	log_debug("Powering on host port.");
	hw_status = dwc_get_host_port_ctrlstatus();
	hw_status.powered = 1;
	regs->host_port_ctrlstatus = hw_status;
}

/**
 * Resets the DWC host port; i.e. the USB port that is logically attached to
 * the root hub.
 */
static void dwc_reset_host_port(void) {
	union dwc_host_port_ctrlstatus hw_status;

	log_debug("Resetting host port");

	/* Set the reset flag on the port, then clear it after a certain amount of
	 * time.  */
	hw_status = dwc_get_host_port_ctrlstatus();
	hw_status.reset = 1;
	regs->host_port_ctrlstatus = hw_status;
	sleep(60);  /* (sleep for 60 milliseconds).  */
	hw_status.reset = 0;
	regs->host_port_ctrlstatus = hw_status;
}

enum usb_descriptor_type {
	USB_DESCRIPTOR_TYPE_DEVICE        = 1,
	USB_DESCRIPTOR_TYPE_CONFIGURATION = 2,
	USB_DESCRIPTOR_TYPE_STRING        = 3,
	USB_DESCRIPTOR_TYPE_INTERFACE     = 4,
	USB_DESCRIPTOR_TYPE_ENDPOINT      = 5,
	USB_DESCRIPTOR_TYPE_HUB           = 0x29,
};

enum usb_class_code {
	USB_CLASS_CODE_INTERFACE_SPECIFIC             = 0x00,
	USB_CLASS_CODE_AUDIO                          = 0x01,
	USB_CLASS_CODE_COMMUNICATIONS_AND_CDC_CONTROL = 0x02,
	USB_CLASS_CODE_HID                            = 0x03,
	USB_CLASS_CODE_IMAGE                          = 0x06,
	USB_CLASS_CODE_PRINTER                        = 0x07,
	USB_CLASS_CODE_MASS_STORAGE                   = 0x08,
	USB_CLASS_CODE_HUB                            = 0x09,
	USB_CLASS_CODE_VIDEO                          = 0x0e,
	USB_CLASS_CODE_WIRELESS_CONTROLLER            = 0xe0,
	USB_CLASS_CODE_MISCELLANEOUS                  = 0xef,
	USB_CLASS_CODE_VENDOR_SPECIFIC                = 0xff,
};

/** Hard-coded device descriptor for the faked root hub.  */
static const struct usb_desc_device root_hub_device_descriptor = {
	.b_length = sizeof(struct usb_desc_device),
	.b_desc_type = USB_DESCRIPTOR_TYPE_DEVICE,
	.bcd_usb = 0x200, /* USB version 2.0 (binary-coded decimal) */
	.b_dev_class = USB_CLASS_CODE_HUB,
	.b_dev_subclass = 0,
	.b_dev_protocol = 0,
	.b_max_packet_size = 64,
	.id_vendor = 0,
	.id_product = 0,
	.bcd_device = 0,
	.i_manufacter = 0,
	.i_product = 1,
	.i_serial_number = 0,
	.i_num_configurations = 1,
};

#define USB_CONFIGURATION_ATTRIBUTE_RESERVED_HIGH   0x80
#define USB_CONFIGURATION_ATTRIBUTE_SELF_POWERED    0x40
#define USB_CONFIGURATION_ATTRIBUTE_REMOTE_WAKEUP   0x20
#define USB_CONFIGURATION_ATTRIBUTE_RESERVED_LOW    0x1f

enum usb_bmRequestType_fields {
	USB_BMREQUESTTYPE_DIR_OUT             = (USB_DIRECTION_OUT << 7),
	USB_BMREQUESTTYPE_DIR_IN              = (USB_DIRECTION_IN << 7),
	USB_BMREQUESTTYPE_DIR_MASK            = (0x1 << 7),
	USB_BMREQUESTTYPE_TYPE_STANDARD       = (USB_DEV_REQ_TYPE_STD << 5),
	USB_BMREQUESTTYPE_TYPE_CLASS          = (USB_DEV_REQ_TYPE_CLS << 5),
	USB_BMREQUESTTYPE_TYPE_VENDOR         = (USB_DEV_REQ_TYPE_VND << 5),
	// USB_BMREQUESTTYPE_TYPE_RESERVED       = (USB_REQUEST_TYPE_RESERVED << 5),
	USB_BMREQUESTTYPE_TYPE_MASK           = (0x3 << 5),
	USB_BMREQUESTTYPE_RECIPIENT_DEVICE    = (USB_DEV_REQ_TYPE_DEV << 0),
	USB_BMREQUESTTYPE_RECIPIENT_INTERFACE = (USB_DEV_REQ_TYPE_IFC << 0),
	USB_BMREQUESTTYPE_RECIPIENT_ENDPOINT  = (USB_DEV_REQ_TYPE_ENP << 0),
	USB_BMREQUESTTYPE_RECIPIENT_OTHER     = (USB_DEV_REQ_TYPE_OTH << 0),
	USB_BMREQUESTTYPE_RECIPIENT_MASK      = (0x1f << 0),
};

/** Hard-coded configuration descriptor, along with an associated interface
 * descriptor and endpoint descriptor, for the faked root hub.  */
static const struct {
	struct usb_desc_configuration configuration;
	struct usb_desc_interface interface;
	struct usb_desc_endpoint endpoint;
}__attribute__((packed)) root_hub_configuration = {
	.configuration = {
		.b_length = sizeof(struct usb_desc_configuration),
		.b_desc_type = USB_DESCRIPTOR_TYPE_CONFIGURATION,
		.w_total_length = sizeof(root_hub_configuration),
		.b_num_interfaces = 1,
		.b_configuration_value = 1,
		.i_configuration = 0,
		.bm_attributes = USB_CONFIGURATION_ATTRIBUTE_RESERVED_HIGH |
						USB_CONFIGURATION_ATTRIBUTE_SELF_POWERED,
		.b_max_power = 0,
	},
	.interface = {
		.b_length = sizeof(struct usb_desc_interface),
		.b_desc_type = USB_DESCRIPTOR_TYPE_INTERFACE,
		.b_interface_number = 0,
		.b_alternate_setting = 0,
		.b_num_endpoints = 1,
		.b_interface_class = USB_CLASS_CODE_HUB,
		.b_interface_subclass = 0,
		.b_interface_protocol = 0,
		.i_interface = 0,
	},
	.endpoint = {
		.b_length = sizeof(struct usb_desc_endpoint),
		.b_desc_type = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.b_endpoint_address = 1 | (USB_DIRECTION_IN << 7),
		.bm_attributes = USB_COMM_INTERRUPT,
		.w_max_packet_size = 64,
		.b_interval = 0xff,
	},
};

// /** Hard-coded list of language IDs for the faked root hub.  */
// static const struct usb_string_descriptor root_hub_string_0 = {
//     /* b_length is the base size plus the length of the b_string */
//     .b_length = sizeof(struct usb_string_descriptor) +
//                1 * sizeof(root_hub_string_0.b_string[0]),
//     .b_desc_type = USB_DESCRIPTOR_TYPE_STRING,
//     .b_string = {USB_LANGID_US_ENGLISH},
// };

// /** Hard-coded product string for the faked root hub.  */
// static const struct usb_string_descriptor root_hub_string_1 = {
//     /* b_length is the base size plus the length of the b_string */
//     .b_length = sizeof(struct usb_string_descriptor) +
//                16 * sizeof(root_hub_string_1.b_string[0]),
//     .b_desc_type = USB_DESCRIPTOR_TYPE_STRING,

//     /* This is a UTF-16LE string, hence the array of individual characters
//      * rather than a string literal.  */
//     .b_string = {'U', 'S', 'B', ' ',
//                 '2', '.', '0', ' ',
//                 'R', 'o', 'o', 't', ' ',
//                 'H', 'u', 'b'},
// };

// /** Hard-coded table of strings for the faked root hub.  */
// static const struct usb_string_descriptor * const root_hub_strings[] = {
//     &root_hub_string_0,
//     &root_hub_string_1,
// };

/** Hard-coded hub descriptor for the faked root hub.  */
static const struct usb_desc_hub root_hub_hub_descriptor = {
	/* b_desc_length is the base size plus the length of the var_data */
	.b_desc_length = sizeof(struct usb_desc_hub) +
				   2 * sizeof(root_hub_hub_descriptor.var_data[0]),
	.b_desc_type = USB_DESCRIPTOR_TYPE_HUB,
	.b_nbr_ports = 1,
	.w_hub_characteristics = 0,
	.b_pwr_on_2_pwr_good = 0,
	.b_hub_contr_current = 0,
	.var_data = { 0x00 /* DeviceRemovable */,
				 0xff, /* PortPwrCtrlMask */ },
};

#define USB_DEVICE_STATUS_SELF_POWERED  (1 << 0)
#define USB_DEVICE_STATUS_REMOTE_WAKEUP (1 << 1)

/** Format of the device status information returned by a
 * usb_device_request::USB_DEVICE_REQUEST_GET_STATUS control message.
 * Documented in Section 9.4.6 of the USB 2.0 specification.  */
struct usb_dev_status {
	uint16_t w_status;
}__attribute__((packed));

/** Hard-coded hub status for the faked root hub.  */
static const struct usb_dev_status root_hub_device_status = {
	.w_status = USB_DEVICE_STATUS_SELF_POWERED,
};

/**
 * Pending interrupt transfer (if any) to the root hub's status change endpoint.
 */
static struct usb_request *root_hub_status_change_request = NULL;

/**
 * Saved status of the host port. This is modified when the host controller
 * issues an interrupt due to a host port status change. The reason we need to
 * keep track of this status in a separate variable rather than using the
 * hardware register directly is that the changes in the hardware register need
 * to be cleared in order to clear the interrupt.
 */
static struct usb_port_status host_port_status;

/**
 * Called when host_port_status has been updated so that any status change
 * interrupt transfer that was sent to the root hub can be fulfilled.
 */
static void dwc_host_port_status_changed(void) {
	struct usb_request *req = root_hub_status_change_request;
	struct usb_dwc_request *_req = (struct usb_dwc_request *)req->hci_specific;

	if (req != NULL) {
		root_hub_status_change_request = NULL;
		log_debug("Host port status changed; "
				  "responding to status changed transfer on root hub");
		*(uint8_t*)req->buf = 0x2; /* 0x2 means Port 1 status changed (bit 0 is
									 used for the hub itself) */
		_req->actual_size = 1;
		req->req_stat = USB_REQ_NOERR;
		usb_request_complete(req);
	}
}


/**
 * Fake a standard (i.e. not hub-specific) control message request to the root
 * hub.
 *
 * @param req
 *      Standard request to the root hub to fake.
 *
 * @return
 *      ::USB_REQ_NOERR if request successfully processed; otherwise
 *      another ::enum usb_request_status error code, such as
 *      ::USB_REQ_UNDERRUN.
 */
static enum usb_request_status dwc_root_hub_standard_request(struct usb_request *req) {
	uint16_t len;
	const struct usb_control_header *setup = &req->ctrl_header;
	struct usb_dwc_request *_req = (struct usb_dwc_request *)req->hci_specific;

	switch (setup->b_request)
	{
		case USB_DEV_REQ_GET_STAT:
			len = min(setup->w_length, sizeof(root_hub_device_status));
			memcpy(req->buf, &root_hub_device_status, len);
			_req->actual_size = len;
			return USB_REQ_NOERR;

		case USB_DEV_REQ_SET_ADDR:
			return USB_REQ_NOERR;

		case USB_DEV_REQ_GET_DESC:
			switch ((setup->w_value >> 8)) /* Switch on descriptor type */
			{
				case USB_DESC_TYPE_DEV:
					len = min(setup->w_length, root_hub_device_descriptor.b_length);
					memcpy(req->buf, &root_hub_device_descriptor, len);
					_req->actual_size = len;
					return USB_REQ_NOERR;
				case USB_DESC_TYPE_CONFIG:
					len = min(setup->w_length,
							  root_hub_configuration.configuration.w_total_length);
					memcpy(req->buf, &root_hub_configuration, len);
					_req->actual_size = len;
					return USB_REQ_NOERR;
				case USB_DESC_TYPE_STRING:
					/* Index of string descriptor is in low byte of w_value */
					// if ((setup->w_value & 0xff) < ARRAY_LEN(root_hub_strings)) {
					//     const struct usb_string_descriptor *desc =
					//             root_hub_strings[setup->w_value & 0xff];
					//     len = min(setup->w_length, desc->b_length);
					//     memcpy(req->buf, desc, len);
					//     _req->actual_size = len;
					//     return USB_REQ_NOERR;
					// }
					return USB_REQ_UNDERRUN;
			}
			return USB_REQ_UNDERRUN;

		// case USB_DEV_REQ_gET_CONF:
		//     if (setup->w_length >= 1) {
		//         *(uint8_t*)req->buf = req->endp->dev->configuration;
		//         req->actual_size = 1;
		//     }
		//     return USB_REQ_NOERR;

		case USB_DEV_REQ_SET_CONF:
			if (setup->w_value <= 1) {
				return USB_REQ_NOERR;
			}
	}
	return USB_REQ_UNDERRUN;
}

/**
 * Fills in a <code>struct ::usb_hub_status</code> (which is in the USB
 * standard format) with the current status of the root hub.
 *
 * @param status
 *      The hub status structure to fill in.
 */
static void dwc_get_root_hub_status(struct usb_hub_status *status) {
	status->w_hub_status = 0;
	status->w_hub_change = 0;
	status->local_power = 1;
}

/**
 * Handle a SetPortFeature request on the port attached to the root hub.
 */
static enum usb_request_status dwc_set_host_port_feature(int feature) {
	switch (feature) {
		case USB_HUB_PORT_POWER:
			dwc_power_on_host_port();
			return USB_REQ_NOERR;
		case USB_HUB_PORT_RESET:
			dwc_reset_host_port();
			return USB_REQ_NOERR;
		default:
			return USB_REQ_UNDERRUN;
	}
	return USB_REQ_UNDERRUN;
}

/**
 * Handle a ClearPortFeature request on the port attached to the root hub.
 */
static enum usb_request_status dwc_clear_host_port_feature(int feature) {
	switch (feature) {
		case USB_HUB_PORT_CONNECT:
			host_port_status.connected_changed = 0;
			break;
		case USB_HUB_PORT_ENABLE:
			host_port_status.enabled_changed = 0;
			break;
		case USB_HUB_PORT_SUSPEND:
			host_port_status.suspended_changed = 0;
			break;
		case USB_HUB_PORT_OVERRUN:
			host_port_status.overcurrent_changed = 0;
			break;
		case USB_HUB_PORT_RESET:
			host_port_status.reset_changed = 0;
			break;
		default:
			return USB_REQ_UNDERRUN;
	}
	return USB_REQ_NOERR;
}

/**
 * Fake a hub-class-specific control message request to the root hub.
 *
 * @param req
 *      Hub-class-specific request to the root hub to fake.
 *
 * @return
 *      ::USB_REQ_NOERR if request successfully processed; otherwise
 *      another ::enum usb_request_status error code, such as
 *      ::USB_REQ_UNDERRUN.
 */
static enum usb_request_status dwc_root_hub_class_request(struct usb_request *req) {
	uint16_t len;
	const struct usb_control_header *setup = &req->ctrl_header;
	struct usb_dwc_request *_req = (struct usb_dwc_request *)req->hci_specific;

	switch (setup->b_request)
	{
		case USB_HUB_REQ_GET_DESCRIPTOR:
			/* Switch on descriptor type */
			switch ((setup->w_value >> 8)) {
				case USB_DESCRIPTOR_TYPE_HUB:
					/* GetHubDescriptor (11.24.2) */
					len = min(setup->w_length, root_hub_hub_descriptor.b_desc_length);
					memcpy(req->buf, &root_hub_hub_descriptor, len);
					_req->actual_size = len;
					return USB_REQ_NOERR;
			}
			return USB_REQ_UNDERRUN;
		case USB_HUB_REQ_GET_STATUS:
			switch (setup->bm_request_type & USB_BMREQUESTTYPE_RECIPIENT_MASK)
			{
				case USB_DEV_REQ_TYPE_DEV:
					/* GetHubStatus (11.24.2) */
					if (setup->w_length >= sizeof(struct usb_hub_status))
					{
						dwc_get_root_hub_status((struct usb_hub_status *)req->buf);
						_req->actual_size = sizeof(struct usb_hub_status);
						return USB_REQ_NOERR;
					}
					return USB_REQ_UNDERRUN;

				case USB_DEV_REQ_TYPE_OTH:
					/* GetPortStatus (11.24.2) */
					if (setup->w_length >= sizeof(struct usb_port_status))
					{
						memcpy(req->buf, &host_port_status,
							   sizeof(struct usb_port_status));
						_req->actual_size = sizeof(struct usb_port_status);
						return USB_REQ_NOERR;
					}
					return USB_REQ_UNDERRUN;
			}
			return USB_REQ_UNDERRUN;

		case USB_HUB_REQ_SET_FEATURE:
			switch (setup->bm_request_type & USB_BMREQUESTTYPE_RECIPIENT_MASK)
			{
				case USB_BMREQUESTTYPE_RECIPIENT_DEVICE:
					/* SetHubFeature (11.24.2) */
					/* TODO */
					return USB_REQ_UNDERRUN;

				case USB_BMREQUESTTYPE_RECIPIENT_OTHER:
					/* SetPortFeature (11.24.2) */
					return dwc_set_host_port_feature(setup->w_value);
			}
			return USB_REQ_UNDERRUN;

		case USB_HUB_REQ_CLEAR_FEATURE:
			switch (setup->bm_request_type & USB_BMREQUESTTYPE_RECIPIENT_MASK)
			{
				case USB_BMREQUESTTYPE_RECIPIENT_DEVICE:
					/* ClearHubFeature (11.24.2) */
					/* TODO */
					return USB_REQ_UNDERRUN;

				case USB_BMREQUESTTYPE_RECIPIENT_OTHER:
					/* ClearPortFeature (11.24.2) */
					return dwc_clear_host_port_feature(setup->w_value);
			}
			return USB_REQ_UNDERRUN;
	}
	return USB_REQ_UNDERRUN;
}

static int hcd_rh_ctrl(struct usb_hub_port *port, enum usb_hub_request req,
			unsigned short value) {
	return 0;
}

/**
 * Fake a control transfer to or from the root hub.
 */
 static enum usb_request_status dwc_root_hub_control_msg(struct usb_request *req) {
	switch (req->ctrl_header.bm_request_type & USB_BMREQUESTTYPE_TYPE_MASK) {
		case USB_BMREQUESTTYPE_TYPE_STANDARD:
			return dwc_root_hub_standard_request(req);
		case USB_BMREQUESTTYPE_TYPE_CLASS:
			return dwc_root_hub_class_request(req);
	}
	return USB_REQ_UNDERRUN;
}

/**
 * Fake a request to the root hub.
 */
static void dwc_process_root_hub_request(struct usb_request *req) {
	if (req->endp == NULL) {
		/* Control transfer request to/from default control endpoint.  */
		log_debug("Simulating request to root hub's default endpoint");
		req->req_stat = dwc_root_hub_control_msg(req);
		usb_request_complete(req);
	} else {
		/* Interrupt transfer request from status change endpoint. Assumes that
		 * only one request can be submitted at a time.  */
		log_debug("Posting status change request to root hub");
		root_hub_status_change_request = req;
		if (host_port_status.port_change != 0) {
			dwc_host_port_status_changed();
		}
	}
}

/**
 * Starts a low-level transaction on the USB.
 *
 * @param chan
 *      Index of the host channel to start the transaction on.
 * @param req
 *      USB request set up for the next transaction
 */
static void dwc_channel_start_transaction(uint chan, struct usb_request *req) {
	struct usb_dwc_request *_req = (struct usb_dwc_request *)req->hci_specific;
	volatile struct dwc_host_channel *chanptr = &regs->host_channels[chan];
	union dwc_host_channel_split_control split_control;
	union dwc_host_channel_characteristics characteristics;
	union dwc_host_channel_interrupts interrupt_mask;
	uint next_frame;
	ipl_t ipl;

	ipl = ipl_save();

	/* Clear pending interrupts.  */
	chanptr->interrupt_mask.val = 0;
	chanptr->interrupts.val = 0xffffffff;

	/* Set whether this transaction is the completion part of a split
	 * transaction or not.  */
	split_control = chanptr->split_control;
	split_control.complete_split = _req->complete_split;
	chanptr->split_control = split_control;

	/* Set odd_frame and enable the channel.  */

	next_frame = (regs->host_frame_number & 0xffff) + 1;

	if (!split_control.complete_split)
	{
		_req->csplit_retries = 0;
	}
	characteristics = chanptr->characteristics;
	characteristics.odd_frame = next_frame & 1;
	characteristics.channel_enable = 1;
	chanptr->characteristics = characteristics;

	/* Set the channel's interrupt mask to any interrupts we need to ensure
	 * that dwc_interrupt_handler() gets called when the software must take
	 * action on the transfer. Furthermore, make sure interrupts from this
	 * channel are enabled in the Host All Channels Interrupt Mask Register.
	 * Note: if you enable more channel interrupts here,
	 * dwc_interrupt_handler() needs to be changed to account for interrupts
	 * other than channel halted.  */
	interrupt_mask.val = 0;
	interrupt_mask.channel_halted = 1;
	chanptr->interrupt_mask = interrupt_mask;
	regs->host_channels_interrupt_mask |= 1 << chan;

	ipl_restore(ipl);
}

/**
 * Starts or restarts a USB transfer on a channel of the DesignWare Hi-Speed
 * USB 2.0 OTG Controller.
 *
 * To do this, software must give the parameters of a series of low-level
 * transactions on the USB to the DWC by writing to various registers. Detailed
 * documentation about the registers used here can be found in the declaration
 * of dwc_regs::dwc_host_channel.
 *
 * @param chan
 *      Index of the host channel to start the transfer on.
 * @param req
 *      USB transfer to start.
 */
static void dwc_channel_start_xfer(uint chan, struct usb_request *req) {
	struct usb_dwc_request *_req = (struct usb_dwc_request *)req->hci_specific;
	volatile struct dwc_host_channel *chanptr;
	union dwc_host_channel_characteristics characteristics;
	union dwc_host_channel_split_control split_control;
	union dwc_host_channel_transfer transfer;
	void *data;

	chanptr = &regs->host_channels[chan];
	characteristics.val = 0;
	split_control.val = 0;
	transfer.val = 0;
	_req->short_attempt = 0;

	/* Determine the endpoint number, endpoint type, maximum packet size, and
	 * packets per frame.  */
	if (req->endp != NULL) {
		/* Endpoint explicitly specified.  Get the needed information from the
		 * endpoint descriptor.  */
		// XXX Are & needed?
		characteristics.endpoint_number =
								req->endp->address & 0xf;
		characteristics.endpoint_type =
								req->endp->type & 0x3;
		characteristics.max_packet_size =
								req->endp->max_packet_size & 0x7ff;
		characteristics.packets_per_frame = 1;
		if (req->endp->dev->speed == USB_SPEED_HIGH) {
			characteristics.packets_per_frame +=
						((req->endp->max_packet_size >> 11) & 0x3);
		}
	} else {
		/* Default control endpoint.  The endpoint number, endpoint type, and
		 * packets per frame are pre-determined, while the maximum packet size
		 * can be found in the device descriptor. */
		characteristics.endpoint_number = 0;
		characteristics.endpoint_type = USB_COMM_CONTROL;
		//characteristics.max_packet_size = req->endp->max_packet_size;
		characteristics.max_packet_size = req->endp->dev->dev_desc.b_max_packet_size;
		characteristics.packets_per_frame = 1;
	}

	/* Determine the endpoint direction, data pointer, data size, and initial
	 * packet ID.  For control transfers, the overall phase of the control
	 * transfer must be taken into account. */
	if (characteristics.endpoint_type == USB_COMM_CONTROL) {
		switch (_req->control_phase) {
			case 0: /* SETUP phase of control transfer */
				log_debug("Starting SETUP transaction");
				characteristics.endpoint_direction = USB_DIRECTION_OUT;
				data = &req->ctrl_header;
				transfer.size = sizeof(struct usb_control_header);
				transfer.packet_id = DWC_USB_PID_SETUP;
				break;

			case 1: /* DATA phase of control transfer */
				log_debug("Starting DATA transactions");
				characteristics.endpoint_direction =
										req->ctrl_header.bm_request_type >> 7;
				/* We need to carefully take into account that we might be
				 * re-starting a partially complete transfer.  */
				data = req->buf + _req->actual_size;
				transfer.size = req->len - _req->actual_size;
				if (_req->actual_size == 0) {
					/* First transaction in the DATA phase: use a DATA1 packet
					 * ID.  */
					transfer.packet_id = DWC_USB_PID_DATA1;
				} else {
					/* Later transaction in the DATA phase: restore the saved
					 * packet ID (will be DATA0 or DATA1).  */
					transfer.packet_id = _req->next_data_pid;
				}
				break;

			default: /* STATUS phase of control transfer */
				log_debug("Starting STATUS transaction");
				/* The direction of the STATUS transaction is opposite the
				 * direction of the DATA transactions, or from device to host if
				 * there were no DATA transactions.  */
				if ((req->ctrl_header.bm_request_type >> 7) == USB_DIRECTION_OUT ||
					req->ctrl_header.w_length == 0) {
					characteristics.endpoint_direction = USB_DIRECTION_IN;
				} else {
					characteristics.endpoint_direction = USB_DIRECTION_OUT;
				}
				/* The STATUS transaction has no data buffer, yet must use a
				 * DATA1 packet ID.  */
				data = NULL;
				transfer.size = 0;
				transfer.packet_id = DWC_USB_PID_DATA1;
				break;
		}
	} else {
		/* Starting or re-starting a non-control transfer.  */
		characteristics.endpoint_direction =
					req->endp->address >> 7;

		/* As is the case for the DATA phase of control transfers, we need to
		 * carefully take into account that we might be restarting a partially
		 * complete transfer.  */
		data = req->buf + _req->actual_size;
		transfer.size = req->len - _req->actual_size;
		/* This hardware does not accept interrupt transfers started with more
		 * data than fits in one (micro)frame--- that is, the maximum packets
		 * per frame allowed by the endpoint times the maximum packet size
		 * allowed by the endpoint.  */
		if (characteristics.endpoint_type == USB_COMM_INTERRUPT &&
			transfer.size > characteristics.packets_per_frame *
							characteristics.max_packet_size) {
			transfer.size = characteristics.packets_per_frame *
							characteristics.max_packet_size;
			_req->short_attempt = 1;
		}
		transfer.packet_id = _req->next_data_pid;
	}

	/* Set device address.  */
	characteristics.device_address = req->endp->address;

	/* If communicating with a low or full-speed device, program the split
	 * control register.  Also cap the attempted transfer size to the maximum
	 * packet size, since the transfer will very likely have to be deferred to
	 * wait for the Complete Split portion (and then rescheduled on a possibly
	 * different channel later).  And finally, set the low_speed flag in the
	 * Channel Characteristics register if communicating with a low-speed
	 * device.  */
	if (req->endp->dev->speed != USB_SPEED_HIGH) {
		/* Determine which hub is acting as the Transaction Translator.  */
		struct usb_dev *tt_hub;
		uint tt_hub_port;

		tt_hub = req->endp->dev;
		do {
			tt_hub_port = tt_hub->port->idx;
			//XXX tt_hub = tt_hub->parent;
		} while (tt_hub->speed != USB_SPEED_HIGH);

		split_control.port_address = tt_hub_port - 1;
		split_control.hub_address = req->endp->address;
		split_control.split_enable = 1;

		if (transfer.size > characteristics.max_packet_size) {
			transfer.size = characteristics.max_packet_size;
			_req->short_attempt = 1;
		}

		if (req->endp->dev->speed == USB_SPEED_LOW) {
			characteristics.low_speed = 1;
		}
	}

	/* Set up DMA buffer. */
	if (IS_WORD_ALIGNED(data)) {
		/* Can DMA directly from source or to destination if word-aligned. */
		chanptr->dma_address = (uint32_t)data;
	} else {
		/* Need to use alternate buffer for DMA, since the actual source or
		 * destination is not word-aligned.  If the attempted transfer size
		 * overflows this alternate buffer, cap it to the greatest number of
		 * whole packets that fit. */
		chanptr->dma_address = (uint32_t)aligned_bufs[chan];
		if (transfer.size > sizeof(aligned_bufs[chan])) {
			transfer.size = sizeof(aligned_bufs[chan]) -
							(sizeof(aligned_bufs[chan]) %
							  characteristics.max_packet_size);
			_req->short_attempt = 1;
		}
		/* For OUT endpoints, copy the data to send into the DMA buffer. */
		if (characteristics.endpoint_direction == USB_DIRECTION_OUT) {
			memcpy(aligned_bufs[chan], data, transfer.size);
		}
	}

	/* Set pointer to start of next chunk of data to send/receive (may be
	 * different from the actual DMA address to be used by the hardware if an
	 * alternate buffer was selected above). */
	_req->cur_data_ptr = data;

	/* Calculate the number of packets being set up for this transfer. */
	transfer.packet_count = DIV_ROUND_UP(transfer.size,
										 characteristics.max_packet_size);
	if (transfer.packet_count == 0) {
		/* The hardware requires that at least one packet is specified, even
		 * for zero-length transfers. */
		transfer.packet_count = 1;
	}

	/* Remember the actual size and number of packets we are attempting to
	 * transfer.  */
	_req->attempted_size = transfer.size;
	_req->attempted_bytes_remaining = transfer.size;
	_req->attempted_packets_remaining = transfer.packet_count;

	/* Save this pending request in a location in which the interrupt handler
	 * can find it.  */
	channel_pending_xfers[chan] = req;

	log_debug("Setting up transactions on channel %u:\n"
			  "\t\tmax_packet_size=%u, "
			  "endpoint_number=%u, endpoint_direction=%s,\n"
			  "\t\tlow_speed=%u, endpoint_type=%s, device_address=%u,\n\t\t"
			  "size=%u, packet_count=%u, packet_id=%u, split_enable=%u, "
			  "complete_split=%u",
			  chan,
			  characteristics.max_packet_size,
			  characteristics.endpoint_number,
			  "x",//usb_direction_to_string(characteristics.endpoint_direction),
			  characteristics.low_speed,
			  "x", //usb_transfer_type_to_string(characteristics.endpoint_type),
			  characteristics.device_address,
			  transfer.size,
			  transfer.packet_count,
			  transfer.packet_id,
			  split_control.split_enable,
			  _req->complete_split);

	/* Actually program the registers of the appropriate channel.  */
	chanptr->characteristics = characteristics;
	chanptr->split_control   = split_control;
	chanptr->transfer        = transfer;

	/* Enable the channel, thereby starting the USB transfer.  After doing this,
	 * the next code executed to process this transfer will be in
	 * dwc_handle_channel_halted_interrupt() after the Host Controller has
	 * issued an interrupt regarding this channel.  */
	dwc_channel_start_transaction(chan, req);
}

/**
 * Thread procedure for the threads created in defer_xfer().
 *
 * Instances of this thread are killed in usb_free_xfer_request().
 *
 * @param req
 *      USB transfer request to defer.
 *
 * @return
 *      This thread never returns.
 */
static void *defer_xfer_thread(void *data) {
	struct usb_request *req = (struct usb_request *)data;
	struct usb_dwc_request *_req = (struct usb_dwc_request *)req->hci_specific;
	uint interval_ms;
	uint chan;

	if (req->endp->dev->speed == USB_SPEED_HIGH) {
		interval_ms = (1 << (req->endp->interval - 1)) /
							  USB_UFRAMES_PER_MS;
	} else {
		interval_ms = req->endp->interval / USB_FRAMES_PER_MS;
	} if (interval_ms <= 0) {
		interval_ms = 1;
	}

	while (1) {
		semaphore_enter(&_req->deferer_thread_sema);

#if START_SPLIT_INTR_TRANSFERS_ON_SOF
		if (_req->need_sof) {
			union dwc_core_interrupts intr_mask;
			ipl_t ipl;

			log_debug("Waiting for start-of-frame");

			ipl = ipl_save();
			chan = dwc_get_free_channel();
			channel_pending_xfers[chan] = req;
			sofwait |= 1 << chan;
			intr_mask = regs->core_interrupt_mask;
			intr_mask.sof_intr = 1;
			regs->core_interrupt_mask = intr_mask;

			// XXX receive();

			dwc_channel_start_xfer(chan, req);
			_req->need_sof = 0;
			ipl_restore(ipl);
		}
#endif /* START_SPLIT_INTR_TRANSFERS_ON_SOF */
		else {
			log_debug("Waiting %u ms to start xfer again", interval_ms);
			sleep(interval_ms);
			chan = dwc_get_free_channel();
			dwc_channel_start_xfer(chan, req);
		}
	}
	return NULL;
}

/**
 * Called when a USB transfer needs to be retried at a later time due to no data
 * being available from the endpoint.
 *
 * For periodic transfers (e.g. polling an interrupt endpoint), the exact time
 * at which the transfer must be retried is specified by the b_interval member of
 * the endpoint descriptor. For low and full-speed devices, b_interval specifies
 * the number of millisconds to wait before the next poll, while for high-speed
 * devices it specifies the exponent (plus one) of a power-of-two number of
 * milliseconds to wait before the next poll.
 *
 * To actually implement delaying a transfer, we associate each transfer with a
 * thread created on-demand. Each such thread simply enters a loop where it
 * calls sleep() for the appropriate number of milliseconds, then retries the
 * transfer. A semaphore is needed to make the thread do nothing until the
 * request has actually been submitted and deferred.
 *
 * Note: this code gets used to scheduling polling of IN interrupt endpoints,
 * including those on hubs and HID devices. Thus, polling of these devices for
 * status changes (in the case of hubs) or new input (in the case of HID
 * devices) is done in software. This wakes up the CPU a lot and wastes time
 * and energy. But with USB 2.0, there is no way around this, other than by
 * suspending the USB device which we don't support.
 *
 * @param req
 *      USB transfer to defer.
 *
 * @return
 *      ::USB_REQ_NOERR if deferral process successfully started; otherwise
 *      another ::enum usb_request_status error code.
 */
static enum usb_request_status defer_xfer(struct usb_request *req) {
	struct usb_dwc_request *_req = (struct usb_dwc_request *)req->hci_specific;

	// XXX: mov it to hci initialization
	// semaphore_init(&_req->deferer_thread_sema, 0);
	log_debug("Deferring transfer");
	if (!_req->deferer_thread) {
		_req->deferer_thread = thread_create(0, defer_xfer_thread, req);
	}
	semaphore_leave(&_req->deferer_thread_sema);

	return USB_REQ_NOERR;
}

/** Internal transfer status codes used to simplify interrupt handling.  */
enum dwc_intr_status {
	XFER_COMPLETE            = 0,
	XFER_FAILED              = 1,
	XFER_NEEDS_DEFERRAL      = 2,
	XFER_NEEDS_RESTART       = 3,
	XFER_NEEDS_TRANS_RESTART = 4,
};

/**
 * Handle a channel halting with no apparent error.
 */
static enum dwc_intr_status dwc_handle_normal_channel_halted(
		struct usb_request *req, uint chan,
		union dwc_host_channel_interrupts interrupts) {
	volatile struct dwc_host_channel *chanptr = &regs->host_channels[chan];
	struct usb_dwc_request *_req = (struct usb_dwc_request *)req->hci_specific;

	/* The hardware seems to update transfer.packet_count as expected, so we can
	 * look at it before deciding whether to use transfer.size (which is not
	 * always updated as expected).  */
	uint packets_remaining   = chanptr->transfer.packet_count;
	uint packets_transferred = _req->attempted_packets_remaining -
							   packets_remaining;

	log_debug("%u packets transferred on channel %u", packets_transferred,
		chan);

	if (packets_transferred != 0) {
		uint bytes_transferred = 0;
		union dwc_host_channel_characteristics characteristics =
											chanptr->characteristics;
		uint max_packet_size = characteristics.max_packet_size;
		enum usb_direction dir = characteristics.endpoint_direction;
		enum usb_comm_type type = characteristics.endpoint_type;

		/* Calculate number of bytes transferred and copy data from DMA
		 * buffer if needed.  */

		if (dir == USB_DESC_ENDP_ADDR_IN) {
			/* The transfer.size field seems to be updated sanely for IN
			 * transfers.  (Good thing too, since otherwise it would be
			 * impossible to determine the length of short packets...)  */
			bytes_transferred = _req->attempted_bytes_remaining -
								chanptr->transfer.size;
			/* Copy data from DMA buffer if needed */
			if (!IS_WORD_ALIGNED(_req->cur_data_ptr)) {
				memcpy(_req->cur_data_ptr,
					   &aligned_bufs[chan][_req->attempted_size -
										   _req->attempted_bytes_remaining],
					   bytes_transferred);
			}
		} else {
			/* Ignore transfer.size field for OUT transfers because it's not
			 * updated sanely.  */
			if (packets_transferred > 1) {
				/* More than one packet transferred: all except the last
				 * must have been max_packet_size.  */
				bytes_transferred += max_packet_size*(packets_transferred - 1);
			}
			/* If the last packet in this transfer attempt was transmitted, its
			 * size is the remainder of the attempted transfer size.  Otherwise,
			 * it's another max_packet_size.  */
			if (packets_remaining == 0 &&
				(_req->attempted_size % max_packet_size != 0 ||
				 _req->attempted_size == 0)) {
				bytes_transferred += _req->attempted_size % max_packet_size;
			} else {
				bytes_transferred += max_packet_size;
			}
		}

		log_debug("Calculated %u bytes transferred", bytes_transferred);

		/* Account for packets and bytes transferred  */
		_req->attempted_packets_remaining -= packets_transferred;
		_req->attempted_bytes_remaining -= bytes_transferred;
		_req->cur_data_ptr += bytes_transferred;

		/* Check if transfer complete (at least to the extent that data was
		 * programmed into the channel).  */
		if (_req->attempted_packets_remaining == 0 ||
			(dir == USB_DIRECTION_IN &&
			 bytes_transferred < packets_transferred * max_packet_size)) {
			/* The transfer_completed flag should have been set by the hardware,
			 * although it's essentially meaningless because it gets set at
			 * other times as well.  (For example, it appears to be set when a
			 * split transaction has completed, even if there are still packets
			 * remaining to be transferred).  */
			if (!interrupts.transfer_completed) {
				log_error("transfer_completed flag not "
							  "set on channel %u as expected "
							  "(interrupts=0x%08x, transfer=0x%08x).", chan,
							  interrupts.val, chanptr->transfer.val);
				return XFER_FAILED;
			}

			/* If we programmed less than the desired transfer size into the
			 * channels (for one of several reasons--- see
			 * dwc_channel_start_xfer()), continue to attempt the transfer,
			 * unless it was an interrupt transfer, in which case at most one
			 * attempt should be made, or if fewer bytes were transferred than
			 * attempted (for an IN transfer), indicating the transfer is
			 * already done.  */
			if (_req->short_attempt && _req->attempted_bytes_remaining == 0 &&
				type != USB_COMM_INTERRUPT) {
				log_debug("Starting next part of %u-byte transfer "
						  "after short attempt of %u bytes",
						  req->len, _req->attempted_size);
				_req->complete_split = 0;
				_req->next_data_pid = chanptr->transfer.packet_id;
				if (!usb_is_control_request(req) || _req->control_phase == 1) {
					_req->actual_size = (char *)_req->cur_data_ptr - req->buf;
				}
				return XFER_NEEDS_RESTART;
			}

			/* Unlike other transfers, control transfers consist of multiple
			 * phases.  If we only just completed the SETUP or DATA phase of a
			 * control transfer, advance to the next phase and do not signal
			 * transfer completion.  */
			if (usb_is_control_request(req) && _req->control_phase < 2) {
				/* Reset the CSPLIT flag.  */
				_req->complete_split = 0;

				/* Record bytes transferred if we just completed the
				 * data phase.  */
				if (_req->control_phase == 1) {
					_req->actual_size = (char *)_req->cur_data_ptr - req->buf;
				}

				/* Advance to the next phase. */
				_req->control_phase++;

				/* Skip DATA phase if there is no data to send/receive.
				 * */
				if (_req->control_phase == 1 && req->len == 0) {
					_req->control_phase++;
				}
				return XFER_NEEDS_RESTART;
			}

			/* Transfer is actually complete (or at least, it was an IN transfer
			 * that completed with fewer bytes transferred than requested).  */
			log_debug("Transfer completed on channel %u", chan);
			return XFER_COMPLETE;
		} else {
			/* Transfer not complete, so start the next transaction.  */

			/* Flip the CSPLIT flag if doing split transactions.  */
			if (chanptr->split_control.split_enable) {
				_req->complete_split ^= 1;
			}

			log_debug("Continuing transfer (complete_split=%u)",
						  _req->complete_split);
			return XFER_NEEDS_TRANS_RESTART;
		}
	} else {
		/* No packets transferred, but no error flag was set.  This is expected
		 * only if we just did a Start Split transaction, in which case we
		 * should continue on to the Complete Split transaction.  We also check
		 * for the ack_response_received flag, which should be set to indicate
		 * that the device acknowledged the Start Split transaction.  */
		if (interrupts.ack_response_received &&
			chanptr->split_control.split_enable && !_req->complete_split) {
			/* Start CSPLIT */
			_req->complete_split = 1;
			log_debug("Continuing transfer (complete_split=%u)",
						  _req->complete_split);
			return XFER_NEEDS_TRANS_RESTART;
		} else {
			log_error("No packets transferred.");
			return XFER_FAILED;
		}
	}
}

/**
 * Handle a channel halted interrupt on the specified channel. This can occur
 * anytime after dwc_channel_start_transaction() enabled the channel and the
 * corresponding channel halted interrupt.
 *
 * @param chan
 *      Index of the DWC host channel on which the channel halted interrupt
 *      occurred.
 */
static void dwc_handle_channel_halted_interrupt(uint chan) {
	struct usb_request *req = channel_pending_xfers[chan];
	struct usb_dwc_request *_req = (struct usb_dwc_request *)req->hci_specific;
	volatile struct dwc_host_channel *chanptr = &regs->host_channels[chan];
	union dwc_host_channel_interrupts interrupts = chanptr->interrupts;
	enum dwc_intr_status intr_status;

	log_debug("Handling channel %u halted interrupt"
			  "\t\t(interrupts pending: 0x%08x, characteristics=0x%08x, "
			  "transfer=0x%08x)",
			  chan, interrupts.val, chanptr->characteristics.val,
			  chanptr->transfer.val);

	/* Determine the cause of the interrupt. */

	if (interrupts.stall_response_received || interrupts.ahb_error ||
		interrupts.transaction_error || interrupts.babble_error ||
		interrupts.excess_transaction_error ||
		interrupts.frame_list_rollover ||
		(interrupts.nyet_response_received && !_req->complete_split) ||
		(interrupts.data_toggle_error &&
		chanptr->characteristics.endpoint_direction == USB_DESC_ENDP_ADDR_OUT)) {
		/* An error occurred. Complete the transfer immediately with an error
		 * status. */
		log_error("Transfer error on channel %u (interrupts pending: 0x%08x, "
				  " packet_count=%u)", chan, interrupts.val,
				  chanptr->transfer.packet_count);
		intr_status = XFER_FAILED;
	} else if (interrupts.frame_overrun) {
		/* Restart transactions that fail sporatically due to frame overruns.
		 * TODO: why does this happen? */
		log_debug("Frame overrun on channel %u; restarting transaction", chan);
		intr_status = XFER_NEEDS_TRANS_RESTART;
	} else if (interrupts.nyet_response_received) {
		/* Device sent NYET packet when completing a split transaction. Try the
		 * CSPLIT again later. As a special case, if too many NYETs are
		 * received, restart the entire split transaction. (Apparently, because
		 * of frame overruns or some other reason it's possible for NYETs to be
		 * issued indefinitely until the transaction is retried.) */
		log_debug("NYET response received on channel %u", chan);
		if (++_req->csplit_retries >= 10) {
			log_debug("Restarting split transaction (CSPLIT tried %u times)",
						_req->csplit_retries);
			_req->complete_split = false;
		}
		intr_status = XFER_NEEDS_TRANS_RESTART;
	} else if (interrupts.nak_response_received) {
		/* Device sent NAK packet. This happens when the device had no data to
		 * send at this time. Try again later. Special case: if the NAK was
		 * sent during a Complete Split transaction, restart with the Start
		 * Split, not the Complete Split. */
		log_debug("NAK response received on channel %u", chan);
		intr_status = XFER_NEEDS_DEFERRAL;
		_req->complete_split = false;
	} else {
		/* No apparent error occurred. */
		intr_status = dwc_handle_normal_channel_halted(req, chan, interrupts);
	}

#if START_SPLIT_INTR_TRANSFERS_ON_SOF
	if ((intr_status == XFER_NEEDS_RESTART ||
		 intr_status == XFER_NEEDS_TRANS_RESTART) &&
		 req->endp->type == USB_COMM_INTERRUPT &&
		 req->endp->dev->speed != USB_SPEED_HIGH &&
		!_req->complete_split) {
		intr_status = XFER_NEEDS_DEFERRAL;
		_req->need_sof = 1;
	}
#endif

	switch (intr_status) {
		case XFER_COMPLETE:
			req->req_stat = USB_REQ_NOERR;
			break;
		case XFER_FAILED:
			req->req_stat = USB_REQ_UNDERRUN;
			break;
		case XFER_NEEDS_DEFERRAL:
			break;
		case XFER_NEEDS_RESTART:
			dwc_channel_start_xfer(chan, req);
			return;
		case XFER_NEEDS_TRANS_RESTART:
			dwc_channel_start_transaction(chan, req);
			return;
	}

	/* Transfer complete, transfer encountered an error, or transfer needs to
	 * be retried later. */

	/* Save the data packet ID. */
	_req->next_data_pid = chanptr->transfer.packet_id;

	/* Clear and disable interrupts on this channel. */
	chanptr->interrupt_mask.val = 0;
	chanptr->interrupts.val = 0xffffffff;

	/* Release the channel. */
	channel_pending_xfers[chan] = NULL;
	dwc_release_channel(chan);

	/* Set the actual transferred size, unless we are doing a control transfer
	 * and aren't on the DATA phase. */
	if (!(req->endp->type == USB_COMM_CONTROL) || _req->control_phase == 1) {
		_req->actual_size = (char *)_req->cur_data_ptr - req->buf;
	}

	/* If we got here because we received a NAK or NYET, defer the request for
	 * a later time. */
	if (intr_status == XFER_NEEDS_DEFERRAL) {
		enum usb_request_status status = defer_xfer(req);
		if (status == USB_REQ_NOERR) {
			return;
		} else {
			req->req_stat = status;
		}
	}

	/* If we got here because the transfer successfully completed or an error
	 * occurred, call the device-driver-provided completion callback. */
	usb_request_complete(req);
}

/**
 * Interrupt handler function for the Synopsys DesignWare Hi-Speed USB 2.0
 * On-The-Go Controller (DWC). This should only be called when an interrupt
 * this driver explicitly enabled is pending. See the comment above
 * dwc_setup_interrupts() for an overview of interrupts on this hardware.
 */
static irq_return_t dwc_interrupt_handler(unsigned int irq_nr, void *data) {
	union dwc_core_interrupts interrupts = regs->core_interrupts;

#if START_SPLIT_INTR_TRANSFERS_ON_SOF
	if (interrupts.sof_intr) {
		/* Start of frame (SOF) interrupt occurred. */

		log_debug("Received SOF intr (host_frame_number=0x%08x)",
			regs->host_frame_number);
		if ((regs->host_frame_number & 0x7) != 6) {
			union dwc_core_interrupts tmp;

			if (sofwait != 0) {
				/* Wake up one channel waiting for SOF */
				uint chan = first_set_bit(sofwait);
				struct usb_dwc_request *_req = (struct usb_dwc_request *)
					channel_pending_xfers[chan]->hci_specific;
				sched_wakeup(&_req->deferer_thread->schedee);
				sofwait &= ~(1 << chan);
			}

			/* Disable SOF interrupt if no longer needed */
			if (sofwait == 0) {
				tmp = regs->core_interrupt_mask;
				tmp.sof_intr = 0;
				regs->core_interrupt_mask = tmp;
			}

			/* Clear SOF interrupt */
			tmp.val = 0;
			tmp.sof_intr = 1;
			regs->core_interrupts = tmp;
		}
	}
#endif /* START_SPLIT_INTR_TRANSFERS_ON_SOF */

	if (interrupts.host_channel_intr) {
		/* One or more channels has an interrupt pending. */

		uint32_t chintr;
		uint chan;

		/* A bit in the "Host All Channels Interrupt Register" is set if an
		 * interrupt has occurred on the corresponding host channel. Process
		 * all set bits. */
		chintr = regs->host_channels_interrupt;
		do {
			chan = first_set_bit(chintr);
			dwc_handle_channel_halted_interrupt(chan);
			chintr ^= (1 << chan);
		} while (chintr != 0);
	} if (interrupts.port_intr) {
		/* Status of the host port changed. Update host_port_status. */
		union dwc_host_port_ctrlstatus hw_status = regs->host_port_ctrlstatus;

		log_debug("Port interrupt detected: host_port_ctrlstatus=0x%08x",
				  hw_status.val);

		host_port_status.connected   = hw_status.connected;
		host_port_status.enabled     = hw_status.enabled;
		host_port_status.suspended   = hw_status.suspended;
		host_port_status.overcurrent = hw_status.overcurrent;
		host_port_status.reset       = hw_status.reset;
		host_port_status.powered     = hw_status.powered;
		host_port_status.low_speed_attached = (hw_status.speed == USB_SPEED_LOW);
		host_port_status.high_speed_attached = (hw_status.speed == USB_SPEED_HIGH);

		host_port_status.connected_changed   = hw_status.connected_changed;
		host_port_status.enabled_changed     = hw_status.enabled_changed;
		host_port_status.overcurrent_changed = hw_status.overcurrent_changed;

		/* Clear the interrupt(s), which are "write-clear", by writing the Host
		 * Port Control and Status register back to itself. But as a special
		 * case, 'enabled' must be written as 0; otherwise the port will
		 * apparently disable itself. */
		hw_status.enabled = 0;
		regs->host_port_ctrlstatus = hw_status;

		/* Complete status change request to the root hub if one has been
		 * submitted. */
		dwc_host_port_status_changed();
	}

	return IRQ_HANDLED;
}

/**
 * Performs initial setup of the Synopsys Designware USB 2.0 On-The-Go
 * Controller (DWC) interrupts.
 *
 * The DWC contains several levels of interrupt registers, detailed in the
 * following list. Note that for each level, each bit of the "interrupt"
 * register contains the state of a pending interrupt (1 means interrupt
 * pending; write 1 to clear), while the "interrupt mask" register has the same
 * format but is used to turn the corresponding interrupt on or off (1 means on;
 * write 1 to turn on; write 0 to turn off).
 *
 * - The AHB configuration register contains a mask bit used to enable/disable
 *   all interrupts whatsoever from the DWC hardware.
 * - The "Core" interrupt and interrupt mask registers control top-level
 *   interrupts. For example, a single bit in these registers corresponds to
 *   all channel interrupts.
 * - The "Host All Channels" interrupt and interrupt mask registers control all
 *   interrupts on each channel.
 * - The "Channel" interrupt and interrupt mask registers, of which one copy
 *   exists for each channel, control individual interrupt types on that
 *   channel.
 *
 * We can assume that an interrupt only occurs if it is enabled in all the
 * places listed above. Furthermore, it only seems to work to clear interrupts
 * at the lowest level; for example, a channel interrupt must be cleared in its
 * individual channel interrupt register rather than in one of the higher level
 * interrupt registers.
 *
 * The above just covers the DWC-specific interrupt registers. In addition to
 * those, the system will have other ways to control interrupts. For example,
 * on the BCM2835 (Raspberry Pi), the interrupt line going to the DWC is just
 * one of many dozen and can be enabled/disabled using the interrupt
 * controller.
 * In the code below we enable this interrupt line and register a handler
 * function so that we can actually get interrupts from the DWC.
 *
 * And all that's in addition to the CPSR of the ARM processor itself, or the
 * equivalent on other CPUs. So all in all, you literally have to enable
 * interrupts in 6 different places to get an interrupt when a USB transfer has
 * completed.
 */
static void dwc_setup_interrupts(struct usb_hcd *hcd) {
	union dwc_core_interrupts core_interrupt_mask;

	/* Clear all pending core interrupts. */
	regs->core_interrupt_mask.val = 0;
	regs->core_interrupts.val = 0xffffffff;

	/* Enable core host channel and port interrupts. */
	core_interrupt_mask.val = 0;
	core_interrupt_mask.host_channel_intr = 1;
	core_interrupt_mask.port_intr = 1;
	regs->core_interrupt_mask = core_interrupt_mask;

	/* Enable the interrupt line that goes to the USB controller and register
	 * the interrupt handler. */
	irq_attach(IRQ_USB, dwc_interrupt_handler, 0, hcd, "usb_dwc irq");

	/* Enable interrupts for entire USB host controller. (Yes that's what we
	 * just did, but this one is controlled by the host controller itself.) */
	regs->ahb_configuration |= DWC_AHB_INTERRUPT_ENABLE;
}

/**
 * Initialize a bitmask and semaphore that keep track of the free/inuse status
 * of the host channels and a queue in which to place submitted USB transfer
 * requests, then start the USB transfer request scheduler thread.
 */
static enum usb_request_status dwc_start_xfer_scheduler(void) {
	semaphore_init(&chfree_sema, DWC_NUM_CHANNELS);

	static_assert(DWC_NUM_CHANNELS <= 8 * sizeof(chfree));
	chfree = (1 << DWC_NUM_CHANNELS) - 1;

	return USB_REQ_NOERR;
}

/* Implementation of hcd_start() for the DesignWare Hi-Speed USB 2.0 On-The-Go
 * Controller. See usb_hcdi.h for the documentation of this interface of the
 * Host Controller Driver. */
int hcd_start(struct usb_hcd *hcd) {
	enum usb_request_status status;

	status = dwc_power_on();
	if (status != USB_REQ_NOERR) {
		return status;
	}

	dwc_soft_reset();
	dwc_setup_dma_mode();
	dwc_setup_interrupts(hcd);

	status = dwc_start_xfer_scheduler();
	if (status != USB_REQ_NOERR) {
		dwc_power_off();
	}

	return status;
}

/* Implementation of hcd_stop() for the DesignWare Hi-Speed USB 2.0 On-The-Go
 * Controller. See usb_hcdi.h for the documentation of this interface of the
 * Host Controller Driver. */
int hcd_stop(struct usb_hcd *hcd) {
	/* Disable IRQ line and handler. */
	irq_detach(IRQ_USB, hcd);

	/* Power off USB hardware. */
	dwc_power_off();

	return 0;
}

/* Implementation of hcd_submit_xfer_request() for the DesignWare Hi-Speed USB
 * 2.0 On-The-Go Controller. See usb_hcdi.h for the documentation of this
 * interface of the Host Controller Driver. */
/**
 * @details
 *
 * This Host Controller Driver implements this interface asynchronously, as
 * intended. Furthermore, it uses a simplistic scheduling algorithm where it
 * places transfer requests into a single queue and executes them in the order
 * they were submitted. Transfers that need to be retried, including periodic
 * transfers that receive a NAK reply and split transactions that receive a
 * NYET reply when doing the Complete Split transaction, are scheduled to be
 * retried at an appropriate time by separate code that shortcuts the main
 * queue when the timer expires.
 *
 * Jump to dwc_schedule_xfer_requests() to see what happens next.
 */
static int hcd_request(struct usb_request *req) {
	if (0) {//XXX is_root_hub(req->endp->dev)) {
		/* Special case: request is to the root hub. Fake it. */
		dwc_process_root_hub_request(req);
	} else {
		/* Normal case: schedule the transfer on some channel. */
		uint chan = dwc_get_free_channel();
		dwc_channel_start_xfer(chan, req);
	}

	return USB_REQ_NOERR;
}

static struct usb_hcd_ops usb_dwc_hcd_ops = {
	.hcd_start = hcd_start,
	.hcd_stop = hcd_stop,
	// .hcd_hci_alloc = usb_dvc_hcd_hcd_alloc,
	// .hcd_hci_free = usb_dvc_hcd_hcd_free,
	.rhub_ctrl = hcd_rh_ctrl,
	.request = hcd_request,
};

static int usb_dwc_init(void) {
	struct usb_hcd *hcd = usb_hcd_alloc(&usb_dwc_hcd_ops, NULL /* XXX */);
	if (!hcd) {
		return -ENOMEM;
	}

	return usb_hcd_register(hcd);
}
