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

#include <interrupt.h>
#include <mailbox.h>
#include <string.h>
#include <thread.h>
#include <usb_core_driver.h>
#include <usb_dwc_regs.h>
#include <usb_hcdi.h>
#include <usb_hub_defs.h>
#include <usb_std_defs.h>
#include "bcm2835.h"

#include <util/log.h>
#include <linux/types.h>
#include <hal/ipl.h>

/** Round a number up to the next multiple of the word size.  */
#define WORD_ALIGN(n) (((n) + sizeof(ulong) - 1) & ~(sizeof(ulong) - 1))

/** Determines whether a pointer is word-aligned or not.  */
#define IS_WORD_ALIGNED(ptr) ((ulong)(ptr) % sizeof(ulong) == 0)

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

/** Thread ID of USB transfer request scheduler thread.  */
static tid_typ dwc_xfer_scheduler_tid;

/** Bitmap of channel free (1) or in-use (0) statuses.  */
static uint chfree;

#if START_SPLIT_INTR_TRANSFERS_ON_SOF
/** Bitmap of channels waiting for start-of-frame  */
static uint sofwait;
#endif

/** Semaphore that tracks the number of free channels in chfree bitmask.  */
static semaphore chfree_sema;

/**
 * Array that holds pointers to the USB transfer request (if any) currently
 * being completed on each hardware channel.
 */
static struct usb_xfer_request *channel_pending_xfers[DWC_NUM_CHANNELS];

/** Aligned buffers for DMA.  */
static uint8_t aligned_bufs[DWC_NUM_CHANNELS][WORD_ALIGN(USB_MAX_PACKET_SIZE)]
								__aligned(4);

/* Find index of first set bit in a nonzero word.  */
static inline ulong first_set_bit(ulong word)
{
	return 31 - __builtin_clz(word);
}

/**
 * Finds and reserves an unused DWC USB host channel.  This is blocking and
 * waits until a channel is available.
 *
 * @return
 *      Index of the free channel.
 */
static uint
dwc_get_free_channel(void)
{
	uint chan;
	ipl_t ipl;

	ipl = ipl_save();
	wait(chfree_sema);
	chan = first_set_bit(chfree);
	chfree ^= (1 << chan);
	ipl_restore(ipl);
	return chan;
}

/**
 * Marks a channel as free.  This signals any thread that may be waiting for a
 * free channel.
 *
 * @param chan
 *      Index of DWC USB host channel to release.
 */
static void
dwc_release_channel(uint chan)
{
	ipl_t ipl;

	ipl = ipl_save();
	chfree |= (1 << chan);
	signal(chfree_sema);
	ipl_restore(ipl);
}

/**
 * Powers on the DWC hardware.
 */
static usb_status_t
dwc_power_on(void)
{
	int retval;

	log_info("Powering on Synopsys DesignWare Hi-Speed "
			 "USB 2.0 On-The-Go Controller\n");
	retval = board_setpower(POWER_USB, TRUE);
	return (retval == OK) ? USB_STATUS_SUCCESS : USB_STATUS_HARDWARE_ERROR;
}

static void
dwc_power_off(void)
{
	log_info("Powering off Synopsys DesignWare Hi-Speed "
			 "USB 2.0 On-The-Go Controller\n");
	board_setpower(POWER_USB, FALSE);
}

/**
 * Performs a software reset of the DWC hardware.  Note: the DWC seems to be in
 * a reset state after the initial power on, so this is only strictly necessary
 * when hcd_start() is entered with the DWC already powered on (e.g. when
 * starting a new kernel directly in software with kexec()).
 */
static void
dwc_soft_reset(void)
{
	log_debug("Resetting USB controller\n");

	/* Set soft reset flag, then wait until it's cleared.  */
	regs->core_reset = DWC_SOFT_RESET;
	while (regs->core_reset & DWC_SOFT_RESET)
	{
	}
}

/**
 * Set up the DWC OTG USB Host Controller for DMA (direct memory access).  This
 * makes it possible for the Host Controller to directly access in-memory
 * buffers when performing USB transfers.  Beware: all buffers accessed with DMA
 * must be 4-byte-aligned.  Furthermore, if the L1 data cache is enabled, then
 * it must be explicitly flushed to maintain cache coherency since it is
 * internal to the ARM processor.  (This is not currently handled by this driver
 * because Xinu does not enable the L1 data cache.)
 */
static void
dwc_setup_dma_mode(void)
{
	const uint32_t rx_words = 1024;  /* Size of Rx FIFO in 4-byte words */
	const uint32_t tx_words = 1024;  /* Size of Non-periodic Tx FIFO in 4-byte words */
	const uint32_t ptx_words = 1024; /* Size of Periodic Tx FIFO in 4-byte words */

	/* First configure the Host Controller's FIFO sizes.  This is _required_
	 * because the default values (at least in Broadcom's instantiation of the
	 * Synopsys USB block) do not work correctly.  If software fails to do this,
	 * receiving data will fail in virtually impossible to debug ways that cause
	 * memory corruption.  This is true even though we are using DMA and not
	 * otherwise interacting with the Host Controller's FIFOs in this driver. */
	log_debug("%u words of RAM available for dynamic FIFOs\n", regs->hwcfg3 >> 16);
	log_debug("original FIFO sizes: rx 0x%08x,  tx 0x%08x, ptx 0x%08x\n",
			  regs->rx_fifo_size, regs->nonperiodic_tx_fifo_size,
			  regs->host_periodic_tx_fifo_size);
	regs->rx_fifo_size = rx_words;
	regs->nonperiodic_tx_fifo_size = (tx_words << 16) | rx_words;
	regs->host_periodic_tx_fifo_size = (ptx_words << 16) | (rx_words + tx_words);

	/* Actually enable DMA by setting the appropriate flag; also set an extra
	 * flag available only in Broadcom's instantiation of the Synopsys USB block
	 * that may or may not actually be needed.  */
	regs->ahb_configuration |= DWC_AHB_DMA_ENABLE | BCM_DWC_AHB_AXI_WAIT;
}

/**
 * Read the Host Port Control and Status register with the intention of
 * modifying it.  Due to the inconsistent design of the bits in this register,
 * this requires zeroing the write-clear bits so they aren't unintentionally
 * cleared by writing back 1's to them.
 */
