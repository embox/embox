#ifndef ESP32C5_GPIO_H_
#define ESP32C5_GPIO_H_

#define GPIO_BASE                            0x60091000
#define IO_MUX_BASE                          0x60090000

/* IO_MUX function selectors. */
#define FUNC_U0TXD_U0TXD                     0
#define FUNC_U0RXD_U0RXD                     0
#define FUNC_MTMS_FSPIQ                       2
#define FUNC_GPIO6_FSPICLK                    2
#define FUNC_GPIO7_FSPID                      2
#define FUNC_GPIO10_FSPICS0                   2

/* GPIO pin numbers. */
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
