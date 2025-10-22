/**
 * @file
 * @brief ISM43362-M3G-L44-SPI Inventek eS-WiFi module library
 * @author Daria Pechenova
 * @version
 * @date 18.07.2025
 */

#ifndef ISM43362_CONFIG_H_
#define ISM43362_CONFIG_H_

#include <config/board_config.h>

#define CONF_SPI_PIN_CS_PORT            CONF_ESWIFI_PIN_CS_PORT
#define CONF_SPI_PIN_CS_NR             (1 << CONF_ESWIFI_PIN_CS_NR)

#define CONF_SPI_PIN_WAKE_PORT          CONF_ESWIFI_PIN_WAKE_PORT
#define CONF_SPI_PIN_WAKE_NR           (1 << CONF_ESWIFI_PIN_WAKE_NR)

#define CONF_SPI_PIN_CMDDATA_PORT       CONF_ESWIFI_PIN_CMDDATA_PORT
#define CONF_SPI_PIN_CMDDATA_NR        (1 << CONF_ESWIFI_PIN_CMDDATA_NR)

#define CONF_SPI_PIN_RESET_PORT         CONF_ESWIFI_PIN_RESET_PORT
#define CONF_SPI_PIN_RESET_NR          (1 << CONF_ESWIFI_PIN_RESET_NR)

#define CONF_SPI_PIN_WIFI_LED_PORT      CONF_ESWIFI_PIN_WIFI_LED_PORT
#define CONF_SPI_PIN_WIFI_LED_NR       (1 << CONF_ESWIFI_PIN_WIFI_LED_NR)

#endif /* ISM43362_CONFIG_H_ */