static union dwc_host_port_ctrlstatus
dwc_get_host_port_ctrlstatus(void)
{
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
static void
dwc_power_on_host_port(void)
{
	union dwc_host_port_ctrlstatus hw_status;

	log_debug("Powering on host port.\n");
	hw_status = dwc_get_host_port_ctrlstatus();
	hw_status.powered = 1;
	regs->host_port_ctrlstatus = hw_status;
}

/**
 * Resets the DWC host port; i.e. the USB port that is logically attached to the
 * root hub.
 */
static void
dwc_reset_host_port(void)
{
	union dwc_host_port_ctrlstatus hw_status;

	log_debug("Resetting host port\n");

	/* Set the reset flag on the port, then clear it after a certain amount of
	 * time.  */
	hw_status = dwc_get_host_port_ctrlstatus();
	hw_status.reset = 1;
	regs->host_port_ctrlstatus = hw_status;
	sleep(60);  /* (sleep for 60 milliseconds).  */
	hw_status.reset = 0;
	regs->host_port_ctrlstatus = hw_status;
}

/** Hard-coded device descriptor for the faked root hub.  */
static const struct usb_device_descriptor root_hub_device_descriptor = {
	.bLength = sizeof(struct usb_device_descriptor),
	.bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
	.bcdUSB = 0x200, /* USB version 2.0 (binary-coded decimal) */
	.bDeviceClass = USB_CLASS_CODE_HUB,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0,
	.idProduct = 0,
	.bcdDevice = 0,
	.iManufacturer = 0,
	.iProduct = 1,
	.iSerialNumber = 0,
	.bNumConfigurations = 1,
};

/** Hard-coded configuration descriptor, along with an associated interface
 * descriptor and endpoint descriptor, for the faked root hub.  */
static const struct {
	struct usb_configuration_descriptor configuration;
	struct usb_interface_descriptor interface;
	struct usb_endpoint_descriptor endpoint;
} __packed root_hub_configuration = {
	.configuration = {
		.bLength = sizeof(struct usb_configuration_descriptor),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION,
		.wTotalLength = sizeof(root_hub_configuration),
		.bNumInterfaces = 1,
		.bConfigurationValue = 1,
		.iConfiguration = 0,
		.bmAttributes = USB_CONFIGURATION_ATTRIBUTE_RESERVED_HIGH |
						USB_CONFIGURATION_ATTRIBUTE_SELF_POWERED,
		.bMaxPower = 0,
	},
	.interface = {
		.bLength = sizeof(struct usb_interface_descriptor),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
		.bInterfaceNumber = 0,
		.bAlternateSetting = 0,
		.bNumEndpoints = 1,
		.bInterfaceClass = USB_CLASS_CODE_HUB,
		.bInterfaceSubClass = 0,
		.bInterfaceProtocol = 0,
		.iInterface = 0,
	},
	.endpoint = {
		.bLength = sizeof(struct usb_endpoint_descriptor),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 1 | (USB_DIRECTION_IN << 7),
		.bmAttributes = USB_TRANSFER_TYPE_INTERRUPT,
		.wMaxPacketSize = 64,
		.bInterval = 0xff,
	},
};

/** Hard-coded list of language IDs for the faked root hub.  */
static const struct usb_string_descriptor root_hub_string_0 = {
	/* bLength is the base size plus the length of the bString */
	.bLength = sizeof(struct usb_string_descriptor) +
			   1 * sizeof(root_hub_string_0.bString[0]),
	.bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
	.bString = {USB_LANGID_US_ENGLISH},
};

/** Hard-coded product string for the faked root hub.  */
static const struct usb_string_descriptor root_hub_string_1 = {
	/* bLength is the base size plus the length of the bString */
	.bLength = sizeof(struct usb_string_descriptor) +
			   16 * sizeof(root_hub_string_1.bString[0]),
	.bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,

	/* This is a UTF-16LE string, hence the array of individual characters
	 * rather than a string literal.  */
	.bString = {'U', 'S', 'B', ' ',
				'2', '.', '0', ' ',
				'R', 'o', 'o', 't', ' ',
				'H', 'u', 'b'},
};

/** Hard-coded table of strings for the faked root hub.  */
static const struct usb_string_descriptor * const root_hub_strings[] = {
	&root_hub_string_0,
	&root_hub_string_1,
};

/** Hard-coded hub descriptor for the faked root hub.  */
static const struct usb_hub_descriptor root_hub_hub_descriptor = {
	/* bDescLength is the base size plus the length of the varData */
	.bDescLength = sizeof(struct usb_hub_descriptor) +
				   2 * sizeof(root_hub_hub_descriptor.varData[0]),
	.bDescriptorType = USB_DESCRIPTOR_TYPE_HUB,
	.bNbrPorts = 1,
	.wHubCharacteristics = 0,
	.bPwrOn2PwrGood = 0,
	.bHubContrCurrent = 0,
	.varData = { 0x00 /* DeviceRemovable */,
				 0xff, /* PortPwrCtrlMask */ },
};

/** Hard-coded hub status for the faked root hub.  */
static const struct usb_device_status root_hub_device_status = {
	.wStatus = USB_DEVICE_STATUS_SELF_POWERED,
};

/**
 * Pending interrupt transfer (if any) to the root hub's status change endpoint.
 */
static struct usb_xfer_request *root_hub_status_change_request = NULL;

/**
 * Saved status of the host port.  This is modified when the host controller
 * issues an interrupt due to a host port status change.  The reason we need to
 * keep track of this status in a separate variable rather than using the
 * hardware register directly is that the changes in the hardware register need
 * to be cleared in order to clear the interrupt.
 */
static struct usb_port_status host_port_status;

/**
 * Called when host_port_status has been updated so that any status change
 * interrupt transfer that was sent to the root hub can be fulfilled.
 */
static void
dwc_host_port_status_changed(void)
{
	struct usb_xfer_request *req = root_hub_status_change_request;
	if (req != NULL)
	{
		root_hub_status_change_request = NULL;
		log_debug("Host port status changed; "
				  "responding to status changed transfer on root hub\n");
		*(uint8_t*)req->recvbuf = 0x2; /* 0x2 means Port 1 status changed (bit 0 is
									 used for the hub itself) */
		req->actual_size = 1;
		req->status = USB_STATUS_SUCCESS;
		usb_complete_xfer(req);
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
 *      ::USB_STATUS_SUCCESS if request successfully processed; otherwise
 *      another ::usb_status_t error code, such as
 *      ::USB_STATUS_UNSUPPORTED_REQUEST.
 */
static usb_status_t
dwc_root_hub_standard_request(struct usb_xfer_request *req)
{
	uint16_t len;
	const struct usb_control_setup_data *setup = &req->setup_data;

	switch (setup->bRequest)
	{
		case USB_DEVICE_REQUEST_GET_STATUS:
			len = min(setup->wLength, sizeof(root_hub_device_status));
			memcpy(req->recvbuf, &root_hub_device_status, len);
			req->actual_size = len;
			return USB_STATUS_SUCCESS;

		case USB_DEVICE_REQUEST_SET_ADDRESS:
			return USB_STATUS_SUCCESS;

		case USB_DEVICE_REQUEST_GET_DESCRIPTOR:
			switch ((setup->wValue >> 8)) /* Switch on descriptor type */
			{
				case USB_DESCRIPTOR_TYPE_DEVICE:
					len = min(setup->wLength, root_hub_device_descriptor.bLength);
					memcpy(req->recvbuf, &root_hub_device_descriptor, len);
					req->actual_size = len;
					return USB_STATUS_SUCCESS;
				case USB_DESCRIPTOR_TYPE_CONFIGURATION:
					len = min(setup->wLength,
							  root_hub_configuration.configuration.wTotalLength);
					memcpy(req->recvbuf, &root_hub_configuration, len);
					req->actual_size = len;
					return USB_STATUS_SUCCESS;
				case USB_DESCRIPTOR_TYPE_STRING:
					/* Index of string descriptor is in low byte of wValue */
					if ((setup->wValue & 0xff) < ARRAY_LEN(root_hub_strings))
					{
						const struct usb_string_descriptor *desc =
								root_hub_strings[setup->wValue & 0xff];
						len = min(setup->wLength, desc->bLength);
						memcpy(req->recvbuf, desc, len);
						req->actual_size = len;
						return USB_STATUS_SUCCESS;
					}
					return USB_STATUS_UNSUPPORTED_REQUEST;
			}
			return USB_STATUS_UNSUPPORTED_REQUEST;

		case USB_DEVICE_REQUEST_GET_CONFIGURATION:
			if (setup->wLength >= 1)
			{
				*(uint8_t*)req->recvbuf = req->dev->configuration;
				req->actual_size = 1;
			}
			return USB_STATUS_SUCCESS;

		case USB_DEVICE_REQUEST_SET_CONFIGURATION:
			if (setup->wValue <= 1)
			{
				return USB_STATUS_SUCCESS;
			}
	}
	return USB_STATUS_UNSUPPORTED_REQUEST;
}

/**
 * Fills in a <code>struct ::usb_hub_status</code> (which is in the USB standard
 * format) with the current status of the root hub.
 *
 * @param status
 *      The hub status structure to fill in.
 */
static void
dwc_get_root_hub_status(struct usb_hub_status *status)
{
	status->wHubStatus = 0;
	status->wHubChange = 0;
	status->local_power = 1;
}

/**
 * Handle a SetPortFeature request on the port attached to the root hub.
 */
static usb_status_t
dwc_set_host_port_feature(enum usb_port_feature feature)
{
	switch (feature)
	{
		case USB_PORT_POWER:
			dwc_power_on_host_port();
			return USB_STATUS_SUCCESS;
		case USB_PORT_RESET:
			dwc_reset_host_port();
			return USB_STATUS_SUCCESS;
		default:
			return USB_STATUS_UNSUPPORTED_REQUEST;
	}
	return USB_STATUS_UNSUPPORTED_REQUEST;
}

/**
 * Handle a ClearPortFeature request on the port attached to the root hub.
 */
static usb_status_t
dwc_clear_host_port_feature(enum usb_port_feature feature)
{
	switch (feature)
	{
		case USB_C_PORT_CONNECTION:
			host_port_status.connected_changed = 0;
			break;
		case USB_C_PORT_ENABLE:
			host_port_status.enabled_changed = 0;
			break;
		case USB_C_PORT_SUSPEND:
			host_port_status.suspended_changed = 0;
			break;
		case USB_C_PORT_OVER_CURRENT:
			host_port_status.overcurrent_changed = 0;
			break;
		case USB_C_PORT_RESET:
			host_port_status.reset_changed = 0;
			break;
		default:
			return USB_STATUS_UNSUPPORTED_REQUEST;
	}
	return USB_STATUS_SUCCESS;
}

/**
 * Fake a hub-class-specific control message request to the root hub.
 *
 * @param req
 *      Hub-class-specific request to the root hub to fake.
 *
 * @return
 *      ::USB_STATUS_SUCCESS if request successfully processed; otherwise
 *      another ::usb_status_t error code, such as
 *      ::USB_STATUS_UNSUPPORTED_REQUEST.
 */
static usb_status_t
dwc_root_hub_class_request(struct usb_xfer_request *req)
{
	uint16_t len;
	const struct usb_control_setup_data *setup = &req->setup_data;
	switch (setup->bRequest)
	{
		case USB_HUB_REQUEST_GET_DESCRIPTOR:
			switch ((setup->wValue >> 8)) /* Switch on descriptor type */
			{
				case USB_DESCRIPTOR_TYPE_HUB:
					/* GetHubDescriptor (11.24.2) */
					len = min(setup->wLength, root_hub_hub_descriptor.bDescLength);
					memcpy(req->recvbuf, &root_hub_hub_descriptor, len);
					req->actual_size = len;
					return USB_STATUS_SUCCESS;
			}
			return USB_STATUS_UNSUPPORTED_REQUEST;
		case USB_HUB_REQUEST_GET_STATUS:
			switch (setup->bmRequestType & USB_BMREQUESTTYPE_RECIPIENT_MASK)
			{
				case USB_BMREQUESTTYPE_RECIPIENT_DEVICE:
					/* GetHubStatus (11.24.2) */
					if (setup->wLength >= sizeof(struct usb_hub_status))
					{
						dwc_get_root_hub_status(req->recvbuf);
						req->actual_size = sizeof(struct usb_hub_status);
						return USB_STATUS_SUCCESS;
					}
					return USB_STATUS_UNSUPPORTED_REQUEST;

				case USB_BMREQUESTTYPE_RECIPIENT_OTHER:
					/* GetPortStatus (11.24.2) */
					if (setup->wLength >= sizeof(struct usb_port_status))
					{
						memcpy(req->recvbuf, &host_port_status,
							   sizeof(struct usb_port_status));
						req->actual_size = sizeof(struct usb_port_status);
						return USB_STATUS_SUCCESS;
					}
					return USB_STATUS_UNSUPPORTED_REQUEST;
			}
			return USB_STATUS_UNSUPPORTED_REQUEST;

		case USB_HUB_REQUEST_SET_FEATURE:
			switch (setup->bmRequestType & USB_BMREQUESTTYPE_RECIPIENT_MASK)
			{
				case USB_BMREQUESTTYPE_RECIPIENT_DEVICE:
					/* SetHubFeature (11.24.2) */
					/* TODO */
					return USB_STATUS_UNSUPPORTED_REQUEST;

				case USB_BMREQUESTTYPE_RECIPIENT_OTHER:
					/* SetPortFeature (11.24.2) */
					return dwc_set_host_port_feature(setup->wValue);
			}
			return USB_STATUS_UNSUPPORTED_REQUEST;

		case USB_HUB_REQUEST_CLEAR_FEATURE:
			switch (setup->bmRequestType & USB_BMREQUESTTYPE_RECIPIENT_MASK)
			{
				case USB_BMREQUESTTYPE_RECIPIENT_DEVICE:
					/* ClearHubFeature (11.24.2) */
					/* TODO */
					return USB_STATUS_UNSUPPORTED_REQUEST;

				case USB_BMREQUESTTYPE_RECIPIENT_OTHER:
					/* ClearPortFeature (11.24.2) */
					return dwc_clear_host_port_feature(setup->wValue);
			}
			return USB_STATUS_UNSUPPORTED_REQUEST;
	}
	return USB_STATUS_UNSUPPORTED_REQUEST;
}

/**
 * Fake a control transfer to or from the root hub.
 */
static usb_status_t
dwc_root_hub_control_msg(struct usb_xfer_request *req)
{
	switch (req->setup_data.bmRequestType & USB_BMREQUESTTYPE_TYPE_MASK)
	{
		case USB_BMREQUESTTYPE_TYPE_STANDARD:
			return dwc_root_hub_standard_request(req);
		case USB_BMREQUESTTYPE_TYPE_CLASS:
			return dwc_root_hub_class_request(req);
	}
	return USB_STATUS_UNSUPPORTED_REQUEST;
}

/**
 * Fake a request to the root hub.
 */
static void
dwc_process_root_hub_request(struct usb_xfer_request *req)
{
	if (req->endpoint_desc == NULL)
	{
		/* Control transfer request to/from default control endpoint.  */
		log_debug("Simulating request to root hub's default endpoint");
		req->status = dwc_root_hub_control_msg(req);
		usb_complete_xfer(req);
	}
	else
	{
		/* Interrupt transfer request from status change endpoint.  Assumes that
		 * only one request can be submitted at a time.  */
		log_debug("Posting status change request to root hub");
		root_hub_status_change_request = req;
		if (host_port_status.wPortChange != 0)
		{
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
static void
dwc_channel_start_transaction(uint chan, struct usb_xfer_request *req)
{
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
	split_control.complete_split = req->complete_split;
	chanptr->split_control = split_control;

	/* Set odd_frame and enable the channel.  */

	next_frame = (regs->host_frame_number & 0xffff) + 1;

	if (!split_control.complete_split)
	{
		req->csplit_retries = 0;
	}
	characteristics = chanptr->characteristics;
	characteristics.odd_frame = next_frame & 1;
	characteristics.channel_enable = 1;
	chanptr->characteristics = characteristics;

	/* Set the channel's interrupt mask to any interrupts we need to ensure that
	 * dwc_interrupt_handler() gets called when the software must take action on
	 * the transfer.  Furthermore, make sure interrupts from this channel are
	 * enabled in the Host All Channels Interrupt Mask Register.  Note: if you
	 * enable more channel interrupts here, dwc_interrupt_handler() needs to be
	 * changed to account for interrupts other than channel halted.  */
	interrupt_mask.val = 0;
	interrupt_mask.channel_halted = 1;
	chanptr->interrupt_mask = interrupt_mask;
	regs->host_channels_interrupt_mask |= 1 << chan;

	ipl_restore(ipl);
}

/**
 * Starts or restarts a USB transfer on a channel of the DesignWare Hi-Speed USB
 * 2.0 OTG Controller.
 *
 * To do this, software must give the parameters of a series of low-level
 * transactions on the USB to the DWC by writing to various registers.  Detailed
 * documentation about the registers used here can be found in the declaration
 * of dwc_regs::dwc_host_channel.
 *
 * @param chan
 *      Index of the host channel to start the transfer on.
 * @param req
 *      USB transfer to start.
 */
static void
dwc_channel_start_xfer(uint chan, struct usb_xfer_request *req)
{
	volatile struct dwc_host_channel *chanptr;
	union dwc_host_channel_characteristics characteristics;
	union dwc_host_channel_split_control split_control;
	union dwc_host_channel_transfer transfer;
	void *data;

	chanptr = &regs->host_channels[chan];
	characteristics.val = 0;
	split_control.val = 0;
	transfer.val = 0;
	req->short_attempt = 0;

	/* Determine the endpoint number, endpoint type, maximum packet size, and
	 * packets per frame.  */
	if (req->endpoint_desc != NULL)
	{
		/* Endpoint explicitly specified.  Get the needed information from the
		 * endpoint descriptor.  */
		characteristics.endpoint_number =
									req->endpoint_desc->bEndpointAddress & 0xf;
		characteristics.endpoint_type =
									req->endpoint_desc->bmAttributes & 0x3;
		characteristics.max_packet_size =
									req->endpoint_desc->wMaxPacketSize & 0x7ff;
		characteristics.packets_per_frame = 1;
		if (req->dev->speed == USB_SPEED_HIGH)
		{
			characteristics.packets_per_frame +=
						((req->endpoint_desc->wMaxPacketSize >> 11) & 0x3);
		}
	}
	else
	{
		/* Default control endpoint.  The endpoint number, endpoint type, and
		 * packets per frame are pre-determined, while the maximum packet size
		 * can be found in the device descriptor.  */
		characteristics.endpoint_number = 0;
		characteristics.endpoint_type = USB_TRANSFER_TYPE_CONTROL;
		characteristics.max_packet_size = req->dev->descriptor.bMaxPacketSize0;
		characteristics.packets_per_frame = 1;
	}

	/* Determine the endpoint direction, data pointer, data size, and initial
	 * packet ID.  For control transfers, the overall phase of the control
	 * transfer must be taken into account.  */
	if (characteristics.endpoint_type == USB_TRANSFER_TYPE_CONTROL)
	{
		switch (req->control_phase)
		{
			case 0: /* SETUP phase of control transfer */
				usb_dev_debug(req->dev, "Starting SETUP transaction\n");
				characteristics.endpoint_direction = USB_DIRECTION_OUT;
				data = &req->setup_data;
				transfer.size = sizeof(struct usb_control_setup_data);
				transfer.packet_id = DWC_USB_PID_SETUP;
				break;

			case 1: /* DATA phase of control transfer */
				usb_dev_debug(req->dev, "Starting DATA transactions\n");
				characteristics.endpoint_direction =
										req->setup_data.bmRequestType >> 7;
				/* We need to carefully take into account that we might be
				 * re-starting a partially complete transfer.  */
				data = req->recvbuf + req->actual_size;
				transfer.size = req->size - req->actual_size;
				if (req->actual_size == 0)
				{
					/* First transaction in the DATA phase: use a DATA1 packet
					 * ID.  */
					transfer.packet_id = DWC_USB_PID_DATA1;
				}
				else
				{
					/* Later transaction in the DATA phase: restore the saved
					 * packet ID (will be DATA0 or DATA1).  */
					transfer.packet_id = req->next_data_pid;
				}
				break;

			default: /* STATUS phase of control transfer */
				usb_dev_debug(req->dev, "Starting STATUS transaction\n");
				/* The direction of the STATUS transaction is opposite the
				 * direction of the DATA transactions, or from device to host if
				 * there were no DATA transactions.  */
				if ((req->setup_data.bmRequestType >> 7) == USB_DIRECTION_OUT ||
					req->setup_data.wLength == 0)
				{
					characteristics.endpoint_direction = USB_DIRECTION_IN;
				}
				else
				{
					characteristics.endpoint_direction = USB_DIRECTION_OUT;
				}
				/* The STATUS transaction has no data buffer, yet must use a
				 * DATA1 packet ID.  */
				data = NULL;
				transfer.size = 0;
				transfer.packet_id = DWC_USB_PID_DATA1;
				break;
		}
	}
	else /* Starting or re-starting a non-control transfer.  */
	{
		characteristics.endpoint_direction =
					req->endpoint_desc->bEndpointAddress >> 7;

		/* As is the case for the DATA phase of control transfers, we need to
		 * carefully take into account that we might be restarting a partially
		 * complete transfer.  */
		data = req->recvbuf + req->actual_size;
		transfer.size = req->size - req->actual_size;
		/* This hardware does not accept interrupt transfers started with more
		 * data than fits in one (micro)frame--- that is, the maximum packets
		 * per frame allowed by the endpoint times the maximum packet size
		 * allowed by the endpoint.  */
		if (characteristics.endpoint_type == USB_TRANSFER_TYPE_INTERRUPT &&
			transfer.size > characteristics.packets_per_frame *
							characteristics.max_packet_size)
		{
			transfer.size = characteristics.packets_per_frame *
							characteristics.max_packet_size;
			req->short_attempt = 1;
		}
		transfer.packet_id = req->next_data_pid;
	}

	/* Set device address.  */
	characteristics.device_address = req->dev->address;

	/* If communicating with a low or full-speed device, program the split
	 * control register.  Also cap the attempted transfer size to the maximum
	 * packet size, since the transfer will very likely have to be deferred to
	 * wait for the Complete Split portion (and then rescheduled on a possibly
	 * different channel later).  And finally, set the low_speed flag in the
	 * Channel Characteristics register if communicating with a low-speed
	 * device.  */
	if (req->dev->speed != USB_SPEED_HIGH)
	{
		/* Determine which hub is acting as the Transaction Translator.  */
		struct usb_device *tt_hub;
		uint tt_hub_port;

		tt_hub = req->dev;
		do {
			tt_hub_port = tt_hub->port_number;
			tt_hub = tt_hub->parent;
		} while (tt_hub->speed != USB_SPEED_HIGH);

		split_control.port_address = tt_hub_port - 1;
		split_control.hub_address = tt_hub->address;
		split_control.split_enable = 1;

		if (transfer.size > characteristics.max_packet_size)
		{
			transfer.size = characteristics.max_packet_size;
			req->short_attempt = 1;
		}

		if (req->dev->speed == USB_SPEED_LOW)
		{
			characteristics.low_speed = 1;
		}
	}

	/* Set up DMA buffer.  */
	if (IS_WORD_ALIGNED(data))
	{
		/* Can DMA directly from source or to destination if word-aligned.  */
		chanptr->dma_address = (uint32_t)data;
	}
	else
	{
		/* Need to use alternate buffer for DMA, since the actual source or
		 * destination is not word-aligned.  If the attempted transfer size
		 * overflows this alternate buffer, cap it to the greatest number of
		 * whole packets that fit.  */
		chanptr->dma_address = (uint32_t)aligned_bufs[chan];
		if (transfer.size > sizeof(aligned_bufs[chan]))
		{
			transfer.size = sizeof(aligned_bufs[chan]) -
							(sizeof(aligned_bufs[chan]) %
							  characteristics.max_packet_size);
			req->short_attempt = 1;
		}
		/* For OUT endpoints, copy the data to send into the DMA buffer.  */
		if (characteristics.endpoint_direction == USB_DIRECTION_OUT)
		{
			memcpy(aligned_bufs[chan], data, transfer.size);
		}
	}

	/* Set pointer to start of next chunk of data to send/receive (may be
	 * different from the actual DMA address to be used by the hardware if an
	 * alternate buffer was selected above).  */
	req->cur_data_ptr = data;

	/* Calculate the number of packets being set up for this transfer.  */
	transfer.packet_count = DIV_ROUND_UP(transfer.size,
										 characteristics.max_packet_size);
	if (transfer.packet_count == 0)
	{
		/* The hardware requires that at least one packet is specified, even for
		 * zero-length transfers.  */
		transfer.packet_count = 1;
	}

	/* Remember the actual size and number of packets we are attempting to
	 * transfer.  */
	req->attempted_size = transfer.size;
	req->attempted_bytes_remaining = transfer.size;
	req->attempted_packets_remaining = transfer.packet_count;

	/* Save this pending request in a location in which the interrupt handler
	 * can find it.  */
	channel_pending_xfers[chan] = req;

	usb_dev_debug(req->dev, "Setting up transactions on channel %u:\n"
				  "\t\tmax_packet_size=%u, "
				  "endpoint_number=%u, endpoint_direction=%s,\n"
				  "\t\tlow_speed=%u, endpoint_type=%s, device_address=%u,\n\t\t"
				  "size=%u, packet_count=%u, packet_id=%u, split_enable=%u, "
				  "complete_split=%u\n",
				  chan,
				  characteristics.max_packet_size,
				  characteristics.endpoint_number,
				  usb_direction_to_string(characteristics.endpoint_direction),
				  characteristics.low_speed,
				  usb_transfer_type_to_string(characteristics.endpoint_type),
				  characteristics.device_address,
				  transfer.size,
				  transfer.packet_count,
				  transfer.packet_id,
				  split_control.split_enable,
				  req->complete_split);

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
static thread
defer_xfer_thread(struct usb_xfer_request *req)
{
	uint interval_ms;
	uint chan;

	if (req->dev->speed == USB_SPEED_HIGH)
	{
		interval_ms = (1 << (req->endpoint_desc->bInterval - 1)) /
							  USB_UFRAMES_PER_MS;
	}
	else
	{
		interval_ms = req->endpoint_desc->bInterval / USB_FRAMES_PER_MS;
	}
	if (interval_ms <= 0)
	{
		interval_ms = 1;
	}
	for (;;)
	{
		wait(req->deferer_thread_sema);

#if START_SPLIT_INTR_TRANSFERS_ON_SOF
		if (req->need_sof)
		{
			union dwc_core_interrupts intr_mask;
			ipl_t ipl;

			usb_dev_debug(req->dev,
						  "Waiting for start-of-frame\n");

			ipl = ipl_save();
			chan = dwc_get_free_channel();
			channel_pending_xfers[chan] = req;
			sofwait |= 1 << chan;
			intr_mask = regs->core_interrupt_mask;
			intr_mask.sof_intr = 1;
			regs->core_interrupt_mask = intr_mask;

			receive();

			dwc_channel_start_xfer(chan, req);
			req->need_sof = 0;
			ipl_restore(ipl);
		}
		else
#endif /* START_SPLIT_INTR_TRANSFERS_ON_SOF */
		{
			usb_dev_debug(req->dev,
						  "Waiting %u ms to start xfer again\n", interval_ms);
			sleep(interval_ms);
			chan = dwc_get_free_channel();
			dwc_channel_start_xfer(chan, req);
		}
	}
	return SYSERR;
}

/**
 * Called when a USB transfer needs to be retried at a later time due to no data
 * being available from the endpoint.
 *
 * For periodic transfers (e.g. polling an interrupt endpoint), the exact time
 * at which the transfer must be retried is specified by the bInterval member of
 * the endpoint descriptor.  For low and full-speed devices, bInterval specifies
 * the number of millisconds to wait before the next poll, while for high-speed
 * devices it specifies the exponent (plus one) of a power-of-two number of
 * milliseconds to wait before the next poll.
 *
 * To actually implement delaying a transfer, we associate each transfer with a
 * thread created on-demand.  Each such thread simply enters a loop where it
 * calls sleep() for the appropriate number of milliseconds, then retries the
 * transfer.  A semaphore is needed to make the thread do nothing until the
 * request has actually been submitted and deferred.
 *
 * Note: this code gets used to scheduling polling of IN interrupt endpoints,
 * including those on hubs and HID devices.  Thus, polling of these devices for
 * status changes (in the case of hubs) or new input (in the case of HID
 * devices) is done in software.  This wakes up the CPU a lot and wastes time
 * and energy.  But with USB 2.0, there is no way around this, other than by
 * suspending the USB device which we don't support.
 *
 * @param req
 *      USB transfer to defer.
 *
 * @return
 *      ::USB_STATUS_SUCCESS if deferral process successfully started; otherwise
 *      another ::usb_status_t error code.
 */
static usb_status_t
defer_xfer(struct usb_xfer_request *req)
{
	usb_dev_debug(req->dev, "Deferring transfer\n");
	if (SYSERR == req->deferer_thread_sema)
	{
		req->deferer_thread_sema = semcreate(0);
		if (SYSERR == req->deferer_thread_sema)
		{
			usb_dev_error(req->dev, "Can't create semaphore\n");
			return USB_STATUS_OUT_OF_MEMORY;
		}
	}
	if (BADTID == req->deferer_thread_tid)
	{
		req->deferer_thread_tid = create(defer_xfer_thread,
										 DEFER_XFER_THREAD_STACK_SIZE,
										 DEFER_XFER_THREAD_PRIORITY,
										 DEFER_XFER_THREAD_NAME,
										 1, req);
		if (SYSERR == ready(req->deferer_thread_tid, RESCHED_NO))
		{
			req->deferer_thread_tid = BADTID;
			usb_dev_error(req->dev,
						  "Can't create thread to service periodic transfer\n");
			return USB_STATUS_OUT_OF_MEMORY;
		}
	}
	signal(req->deferer_thread_sema);
	return USB_STATUS_SUCCESS;
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
static enum dwc_intr_status
dwc_handle_normal_channel_halted(struct usb_xfer_request *req, uint chan,
								 union dwc_host_channel_interrupts interrupts)
{
	volatile struct dwc_host_channel *chanptr = &regs->host_channels[chan];

	/* The hardware seems to update transfer.packet_count as expected, so we can
	 * look at it before deciding whether to use transfer.size (which is not
	 * always updated as expected).  */
	uint packets_remaining   = chanptr->transfer.packet_count;
	uint packets_transferred = req->attempted_packets_remaining -
							   packets_remaining;

	usb_dev_debug(req->dev, "%u packets transferred on channel %u\n",
				  packets_transferred, chan);

	if (packets_transferred != 0)
	{
		uint bytes_transferred = 0;
		union dwc_host_channel_characteristics characteristics =
											chanptr->characteristics;
		uint max_packet_size = characteristics.max_packet_size;
		enum usb_direction dir = characteristics.endpoint_direction;
		enum usb_transfer_type type = characteristics.endpoint_type;

		/* Calculate number of bytes transferred and copy data from DMA
		 * buffer if needed.  */

		if (dir == USB_DIRECTION_IN)
		{
			/* The transfer.size field seems to be updated sanely for IN
			 * transfers.  (Good thing too, since otherwise it would be
			 * impossible to determine the length of short packets...)  */
			bytes_transferred = req->attempted_bytes_remaining -
								chanptr->transfer.size;
			/* Copy data from DMA buffer if needed */
			if (!IS_WORD_ALIGNED(req->cur_data_ptr))
			{
				memcpy(req->cur_data_ptr,
					   &aligned_bufs[chan][req->attempted_size -
										   req->attempted_bytes_remaining],
					   bytes_transferred);
			}
		}
		else
		{
			/* Ignore transfer.size field for OUT transfers because it's not
			 * updated sanely.  */
			if (packets_transferred > 1)
			{
				/* More than one packet transferred: all except the last
				 * must have been max_packet_size.  */
				bytes_transferred += max_packet_size * (packets_transferred - 1);
			}
			/* If the last packet in this transfer attempt was transmitted, its
			 * size is the remainder of the attempted transfer size.  Otherwise,
			 * it's another max_packet_size.  */
			if (packets_remaining == 0 &&
				(req->attempted_size % max_packet_size != 0 ||
				 req->attempted_size == 0))
			{
				bytes_transferred += req->attempted_size % max_packet_size;
			}
			else
			{
				bytes_transferred += max_packet_size;
			}
		}

		usb_dev_debug(req->dev, "Calculated %u bytes transferred\n",
					  bytes_transferred);

		/* Account for packets and bytes transferred  */
		req->attempted_packets_remaining -= packets_transferred;
		req->attempted_bytes_remaining -= bytes_transferred;
		req->cur_data_ptr += bytes_transferred;

		/* Check if transfer complete (at least to the extent that data was
		 * programmed into the channel).  */
		if (req->attempted_packets_remaining == 0 ||
			(dir == USB_DIRECTION_IN &&
			 bytes_transferred < packets_transferred * max_packet_size))
		{
			/* The transfer_completed flag should have been set by the hardware,
			 * although it's essentially meaningless because it gets set at
			 * other times as well.  (For example, it appears to be set when a
			 * split transaction has completed, even if there are still packets
			 * remaining to be transferred).  */
			if (!interrupts.transfer_completed)
			{
				usb_dev_error(req->dev, "transfer_completed flag not "
							  "set on channel %u as expected "
							  "(interrupts=0x%08x, transfer=0x%08x).\n", chan,
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
			if (req->short_attempt && req->attempted_bytes_remaining == 0 &&
				type != USB_TRANSFER_TYPE_INTERRUPT)
			{
				usb_dev_debug(req->dev,
							  "Starting next part of %u-byte transfer "
							  "after short attempt of %u bytes\n",
							  req->size, req->attempted_size);
				req->complete_split = 0;
				req->next_data_pid = chanptr->transfer.packet_id;
				if (!usb_is_control_request(req) || req->control_phase == 1)
				{
					req->actual_size = req->cur_data_ptr - req->recvbuf;
				}
				return XFER_NEEDS_RESTART;
			}

			/* Unlike other transfers, control transfers consist of multiple
			 * phases.  If we only just completed the SETUP or DATA phase of a
			 * control transfer, advance to the next phase and do not signal
			 * transfer completion.  */
			if (usb_is_control_request(req) && req->control_phase < 2)
			{
				/* Reset the CSPLIT flag.  */
				req->complete_split = 0;

				/* Record bytes transferred if we just completed the
				 * data phase.  */
				if (req->control_phase == 1)
				{
					req->actual_size = req->cur_data_ptr - req->recvbuf;
				}

				/* Advance to the next phase. */
				req->control_phase++;

				/* Skip DATA phase if there is no data to send/receive.
				 * */
				if (req->control_phase == 1 && req->size == 0)
				{
					req->control_phase++;
				}
				return XFER_NEEDS_RESTART;
			}

			/* Transfer is actually complete (or at least, it was an IN transfer
			 * that completed with fewer bytes transferred than requested).  */
			usb_dev_debug(req->dev, "Transfer completed on channel %u\n", chan);
			return XFER_COMPLETE;
		}
		else
		{
			/* Transfer not complete, so start the next transaction.  */

			/* Flip the CSPLIT flag if doing split transactions.  */
			if (chanptr->split_control.split_enable)
			{
				req->complete_split ^= 1;
			}

			usb_dev_debug(req->dev, "Continuing transfer (complete_split=%u)\n",
						  req->complete_split);
			return XFER_NEEDS_TRANS_RESTART;
		}
	}
	else
	{
		/* No packets transferred, but no error flag was set.  This is expected
		 * only if we just did a Start Split transaction, in which case we
		 * should continue on to the Complete Split transaction.  We also check
		 * for the ack_response_received flag, which should be set to indicate
		 * that the device acknowledged the Start Split transaction.  */
		if (interrupts.ack_response_received &&
			chanptr->split_control.split_enable && !req->complete_split)
		{
			/* Start CSPLIT */
			req->complete_split = 1;
			usb_dev_debug(req->dev, "Continuing transfer (complete_split=%u)\n",
						  req->complete_split);
			return XFER_NEEDS_TRANS_RESTART;
		}
		else
		{
			usb_dev_error(req->dev, "No packets transferred.\n");
			return XFER_FAILED;
		}
	}
}

/**
 * Handle a channel halted interrupt on the specified channel.  This can occur
 * anytime after dwc_channel_start_transaction() enabled the channel and the
 * corresponding channel halted interrupt.
 *
 * @param chan
 *      Index of the DWC host channel on which the channel halted interrupt
 *      occurred.
 */
static void
dwc_handle_channel_halted_interrupt(uint chan)
{
	struct usb_xfer_request *req = channel_pending_xfers[chan];
	volatile struct dwc_host_channel *chanptr = &regs->host_channels[chan];
	union dwc_host_channel_interrupts interrupts = chanptr->interrupts;
	enum dwc_intr_status intr_status;

	log_debug("Handling channel %u halted interrupt\n"
			  "\t\t(interrupts pending: 0x%08x, characteristics=0x%08x, "
			  "transfer=0x%08x)",
			  chan, interrupts.val, chanptr->characteristics.val,
			  chanptr->transfer.val);

	/* Determine the cause of the interrupt.  */

	if (interrupts.stall_response_received || interrupts.ahb_error ||
		interrupts.transaction_error || interrupts.babble_error ||
		interrupts.excess_transaction_error || interrupts.frame_list_rollover ||
		(interrupts.nyet_response_received && !req->complete_split) ||
		(interrupts.data_toggle_error &&
		 chanptr->characteristics.endpoint_direction == USB_DIRECTION_OUT))
	{
		/* An error occurred.  Complete the transfer immediately with an error
		 * status.  */
		usb_dev_error(req->dev, "Transfer error on channel %u "
					  "(interrupts pending: 0x%08x, packet_count=%u)\n",
					  chan, interrupts.val, chanptr->transfer.packet_count);
		intr_status = XFER_FAILED;
	}
	else if (interrupts.frame_overrun)
	{
		/* Restart transactions that fail sporatically due to frame overruns.
		 * TODO: why does this happen?  */
		usb_dev_debug(req->dev, "Frame overrun on channel %u; "
					  "restarting transaction\n", chan);
		intr_status = XFER_NEEDS_TRANS_RESTART;
	}
	else if (interrupts.nyet_response_received)
	{
		/* Device sent NYET packet when completing a split transaction.  Try the
		 * CSPLIT again later.  As a special case, if too many NYETs are
		 * received, restart the entire split transaction.  (Apparently, because
		 * of frame overruns or some other reason it's possible for NYETs to be
		 * issued indefinitely until the transaction is retried.)  */
		usb_dev_debug(req->dev, "NYET response received on channel %u\n", chan);
		if (++req->csplit_retries >= 10)
		{
			usb_dev_debug(req->dev, "Restarting split transaction "
						  "(CSPLIT tried %u times)\n", req->csplit_retries);
			req->complete_split = FALSE;
		}
		intr_status = XFER_NEEDS_TRANS_RESTART;
	}
	else if (interrupts.nak_response_received)
	{
		/* Device sent NAK packet.  This happens when the device had no data to
		 * send at this time.  Try again later.  Special case: if the NAK was
		 * sent during a Complete Split transaction, restart with the Start
		 * Split, not the Complete Split.  */
		usb_dev_debug(req->dev, "NAK response received on channel %u\n", chan);
		intr_status = XFER_NEEDS_DEFERRAL;
		req->complete_split = FALSE;
	}
	else
	{
		/* No apparent error occurred.  */
		intr_status = dwc_handle_normal_channel_halted(req, chan, interrupts);
	}

#if START_SPLIT_INTR_TRANSFERS_ON_SOF
	if ((intr_status == XFER_NEEDS_RESTART ||
		 intr_status == XFER_NEEDS_TRANS_RESTART) &&
		usb_is_interrupt_request(req) && req->dev->speed != USB_SPEED_HIGH &&
		!req->complete_split)
	{
		intr_status = XFER_NEEDS_DEFERRAL;
		req->need_sof = 1;
	}
#endif

	switch (intr_status)
	{
		case XFER_COMPLETE:
			req->status = USB_STATUS_SUCCESS;
			break;
		case XFER_FAILED:
			req->status = USB_STATUS_HARDWARE_ERROR;
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

	/* Transfer complete, transfer encountered an error, or transfer needs to be
	 * retried later.  */

	/* Save the data packet ID.  */
	req->next_data_pid = chanptr->transfer.packet_id;

	/* Clear and disable interrupts on this channel.  */
	chanptr->interrupt_mask.val = 0;
	chanptr->interrupts.val = 0xffffffff;

	/* Release the channel.  */
	channel_pending_xfers[chan] = NULL;
	dwc_release_channel(chan);

	/* Set the actual transferred size, unless we are doing a control transfer
	 * and aren't on the DATA phase.  */
	if (!usb_is_control_request(req) || req->control_phase == 1)
	{
		req->actual_size = req->cur_data_ptr - req->recvbuf;
	}

	/* If we got here because we received a NAK or NYET, defer the request for a
	 * later time.  */
	if (intr_status == XFER_NEEDS_DEFERRAL)
	{
		usb_status_t status = defer_xfer(req);
		if (status == USB_STATUS_SUCCESS)
		{
			return;
		}
		else
		{
			req->status = status;
		}
	}

	/* If we got here because the transfer successfully completed or an error
	 * occurred, call the device-driver-provided completion callback.  */
	usb_complete_xfer(req);
}

/**
 * Interrupt handler function for the Synopsys DesignWare Hi-Speed USB 2.0
 * On-The-Go Controller (DWC).  This should only be called when an interrupt
 * this driver explicitly enabled is pending.  See the comment above
 * dwc_setup_interrupts() for an overview of interrupts on this hardware.
 */
static interrupt
dwc_interrupt_handler(void)
{
	/* Set 'resdefer' to prevent other threads from being scheduled before this
	 * interrupt handler finishes.  This prevents this interrupt handler from
	 * being executed re-entrantly.  */
	extern int resdefer;
	resdefer = 1;

	union dwc_core_interrupts interrupts = regs->core_interrupts;

#if START_SPLIT_INTR_TRANSFERS_ON_SOF
	if (interrupts.sof_intr)
	{
		/* Start of frame (SOF) interrupt occurred.  */

		log_debug("Received SOF intr (host_frame_number=0x%08x)",
				  regs->host_frame_number);
		if ((regs->host_frame_number & 0x7) != 6)
		{
			union dwc_core_interrupts tmp;

			if (sofwait != 0)
			{
				uint chan;

				/* Wake up one channel waiting for SOF */

				chan = first_set_bit(sofwait);
				send(channel_pending_xfers[chan]->deferer_thread_tid, 0);
				sofwait &= ~(1 << chan);
			}

			/* Disable SOF interrupt if no longer needed */
			if (sofwait == 0)
			{
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

	if (interrupts.host_channel_intr)
	{
		/* One or more channels has an interrupt pending.  */

		uint32_t chintr;
		uint chan;

		/* A bit in the "Host All Channels Interrupt Register" is set if an
		 * interrupt has occurred on the corresponding host channel.  Process
		 * all set bits.  */
		chintr = regs->host_channels_interrupt;
		do
		{
			chan = first_set_bit(chintr);
			dwc_handle_channel_halted_interrupt(chan);
			chintr ^= (1 << chan);
		} while (chintr != 0);
	}
	if (interrupts.port_intr)
	{
		/* Status of the host port changed.  Update host_port_status.  */

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
		 * Port Control and Status register back to itself.  But as a special
		 * case, 'enabled' must be written as 0; otherwise the port will
		 * apparently disable itself.  */
		hw_status.enabled = 0;
		regs->host_port_ctrlstatus = hw_status;

		/* Complete status change request to the root hub if one has been
		 * submitted.  */
		dwc_host_port_status_changed();
	}

	/* Reschedule the currently running thread if the interrupt handler
	 * attempted to wake up any threads (for example, threads that might be
	 * waiting for a USB transfer to complete).  */
	if (--resdefer > 0)
	{
		resdefer = 0;
		resched();
	}
}

/**
 * Performs initial setup of the Synopsys Designware USB 2.0 On-The-Go
 * Controller (DWC) interrupts.
 *
 * The DWC contains several levels of interrupt registers, detailed in the
 * following list.  Note that for each level, each bit of the "interrupt"
 * register contains the state of a pending interrupt (1 means interrupt
 * pending; write 1 to clear), while the "interrupt mask" register has the same
 * format but is used to turn the corresponding interrupt on or off (1 means on;
 * write 1 to turn on; write 0 to turn off).
 *
 * - The AHB configuration register contains a mask bit used to enable/disable
 *   all interrupts whatsoever from the DWC hardware.
 * - The "Core" interrupt and interrupt mask registers control top-level
 *   interrupts.  For example, a single bit in these registers corresponds to
 *   all channel interrupts.
 * - The "Host All Channels" interrupt and interrupt mask registers control all
 *   interrupts on each channel.
 * - The "Channel" interrupt and interrupt mask registers, of which one copy
 *   exists for each channel, control individual interrupt types on that
 *   channel.
 *
 * We can assume that an interrupt only occurs if it is enabled in all the
 * places listed above.  Furthermore, it only seems to work to clear interrupts
 * at the lowest level; for example, a channel interrupt must be cleared in its
 * individual channel interrupt register rather than in one of the higher level
 * interrupt registers.
 *
 * The above just covers the DWC-specific interrupt registers.  In addition to
 * those, the system will have other ways to control interrupts.  For example,
 * on the BCM2835 (Raspberry Pi), the interrupt line going to the DWC is just
 * one of many dozen and can be enabled/disabled using the interrupt controller.
 * In the code below we enable this interrupt line and register a handler
 * function so that we can actually get interrupts from the DWC.
 *
 * And all that's in addition to the CPSR of the ARM processor itself, or the
 * equivalent on other CPUs.  So all in all, you literally have to enable
 * interrupts in 6 different places to get an interrupt when a USB transfer has
 * completed.
 */
static void
dwc_setup_interrupts(void)
{
	union dwc_core_interrupts core_interrupt_mask;

	/* Clear all pending core interrupts.  */
	regs->core_interrupt_mask.val = 0;
	regs->core_interrupts.val = 0xffffffff;

	/* Enable core host channel and port interrupts.  */
	core_interrupt_mask.val = 0;
	core_interrupt_mask.host_channel_intr = 1;
	core_interrupt_mask.port_intr = 1;
	regs->core_interrupt_mask = core_interrupt_mask;

	/* Enable the interrupt line that goes to the USB controller and register
	 * the interrupt handler.  */
	interruptVector[IRQ_USB] = dwc_interrupt_handler;
	enable_irq(IRQ_USB);

	/* Enable interrupts for entire USB host controller.  (Yes that's what we
	 * just did, but this one is controlled by the host controller itself.)  */
	regs->ahb_configuration |= DWC_AHB_INTERRUPT_ENABLE;
}

/**
 * Queue of USB transfer requests that have been submitted to the Host
 * Controller Driver but not yet started on a channel.
 */
static mailbox hcd_xfer_mailbox;

/**
 * USB transfer request scheduler thread:  This thread repeatedly waits for next
 * USB transfer request that needs to be scheduled, waits for a free channel,
 * then starts the transfer request on that channel.  This is obviously a very
 * simplistic scheduler as it does not take into account bandwidth requirements
 * or which endpoint a transfer is for.
 *
 * @return
 *      This thread never returns.
 */
static thread
dwc_schedule_xfer_requests(void)
{
	uint chan;
	struct usb_xfer_request *req;

	for (;;)
	{
		/* Get next transfer request.  */
		req = (struct usb_xfer_request*)mailboxReceive(hcd_xfer_mailbox);
		if (is_root_hub(req->dev))
		{
			/* Special case: request is to the root hub.  Fake it. */
			dwc_process_root_hub_request(req);
		}
		else
		{
			/* Normal case: schedule the transfer on some channel.  */
			chan = dwc_get_free_channel();
			dwc_channel_start_xfer(chan, req);
		}
	}
	return SYSERR;
}

/**
 * Initialize a bitmask and semaphore that keep track of the free/inuse status
 * of the host channels and a queue in which to place submitted USB transfer
 * requests, then start the USB transfer request scheduler thread.
 */
static usb_status_t
dwc_start_xfer_scheduler(void)
{
	hcd_xfer_mailbox = mailboxAlloc(1024);
	if (SYSERR == hcd_xfer_mailbox)
	{
		return USB_STATUS_OUT_OF_MEMORY;
	}

	chfree_sema = semcreate(DWC_NUM_CHANNELS);
	if (SYSERR == chfree_sema)
	{
		mailboxFree(hcd_xfer_mailbox);
		return USB_STATUS_OUT_OF_MEMORY;
	}
	STATIC_ASSERT(DWC_NUM_CHANNELS <= 8 * sizeof(chfree));
	chfree = (1 << DWC_NUM_CHANNELS) - 1;

	dwc_xfer_scheduler_tid = create(dwc_schedule_xfer_requests,
									XFER_SCHEDULER_THREAD_STACK_SIZE,
									XFER_SCHEDULER_THREAD_PRIORITY,
									XFER_SCHEDULER_THREAD_NAME, 0);
	if (SYSERR == ready(dwc_xfer_scheduler_tid, RESCHED_NO))
	{
		semfree(chfree_sema);
		mailboxFree(hcd_xfer_mailbox);
		return USB_STATUS_OUT_OF_MEMORY;
	}
	return USB_STATUS_SUCCESS;
}

/* Implementation of hcd_start() for the DesignWare Hi-Speed USB 2.0 On-The-Go
 * Controller.  See usb_hcdi.h for the documentation of this interface of the
 * Host Controller Driver.  */
usb_status_t
hcd_start(void)
{
	usb_status_t status;

	status = dwc_power_on();
	if (status != USB_STATUS_SUCCESS)
	{
		return status;
	}
	dwc_soft_reset();
	dwc_setup_dma_mode();
	dwc_setup_interrupts();
	status = dwc_start_xfer_scheduler();
	if (status != USB_STATUS_SUCCESS)
	{
		dwc_power_off();
	}
	return status;
}

/* Implementation of hcd_stop() for the DesignWare Hi-Speed USB 2.0 On-The-Go
 * Controller.  See usb_hcdi.h for the documentation of this interface of the
 * Host Controller Driver.  */
void
hcd_stop(void)
{
	/* Disable IRQ line and handler.  */
	disable_irq(IRQ_USB);
	interruptVector[IRQ_USB] = NULL;

	/* Stop transfer scheduler thread.  */
	kill(dwc_xfer_scheduler_tid);

	/* Free USB transfer request mailbox.  */
	mailboxFree(hcd_xfer_mailbox);

	/* Free unneeded semaphore.  */
	semfree(chfree_sema);

	/* Power off USB hardware.  */
	dwc_power_off();
}

/* Implementation of hcd_submit_xfer_request() for the DesignWare Hi-Speed USB
 * 2.0 On-The-Go Controller.  See usb_hcdi.h for the documentation of this
 * interface of the Host Controller Driver.  */
/**
 * @details
 *
 * This Host Controller Driver implements this interface asynchronously, as
 * intended.  Furthermore, it uses a simplistic scheduling algorithm where it
 * places transfer requests into a single queue and executes them in the order
 * they were submitted.  Transfers that need to be retried, including periodic
 * transfers that receive a NAK reply and split transactions that receive a NYET
 * reply when doing the Complete Split transaction, are scheduled to be retried
 * at an appropriate time by separate code that shortcuts the main queue when
 * the timer expires.
 *
 * Jump to dwc_schedule_xfer_requests() to see what happens next.
 */
usb_status_t
hcd_submit_xfer_request(struct usb_xfer_request *req)
{
	mailboxSend(hcd_xfer_mailbox, (int)req);
	return USB_STATUS_SUCCESS;
}
