
#ifndef SRC_DRIVERS_GPIO_BCM283X_BCM283X_GPIO_H_
#define SRC_DRIVERS_GPIO_BCM283X_BCM283X_GPIO_H_

// Misc. GPIO pins of note
//
#define GPIO_ONBOARD_LED_PIZERO     47

/* Example use of GpioFunc :
 *
 * A Pi Zero specific definition:
 * 
 * 	gpio_setup_mode(GPIO_PORT_A, pins_GFAlt5, GPIO_MODE_ALT_SET(GFAlt5) );
 * 
 * A generic GPIO API definition:
 * 
 * 	gpio_setup_mode(GPIO_PORT_A, 0x00000002, GPIO_MODE_INPUT );
 * 
 */

typedef enum _GpioFunc {
    GFInput = 0,
    GFOutput = 1,
    GFAlt0 = 4,
    GFAlt1 = 5,
    GFAlt2 = 6,
    GFAlt3 = 7,
    GFAlt4 = 3,
    GFAlt5 = 2
} GpioFunc;

#endif // SRC_DRIVERS_GPIO_BCM283X_BCM283X_GPIO_H_