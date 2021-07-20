/**
 * @file
 * @brief Mailbox Property interface for Raspberry Pi
 *
 * @date 2021.07.05
 * @author kpishere (ref: https://www.valvers.com/open-software/raspberry-pi/bare-metal-programming-in-c-part-5/)
 */

#ifndef BCM2835_MAILBOX_INTERFACE_H
#define BCM2835_MAILBOX_INTERFACE_H

/**
    @brief An enum of the RPI->Videocore firmware mailbox property interface
    properties. Further details are available from
    https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface

    For revision codes: https://www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
*/
typedef enum {
    /* Videocore */
    TAG_GET_FIRMWARE_VERSION = 0x1,

    /* Hardware */
    TAG_GET_BOARD_MODEL = 0x10001,
    TAG_GET_BOARD_REVISION,
    TAG_GET_BOARD_MAC_ADDRESS,
    TAG_GET_BOARD_SERIAL,
    TAG_GET_ARM_MEMORY,
    TAG_GET_VC_MEMORY,
    TAG_GET_CLOCKS,

    /* Config */
    TAG_GET_COMMAND_LINE = 0x50001,

    /* Shared resource management */
    TAG_GET_DMA_CHANNELS = 0x60001,

    /* Power */
    TAG_GET_POWER_STATE = 0x20001,
    TAG_GET_TIMING,
    TAG_SET_POWER_STATE = 0x28001,

    /* Clocks */
    TAG_GET_CLOCK_STATE = 0x30001,
    TAG_SET_CLOCK_STATE = 0x38001,
    TAG_GET_CLOCK_RATE = 0x30002,
    TAG_SET_CLOCK_RATE = 0x38002,
    TAG_GET_MAX_CLOCK_RATE = 0x30004,
    TAG_GET_MIN_CLOCK_RATE = 0x30007,
    TAG_GET_TURBO = 0x30009,
    TAG_SET_TURBO = 0x38009,

    /* Voltage */
    TAG_GET_VOLTAGE = 0x30003,
    TAG_SET_VOLTAGE = 0x38003,
    TAG_GET_MAX_VOLTAGE = 0x30005,
    TAG_GET_MIN_VOLTAGE = 0x30008,
    TAG_GET_TEMPERATURE = 0x30006,
    TAG_GET_MAX_TEMPERATURE = 0x3000A,
    TAG_ALLOCATE_MEMORY = 0x3000C,
    TAG_LOCK_MEMORY = 0x3000D,
    TAG_UNLOCK_MEMORY = 0x3000E,
    TAG_RELEASE_MEMORY = 0x3000F,
    TAG_EXECUTE_CODE = 0x30010,
    TAG_GET_DISPMANX_MEM_HANDLE = 0x30014,
    TAG_GET_EDID_BLOCK = 0x30020,

    /* Framebuffer */
    TAG_ALLOCATE_BUFFER = 0x40001,
    TAG_RELEASE_BUFFER = 0x48001,
    TAG_BLANK_SCREEN = 0x40002,
    TAG_GET_PHYSICAL_SIZE = 0x40003,
    TAG_TEST_PHYSICAL_SIZE = 0x44003,
    TAG_SET_PHYSICAL_SIZE = 0x48003,
    TAG_GET_VIRTUAL_SIZE = 0x40004,
    TAG_TEST_VIRTUAL_SIZE = 0x44004,
    TAG_SET_VIRTUAL_SIZE = 0x48004,
    TAG_GET_DEPTH = 0x40005,
    TAG_TEST_DEPTH = 0x44005,
    TAG_SET_DEPTH = 0x48005,
    TAG_GET_PIXEL_ORDER = 0x40006,
    TAG_TEST_PIXEL_ORDER = 0x44006,
    TAG_SET_PIXEL_ORDER = 0x48006,
    TAG_GET_ALPHA_MODE = 0x40007,
    TAG_TEST_ALPHA_MODE = 0x44007,
    TAG_SET_ALPHA_MODE = 0x48007,
    TAG_GET_PITCH = 0x40008,
    TAG_GET_VIRTUAL_OFFSET = 0x40009,
    TAG_TEST_VIRTUAL_OFFSET = 0x44009,
    TAG_SET_VIRTUAL_OFFSET = 0x48009,
    TAG_GET_OVERSCAN = 0x4000A,
    TAG_TEST_OVERSCAN = 0x4400A,
    TAG_SET_OVERSCAN = 0x4800A,
    TAG_GET_PALETTE = 0x4000B,
    TAG_TEST_PALETTE = 0x4400B,
    TAG_SET_PALETTE = 0x4800B,
    TAG_SET_CURSOR_INFO = 0x8011,
    TAG_SET_CURSOR_STATE = 0x8010
} bcm2835_mailbox_tag_t;

