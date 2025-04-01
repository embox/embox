/**
 * @file
 * @brief
 *
 * @date    07.06.2021
 * @author  kpishere
 */

#include <assert.h>
#include <hal/reg.h>
#include <drivers/common/memory.h>
#include <embox/unit.h>
#include <lib/libds/array.h>
#include <kernel/printk.h>
#include <kernel/panic.h>
#include <kernel/irq.h>

#include <framework/mod/options.h>
#include "bcm283x_gpio.h"
#include <asm/delay.h>
#include <drivers/gpio/gpio_driver.h>

#define REG_BITS 32

#define PBASE                       OPTION_GET(NUMBER,base_addr)   
#define BCM283X_PINS_NUMBER         OPTION_GET(NUMBER,gpio_pins)
#define BCM283X_GPIO_PORTS_COUNT    OPTION_GET(NUMBER,gpio_ports)
#define GPU_IRQ                     OPTION_GET(NUMBER,gpu_irq_int)

#define BCM283X_PULL_PORTA_DEFLT_FLOAT    OPTION_GET(NUMBER,gpu_pullup_porta_float)
#define BCM283X_PULL_PORTA_DEFLT_DOWN     OPTION_GET(NUMBER,gpu_pullup_porta_down)
#define BCM283X_PULL_PORTA_DEFLT_UP       OPTION_GET(NUMBER,gpu_pullup_porta_up)

#define BCM283X_PULL_PORTB_DEFLT_FLOAT    OPTION_GET(NUMBER,gpu_pullup_portb_float)
#define BCM283X_PULL_PORTB_DEFLT_DOWN     OPTION_GET(NUMBER,gpu_pullup_portb_down)
#define BCM283X_PULL_PORTB_DEFLT_UP       OPTION_GET(NUMBER,gpu_pullup_portb_up)

#define PORT_PIN_TO_PINNUMBER(pt, pn)   ( ( ((pt) & 0x3 ) * REG_BITS ) + ((pn) % REG_BITS )) 
#define GPIO_ALTERNATE_INVERT(af) ( ((af) >> 17) & 0x07 )
#define BCM283X_GPIO_CHIP_ID    0

#define IRQ_NR_PORTA  49
#define IRQ_NR_PORTB  50
#define IRQ_NR_PORTC  51
#define IRQ_NR_ALL    52

typedef enum _GpioPullUpDn {
    GPUDNone = 0,
    GPUDDown = 1,
    GPUDUp  = 2
} GpioPullUpDn;

#define GPIO_PULL_NONE_UP_DN_STATES 3

struct GpioPinData {
    /* 0 (reserved) - Pin output set 2 - GPIO pins 64-96 (reserved on Pi Zero)
     * 1 (data[0]) - Pin output set 0 - GPIO pins 0-31
     * 2 (data[1])- Pin output set 1 - GPIO pins 32-53 (54-63 not connected/reserved)
     */
    volatile uint32_t reserved;
    volatile uint32_t data[BCM283X_GPIO_PORTS_COUNT];  
};

struct GpioRegs {
    volatile uint32_t func_select[3 * BCM283X_GPIO_PORTS_COUNT];
    struct GpioPinData output_set;              // Set as output
    struct GpioPinData output_clear;            // Set as input, is default
    struct GpioPinData level;                   // Read for value of pin
    struct GpioPinData ev_detect_status;        // Edge event detected
    struct GpioPinData re_detect_enable;        // Synchoronous rising edge event
    struct GpioPinData fe_detect_enable;        // Synchoronous falling edge event
    struct GpioPinData hi_detect_enable;        // Hi state detected
    struct GpioPinData lo_detect_enable;        // Lo state detected
    struct GpioPinData re_async_detect_enable;  // Enable async rising detection in ev_detect_status
                                                //     (no sampling by clock cycle for edge detection)
    struct GpioPinData fe_async_detect_enable;  // Enable async falling detection in ev_detect_status
    volatile uint32_t reserved;
    volatile uint32_t pupd_enable;              // See func bcm283x_gpio_pin_pullupdown() below
    volatile uint32_t pupd_enable_clocks[BCM283X_GPIO_PORTS_COUNT];
};

