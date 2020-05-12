/*
 * usb_example.c
 *
 *  Created on: May 8, 2020
 *      Author: anton
 */
#include <config/custom_config_qspi.h>

#include <hw_cpm.h>
#include <hw_usb.h>


bool hw_usb_ep_rx_done(uint8 ep_nr, uint8* buffer, uint16 size)
{
        return false;
}

void hw_usb_ep_tx_done(uint8 ep_nr, uint8* buffer)
{

}
#if !defined(dg_configDEVICE)
void hw_usb_bus_event(usb_bus_event_type event)
#else
void hw_usb_bus_event(HW_USB_BUS_EVENT_TYPE event)
#endif
{
}

void hw_usb_bus_frame(uint16 frame_nr)
{
    (void)frame_nr;
}

void hw_usb_ep_nak(uint8 ep_nr)
{
}

uint8* hw_usb_ep_get_rx_buffer(uint8 ep_nr, bool is_setup, uint16* buffer_size)
{

        return NULL;
}

bool hw_usb_ep_rx_read_by_driver(uint8 ep_nr)
{
	return false;
}


#define USB_EP_IO_SIZE 					64

#define USB_EP_BULK_TX 1
#define USB_EP_BULK_RX 2
#define USB_EP_INT_TX 3

// RX Int EP descriptor (Host --> DA)
const hw_usb_device_framework_ep_descriptor_t int_ep_desc =
{
    {
        sizeof(hw_usb_device_framework_ep_descriptor_t),   // uint8  bLength;
        HW_USB_DEVICE_FRAMEWORK_DT_ENDPOINT                         // uint8  bDescriptorType;
    },
    HW_USB_DEVICE_FRAMEWORK_DIR_IN | USB_EP_INT_TX,                  // uint8  bEndpointAddress;
    HW_USB_DEVICE_FRAMEWORK_ENDPOINT_XFER_INT,                          // uint8  bmAttributes;
    8,                                              // uint16 wMaxPacketSize;
    0x01                                            // uint8  bInterval
};

// TX Bulk EP descriptor (DA --> host)
const hw_usb_device_framework_ep_descriptor_t BulkEpTx = {
    {
        sizeof(hw_usb_device_framework_ep_descriptor_t),   // uint8  bLength;
        HW_USB_DEVICE_FRAMEWORK_DT_ENDPOINT                         // uint8  bDescriptorType;
    },
    HW_USB_DEVICE_FRAMEWORK_DIR_IN | USB_EP_BULK_TX,                 // uint8  bEndpointAddress;
    HW_USB_DEVICE_FRAMEWORK_ENDPOINT_XFER_BULK,                         // uint8  bmAttributes;
    USB_EP_IO_SIZE,                                 // uint16 wMaxPacketSize;
    0x0,                                            // uint8  bInterval;
};

// RX Bulk EP descriptor (Host --> DA)
const hw_usb_device_framework_ep_descriptor_t BulkEpRx = {
    {
        sizeof(hw_usb_device_framework_ep_descriptor_t),   // uint8  bLength;
        HW_USB_DEVICE_FRAMEWORK_DT_ENDPOINT                         // uint8  bDescriptorType;
    },
    HW_USB_DEVICE_FRAMEWORK_DIR_OUT | USB_EP_BULK_RX,                // uint8  bEndpointAddress;
    HW_USB_DEVICE_FRAMEWORK_ENDPOINT_XFER_BULK,                         // uint8  bmAttributes;
    USB_EP_IO_SIZE,                                 // uint16 wMaxPacketSize;
    0x0,                                            // uint8  bInterval;
};


int usb_init(void) {


    hw_usb_init();


    hw_usb_ep_configure(USB_EP_DEFAULT, true, NULL);

    hw_usb_ep_configure(USB_EP_INT_TX, false,
                    (hw_usb_device_framework_ep_descriptor_t*) &int_ep_desc);


    hw_usb_ep_configure(USB_EP_BULK_TX, false,
                    (hw_usb_device_framework_ep_descriptor_t*) &BulkEpTx);

    hw_usb_ep_configure(USB_EP_BULK_RX, false,
                    (hw_usb_device_framework_ep_descriptor_t*) &BulkEpRx);

    hw_usb_bus_attach();

    hw_usb_enable_interrupt();
    return 0;
}