typedef enum {
    TAG_STATE_REQUEST = 0,
    TAG_STATE_RESPONSE = 1,
} bcm2835_tag_state_t;


typedef enum {
    PT_OSIZE = 0,
    PT_OREQUEST_OR_RESPONSE = 1,
} bcm2835_tag_buffer_offset_t;

typedef enum {
    T_OIDENT = 0,
    T_OVALUE_SIZE = 1,
    T_ORESPONSE = 2,
    T_OVALUE = 3,
} bcm2835_tag_offset_t;

typedef struct {
    int tag;
    int byte_length;
    union {
        int value_32;
        unsigned char buffer_8[ 256 ];
        int buffer_32[ 256 / sizeof(uint32_t) ];
    } data;
} bcm2835_mailbox_property_t;

typedef enum {
    TAG_CLOCK_RESERVED = 0,
    TAG_CLOCK_EMMC,
    TAG_CLOCK_UART,
    TAG_CLOCK_ARM,
    TAG_CLOCK_CORE,
    TAG_CLOCK_V3D,
    TAG_CLOCK_H264,
    TAG_CLOCK_ISP,
    TAG_CLOCK_SDRAM,
    TAG_CLOCK_PIXEL,
    TAG_CLOCK_PWM,
} bcm2835_tag_clock_id_t;

typedef enum {
    TAG_POWER_SDCARD = 0x00,
    TAG_POWER_UART0 = 0x01,
    TAG_POWER_UART1 = 0x02,
    TAG_POWER_USB_HCD = 0x03,
    TAG_POWER_I2C0 = 0x04,
    TAG_POWER_I2C1 = 0x05,
    TAG_POWER_I2C2 = 0x06,
    TAG_POWER_SPI = 0x07,
    TAG_POWER_CCP2TX = 0x08,
    TAG_POWER_UNKN0_PI4 = 0x09,
    TAG_POWER_UNKN1_PI4 = 0x0a
} bcm2835_tag_power_id_t;

typedef enum {
    TAG_VOLTAGE_RESERVED = 0x00,
    TAG_VOLTAGE_CORE = 0x01,
    TAG_VOLTAGE_SDRAM_C = 0x02,
    TAG_VOLTAGE_SDRAM_P = 0x03,
    TAG_VOLTAGE_SDRAM_I = 0x04
} bcm2835_tag_voltage_id_t;

// Allocate memory flags (TAG_ALLOCATE_MEMORY)
#define BCM2835_MEM_FLAG_NORMAL             ( 0 )      /* normal allocating alias; don't use from ARM */
#define BCM2835_MEM_FLAG_DISCARDABLE        ( 1 << 0 ) /* can be resized to 0 at any time; use for cached data */
#define BCM2835_MEM_FLAG_DIRECT             ( 1 << 2 ) /* 0xC alias uncached */
#define BCM2835_MEM_FLAG_COHERENT           ( 1 << 3 ) /* 0x8 alias; non-allocating in L2 but coherent */
#define BCM2835_MEM_FLAG_L1_NONALLOCATING   ( 3 << 2 ) /* Allocating in L2 */
#define BCM2835_MEM_FLAG_ZERO               ( 1 << 4 ) /* Initialise buffer to all zeros */
#define BCM2835_MEM_FLAG_NO_INIT            ( 1 << 5 ) /* Don't initialise; default is initialise to all ones */
#define BCM2835_MEM_FLAG_HINT_PERMALOCK     ( 1 << 6 ) /* Likely to be locked for long periods of time */

extern void bcm2835_property_init( void );
extern void bcm2835_property_add_tag( bcm2835_mailbox_tag_t tag, ... );
extern int bcm2835_property_process( void );
extern bcm2835_mailbox_property_t* bcm2835_property_get( bcm2835_mailbox_tag_t tag );
extern void bcm2835_property_value32(bcm2835_mailbox_tag_t *props, uint32_t *value32);


#endif /* BCM2835_MAILBOX_INTERFACE_H */