#define REGS_GPIO ((struct GpioRegs *)(PBASE))

//// Globals
//
static uint32_t _pinMaskPortA[GPIO_PULL_NONE_UP_DN_STATES];
static uint32_t _pinMaskPortB[GPIO_PULL_NONE_UP_DN_STATES];
//
////

// Assign desired pull state for one port+pin location, clear the other states for that port+pin
static void bcm293x_gpio_assign_pullup(GpioPullUpDn pull, unsigned char port, gpio_mask_t pin) {
    uint32_t *pullPorts[BCM283X_GPIO_PORTS_COUNT] = {_pinMaskPortA, _pinMaskPortB};
    GpioPullUpDn stateSelection[GPIO_PULL_NONE_UP_DN_STATES];
    switch(pull) {
        case GPUDUp:
            stateSelection[0] = GPUDUp;
            stateSelection[1] = GPUDDown;
            stateSelection[2] = GPUDNone;
        break;
        case GPUDDown:
            stateSelection[0] = GPUDDown;
            stateSelection[1] = GPUDNone;
            stateSelection[2] = GPUDUp;
        break;
        case GPUDNone:
            stateSelection[0] = GPUDNone;
            stateSelection[1] = GPUDUp;
            stateSelection[2] = GPUDDown;
        break;
    }
    pullPorts[port][stateSelection[0]] |= (1 << pin);
    pullPorts[port][stateSelection[1]] &= ~(1 << pin);
    pullPorts[port][stateSelection[2]] &= ~(1 << pin);
}

///
/// The function is set for every pin that has high value set on port
///
static void bcm283x_gpio_pin_set_func(unsigned char port, gpio_mask_t pins, GpioFunc func) {
    uint32_t pinNumber, selector;
    uint8_t bitStart, reg;

    for(unsigned short r = 0; r < REG_BITS; r++ ) {
        if( (pins >> r) & 0x01 ) {  // If bit is set, apply function
            pinNumber = PORT_PIN_TO_PINNUMBER(port, r);
            bitStart = (pinNumber * 3) % 30;
            reg = pinNumber / 10;

            selector = REGS_GPIO->func_select[reg];
            selector &= ~(7 << bitStart);
            selector |= (func << bitStart);

            REGS_GPIO->func_select[reg] = selector;
        }
    }
}

/// This change must be applied accross all pins in one action.  The state can't be read back. 
/// The change persists over a power on/off cycle.
static void bcm283x_gpio_pin_pullupdown(GpioPullUpDn pullState, uint32_t pinMaskPortA, uint32_t pinMaskPortB) {
    // Setup state to change to
    REGS_GPIO->pupd_enable = pullState;
    delay(150);
    // Indicate pins to change
    REGS_GPIO->pupd_enable_clocks[GPIO_PORT_A] = pinMaskPortA;
    REGS_GPIO->pupd_enable_clocks[GPIO_PORT_B] = pinMaskPortB;
    delay(150);
    // Clear values to indicate change complete
    REGS_GPIO->pupd_enable = 0;
    REGS_GPIO->pupd_enable_clocks[GPIO_PORT_A] = 0;
    REGS_GPIO->pupd_enable_clocks[GPIO_PORT_B] = 0;
}

