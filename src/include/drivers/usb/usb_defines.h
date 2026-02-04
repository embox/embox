/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    26.06.2020
 */

#ifndef DRIVERS_USB_DEFINES_H_
#define DRIVERS_USB_DEFINES_H_

#define USB_DIR_OUT                 0x00
#define USB_DIR_IN                  0x80

#define USB_REQ_TYPE_MASK           (0x03 << 5)
#define USB_REQ_TYPE_STANDARD       (0x00 << 5)
#define USB_REQ_TYPE_CLASS          (0x01 << 5)
#define USB_REQ_TYPE_VENDOR         (0x02 << 5)

#define USB_REQ_RECIP_MASK          0x1f
#define USB_REQ_RECIP_DEVICE        0x00
#define USB_REQ_RECIP_IFACE         0x01
#define USB_REQ_RECIP_ENDP          0x02
#define USB_REQ_RECIP_OTHER         0x03

#define USB_REQ_GET_STATUS          0x00
#define USB_REQ_CLEAR_FEATURE       0x01
#define USB_REQ_SET_FEATURE         0x03
#define USB_REQ_SET_ADDRESS         0x05
#define USB_REQ_GET_DESCRIPTOR      0x06
#define USB_REQ_GET_CONFIG          0x08
#define USB_REQ_SET_CONFIG          0x09
#define USB_REQ_SET_INTERFACE       0x0B

#define USB_PORT_FEATURE_CONNECTION      0
#define USB_PORT_FEATURE_ENABLE          1
#define USB_PORT_FEATURE_RESET           4
#define USB_PORT_FEATURE_POWER           8
#define USB_PORT_FEATURE_C_CONNECTION    16
#define USB_PORT_FEATURE_C_ENABLE        17
#define USB_PORT_FEATURE_C_RESET         20

#define USB_PORT_STAT_CONNECTION    0x0001
#define USB_PORT_STAT_ENABLE        0x0002
#define USB_PORT_STAT_SUSPEND       0x0004
#define USB_PORT_STAT_OVERCURRENT   0x0008
#define USB_PORT_STAT_RESET         0x0010

/*
 * Descriptor types ... USB 2.0 spec table 9.5
 */
#define USB_DT_DEVICE               0x01
#define USB_DT_CONFIG               0x02
#define USB_DT_STRING               0x03
#define USB_DT_INTERFACE            0x04
#define USB_DT_ENDPOINT             0x05
#define USB_DT_DEVICE_QUALIFIER     0x06
#define USB_DT_OTHER_SPEED_CONFIG   0x07
#define USB_DT_INTERFACE_POWER      0x08

/* Conventional codes for class-specific descriptors.  The convention is
 * defined in the USB "Common Class" Spec (3.11).  Individual class specs
 * are authoritative for their usage, not the "common class" writeup.
 */
#define USB_DT_CS_DEVICE        (USB_REQ_TYPE_CLASS | USB_DT_DEVICE)
#define USB_DT_CS_CONFIG        (USB_REQ_TYPE_CLASS | USB_DT_CONFIG)
#define USB_DT_CS_STRING        (USB_REQ_TYPE_CLASS | USB_DT_STRING)
#define USB_DT_CS_INTERFACE     (USB_REQ_TYPE_CLASS | USB_DT_INTERFACE)
#define USB_DT_CS_ENDPOINT      (USB_REQ_TYPE_CLASS | USB_DT_ENDPOINT)

/* USB classes */
#define USB_CLASS_COMM       0x02
#define USB_CLASS_HUB        0x09
#define USB_CLASS_CDC_DATA   0x0A

#define USB_DT_HUB     (USB_REQ_TYPE_CLASS | USB_CLASS_HUB)

/* Hub request types */
#define USB_RT_HUB  (USB_REQ_TYPE_CLASS | USB_REQ_RECIP_DEVICE)
#define USB_RT_PORT (USB_REQ_TYPE_CLASS | USB_REQ_RECIP_OTHER)

/* Class requests from the USB 2.0 hub spec, table 11-15 */
#define HUB_CLASS_REQ(dir, type, request) ((((dir) | (type)) << 8) | (request))

#define USB_CLEAR_HUB_FEATURE     HUB_CLASS_REQ(USB_DIR_OUT, USB_RT_HUB, USB_REQ_CLEAR_FEATURE)
#define USB_CLEAR_PORT_FEATURE    HUB_CLASS_REQ(USB_DIR_OUT, USB_RT_PORT, USB_REQ_CLEAR_FEATURE)
#define USB_GET_HUB_DESCRIPTOR    HUB_CLASS_REQ(USB_DIR_IN, USB_RT_HUB, USB_REQ_GET_DESCRIPTOR)
#define USB_GET_HUB_STATUS        HUB_CLASS_REQ(USB_DIR_IN, USB_RT_HUB, USB_REQ_GET_STATUS)
#define USB_GET_PORT_STATUS       HUB_CLASS_REQ(USB_DIR_IN, USB_RT_PORT, USB_REQ_GET_STATUS)
#define USB_SET_HUB_FEATURE       HUB_CLASS_REQ(USB_DIR_OUT, USB_RT_HUB, USB_REQ_SET_FEATURE)
#define USB_SET_PORT_FEATURE      HUB_CLASS_REQ(USB_DIR_OUT, USB_RT_PORT, USB_REQ_SET_FEATURE)

#define HUB_PORT_STATUS     0

/* Configuration descriptor bm_attributes */
#define USB_CFG_ATT_ONE              (1 << 7) /* should always be set */
#define USB_CFG_ATT_SELFPOWER        (1 << 6)
#define USB_CFG_ATT_WAKEUP           (1 << 5)
#define USB_CFG_ATT_BATTERY          (1 << 4)

#endif /* DRIVERS_USB_DEFINES_H_ */
