#ifndef ESP32C5_GPIO_H_
#define ESP32C5_GPIO_H_

#define GPIO_BASE                            0x60091000
#define IO_MUX_BASE                          0x60090000

/* IO_MUX function selectors. */
#define FUNC_U0TXD_U0TXD                     0
#define FUNC_U0RXD_U0RXD                     0
#define PIN_FUNC_GPIO                        1

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

/* GPIO pin numbers, not bit masks. */
#define GPIO0                                0
#define GPIO1                                1
#define GPIO2                                2
#define GPIO3                                3
#define GPIO4                                4
#define GPIO5                                5
#define GPIO6                                6
#define GPIO7                                7
#define GPIO8                                8
#define GPIO9                                9
#define GPIO10                               10
#define GPIO11                               11
#define GPIO12                               12
#define GPIO13                               13
#define GPIO14                               14
#define GPIO15                               15
#define GPIO16                               16
#define GPIO17                               17
#define GPIO18                               18
#define GPIO19                               19
#define GPIO20                               20
#define GPIO21                               21
#define GPIO22                               22
#define GPIO23                               23
#define GPIO24                               24
#define GPIO25                               25
#define GPIO26                               26
#define GPIO27                               27
#define GPIO28                               28
#endif /* ESP32C5_GPIO_H_ */