///
/// port = port A or B
/// pins = a bit field for that port
/// mode = a bit field indicating mode of operation
///
static int bcm283x_gpio_setup_mode(unsigned char port, gpio_mask_t pins, int mode) {
    assert(((port == GPIO_PORT_A) || (port == GPIO_PORT_B)));

    // Determine GPIO Function of Input/Output/Alternate X
    if(mode & GPIO_MODE_ALT_SECTION) {
        bcm283x_gpio_pin_set_func(port,pins,GPIO_ALTERNATE_INVERT(mode));
    } else if (mode & (GPIO_MODE_IN 
        | GPIO_MODE_IN_PULL_UP | GPIO_MODE_IN_PULL_DOWN 
        | GPIO_MODE_IN_SCHMITT 
        | GPIO_MODE_INT_MODE_LEVEL0 | GPIO_MODE_INT_MODE_LEVEL1
        | GPIO_MODE_INT_MODE_RISING | GPIO_MODE_INT_MODE_FALLING )) {
        bcm283x_gpio_pin_set_func(port,pins,GFInput);        
    } else {
        bcm283x_gpio_pin_set_func(port,pins,GFOutput);        
    }

    // Synchornous rising edge
    if( mode & GPIO_MODE_INT_MODE_RISING ) {
        REGS_GPIO->re_detect_enable.data[port] |= pins;
        REGS_GPIO->ev_detect_status.data[port] |= pins;   
    } else {
        REGS_GPIO->re_detect_enable.data[port] &= ~pins;
    }

    // Synchornous falling edge
    if( mode & GPIO_MODE_INT_MODE_FALLING ) {
        REGS_GPIO->fe_detect_enable.data[port] |= pins;
        REGS_GPIO->ev_detect_status.data[port] |= pins;        
    } else {
        REGS_GPIO->fe_detect_enable.data[port] &= ~pins;
    }

    // Level Lo
    if(  mode & GPIO_MODE_INT_MODE_LEVEL0 ) {
        REGS_GPIO->lo_detect_enable.data[port] |= pins;
        REGS_GPIO->ev_detect_status.data[port] |= pins;        
    } else {
        REGS_GPIO->lo_detect_enable.data[port] &= ~pins;
    }

    // Level Hi
    if(  mode & GPIO_MODE_INT_MODE_LEVEL1 ) {
        REGS_GPIO->hi_detect_enable.data[port] |= pins;
        REGS_GPIO->ev_detect_status.data[port] |= pins;        
    } else {
        REGS_GPIO->hi_detect_enable.data[port] &= ~pins;
    }

    // Async mode = no sampleing, synch = sampleing 
    // (kind of like a schmitt trigger in that debouncing happens)
    if(mode & GPIO_MODE_IN_SCHMITT && mode & GPIO_MODE_INT_MODE_RISING) {
        REGS_GPIO->re_async_detect_enable.data[port] &= ~pins;
    } else {
        REGS_GPIO->re_async_detect_enable.data[port] |= pins;
    }

    // Async mode = no sampleing, synch = sampleing 
    // (kind of like a schmitt trigger in that debouncing happens)
    if(mode & GPIO_MODE_IN_SCHMITT && mode & GPIO_MODE_INT_MODE_FALLING) {
        REGS_GPIO->fe_async_detect_enable.data[port] &= ~pins;
    } else {
        REGS_GPIO->fe_async_detect_enable.data[port] |= pins;
    }

    // If no interrupt is requested, disable event interrupts
    //
    if(!(  mode & GPIO_MODE_INT_MODE_LEVEL1 
        || mode & GPIO_MODE_INT_MODE_LEVEL0 
        || mode & GPIO_MODE_INT_MODE_RISING
        || mode & GPIO_MODE_INT_MODE_FALLING)) {
        REGS_GPIO->ev_detect_status.data[port] &= ~pins;
        REGS_GPIO->fe_async_detect_enable.data[port] &= ~pins;
        REGS_GPIO->re_async_detect_enable.data[port] &= ~pins;
    }

    // Assign pull up / pull down -- floating isn't assigned as it is default state
    if(mode & (GPIO_MODE_IN_PULL_UP | GPIO_MODE_OUT_PUSH_PULL) ) {
        bcm293x_gpio_assign_pullup(GPUDUp, port, pins); 
    }
    if(mode & (GPIO_MODE_IN_PULL_DOWN | GPIO_MODE_VDD_LEVEL) ) {
        bcm293x_gpio_assign_pullup(GPUDDown, port, pins); 
    }
    
    // Assign pull up/down/float for all pins in one go (only way allowed)
    for( GpioPullUpDn i = 0; i < GPIO_PULL_NONE_UP_DN_STATES; i++) {
        bcm283x_gpio_pin_pullupdown(i, _pinMaskPortA[i], _pinMaskPortB[i]);
    }

    return 0;
}

