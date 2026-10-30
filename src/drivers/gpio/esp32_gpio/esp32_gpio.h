#ifndef ESP32_GPIO_H_
#define ESP32_GPIO_H_

#include <stdint.h>

/* Packed AF value for gpio_setup_route(), not GPIO_MODE_ALT_SET().
 * Plain IO_MUX function numbers retain route type zero.
 */
#define ESP32_ROUTE_VALUE_MASK                0xffffU
#define ESP32_ROUTE_TYPE_SHIFT                16
#define ESP32_ROUTE_TYPE_MASK                 0xffffU

#define ESP32_ROUTE_IOMUX                     0U
#define ESP32_ROUTE_MATRIX_IN                 1U
#define ESP32_ROUTE_MATRIX_OUT                2U

#define ESP32_ROUTE(type, value) \
	((((type) & ESP32_ROUTE_TYPE_MASK) << ESP32_ROUTE_TYPE_SHIFT) | \
	 ((value) & ESP32_ROUTE_VALUE_MASK))
#define ESP32_ROUTE_TYPE(route) \
	(((route) >> ESP32_ROUTE_TYPE_SHIFT) & ESP32_ROUTE_TYPE_MASK)
#define ESP32_ROUTE_VALUE(route) \
	((route) & ESP32_ROUTE_VALUE_MASK)

#define ESP32_MATRIX_IN(signal) \
	ESP32_ROUTE(ESP32_ROUTE_MATRIX_IN, signal)
#define ESP32_MATRIX_OUT(signal) \
	ESP32_ROUTE(ESP32_ROUTE_MATRIX_OUT, signal)


#define ESP32_GPIO_PIN_COUNT       29U
#define ESP32_GPIO_PIN_MASK        ((1U << ESP32_GPIO_PIN_COUNT) - 1U)
#define ESP32_GPIO_SIGNAL_COUNT    256U
#define ESP32_GPIO_SIG_IN_SEL      (1U << 8)
#define ESP32_GPIO_PAD_DRIVER      (1U << 2)
#define ESP32_GPIO_OUT_GPIO        256U
#define ESP32_GPIO_OUT_OE_SEL      (1U << 10)
#define ESP32_IO_MUX_FUNC_GPIO     1U
#define ESP32_IO_MUX_FUNC_MASK     (7U << 12)
#define ESP32_IO_MUX_FUNC_SHIFT    12
#define ESP32_IO_MUX_INPUT_ENABLE  (1U << 9)
#define ESP32_IO_MUX_PULL_UP       (1U << 8)
#define ESP32_IO_MUX_PULL_DOWN     (1U << 7)

/* ESP32-C5 register layout; offsets are relative to GPIO_BASE. */
struct esp32_gpio_regs {
	volatile uint32_t strap;                 /* 0x000 */
	volatile uint32_t out;                   /* 0x004 */
	volatile uint32_t out_w1ts;
	volatile uint32_t out_w1tc;
	volatile uint32_t out1;
	volatile uint32_t out1_w1ts;
	volatile uint32_t out1_w1tc;
	uint32_t reserved_01c[6];
	volatile uint32_t enable;                /* 0x034 */
	volatile uint32_t enable_w1ts;
	volatile uint32_t enable_w1tc;
	volatile uint32_t enable1;
	volatile uint32_t enable1_w1ts;
	volatile uint32_t enable1_w1tc;
	uint32_t reserved_04c[6];
	volatile uint32_t in;                    /* 0x064 */
	volatile uint32_t in1;
	uint32_t reserved_06c[2];
	volatile uint32_t status;                /* 0x074 */
	volatile uint32_t status_w1ts;
	volatile uint32_t status_w1tc;
	volatile uint32_t status1;
	volatile uint32_t status1_w1ts;
	volatile uint32_t status1_w1tc;
	uint32_t reserved_08c[6];
	volatile uint32_t pcpu_int;              /* 0x0a4 */
	volatile uint32_t sdio_int;
	volatile uint32_t pcpu_int1;
	volatile uint32_t sdio_int1;
	uint32_t reserved_0b4[4];
	volatile uint32_t status_next;           /* 0x0c4 */
	volatile uint32_t status_next1;
	uint32_t reserved_0cc[2];
	volatile uint32_t pin[29];               /* 0x0d4: GPIO0..28 */
	uint32_t reserved_148[99];
	volatile uint32_t func_in_sel_cfg[256];  /* 0x2d4: signal index */
	uint32_t reserved_6d4[256];
	volatile uint32_t func_out_sel_cfg[29];  /* 0xad4: GPIO0..28 */
	uint32_t reserved_b48[172];
	volatile uint32_t clock_gate;            /* 0xdf8 */
	volatile uint32_t date;                  /* 0xdfc */
};

/* ESP32-C5 IO_MUX: pin[n] controls GPIO n, relative to IO_MUX_BASE. */
struct esp32_io_mux_regs {
	volatile uint32_t pin[29];               /* 0x000..0x070 */
	uint32_t reserved_074[98];
	volatile uint32_t date;                  /* 0x1fc */
};

#endif /* ESP32_GPIO_H_ */
