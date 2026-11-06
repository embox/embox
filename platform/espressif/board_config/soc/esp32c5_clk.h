/* ESP32-C5 peripheral clock IDs and logical clock sources. */
#ifndef SOC_ESP32C5_CLK_H_
#define SOC_ESP32C5_CLK_H_

#define ESP32_CLK_UART0 0
#define ESP32_CLK_UART1 1
#define ESP32_CLK_SPI2  2
#define ESP32_CLK_I2C0  3
#define ESP32_CLK_COUNT 4

#define ESP32_CLK_SRC_XTAL    0
#define ESP32_CLK_SRC_PLL80M  1
#define ESP32_CLK_SRC_PLL160M 2
#define ESP32_CLK_SRC_COUNT   3

#endif /* SOC_ESP32C5_CLK_H_ */