static void bcm283x_gpio_set(unsigned char port, gpio_mask_t pins, char level) {
    assert(((port == GPIO_PORT_A) || (port == GPIO_PORT_B)));
    
    if (level) {
        REGS_GPIO->output_set.data[port] |= pins;
    } else {
        REGS_GPIO->output_clear.data[port] |= pins;
    }
}

static gpio_mask_t bcm283x_gpio_get(unsigned char port, gpio_mask_t pins) {
    assert(((port == GPIO_PORT_A) || (port == GPIO_PORT_B)));

    return REGS_GPIO->level.data[port] & pins;
}

static struct gpio_chip bcm283x_gpio_chip = {
    .setup_mode = bcm283x_gpio_setup_mode,
    .get = bcm283x_gpio_get,
    .set = bcm283x_gpio_set,
    .nports = BCM283X_GPIO_PORTS_COUNT,
	.chip_id = BCM283X_GPIO_CHIP_ID,
};

GPIO_CHIP_DEF(&bcm283x_gpio_chip);

irq_return_t bcm283x_gpio_irq_handler(unsigned int irq_nr, void *data) {
    uint16_t ports[BCM283X_GPIO_PORTS_COUNT] = {0,0};

    ports[0] = (IRQ_NR_PORTA == irq_nr || IRQ_NR_ALL == irq_nr );
    ports[1] = (IRQ_NR_PORTB == irq_nr || IRQ_NR_ALL == irq_nr );

    for(uint16_t port = 0; port < BCM283X_GPIO_PORTS_COUNT; port++) {
        if(ports[port]) {
            uint32_t pins = REGS_GPIO->ev_detect_status.data[port];

            // Clear the interrupt event on GPIO port
            REGS_GPIO->ev_detect_status.data[port] = 0xFFFFFFFF;

            gpio_handle_irq(&bcm283x_gpio_chip, port, pins);            
        }
    }

    return IRQ_HANDLED;
}

static int bcm283x_gpio_init(void) {
    // We don't know the pull up/down state of GPIO pins
    // so we'll set the change state to a default assignment.
    // It is expected that pin PINNUMBER_APPLY_CONFIG will be set at 
    // end of config in kernel! 
    uint32_t a[GPIO_PULL_NONE_UP_DN_STATES] = {BCM283X_PULL_PORTA_DEFLT_FLOAT, BCM283X_PULL_PORTA_DEFLT_DOWN, BCM283X_PULL_PORTA_DEFLT_UP};
    uint32_t b[GPIO_PULL_NONE_UP_DN_STATES] = {BCM283X_PULL_PORTB_DEFLT_FLOAT, BCM283X_PULL_PORTB_DEFLT_DOWN, BCM283X_PULL_PORTB_DEFLT_UP};

    for( GpioPullUpDn i = GPUDNone; i < GPIO_PULL_NONE_UP_DN_STATES; i++ ) {
        _pinMaskPortA[i] = a[i];
        _pinMaskPortB[i] = b[i];
    }

    return irq_attach(GPU_IRQ, bcm283x_gpio_irq_handler, 0, NULL, "BCM283x GPIO irq handler");
};

EMBOX_UNIT_INIT(bcm283x_gpio_init);

PERIPH_MEMORY_DEFINE(bcm283x_gpio, PBASE, sizeof(struct GpioRegs));
