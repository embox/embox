/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 20.06.2025
 */

 #include <stddef.h>

#include <util/macro.h>

#include <drivers/spi.h>

#include <framework/mod/options.h>

#define SPI_BUS_NUM        0
#define SPI_DEV_NUM        0

#define BCONF_USE         OPTION_GET(BOOLEAN, bconf_use)

#if BCONF_USE

#include <config/board_config.h>

#define SPI_DEV_NAME       MACRO_CONCAT(MACRO_CONCAT(SPI,SPI_BUS_NUM),_0)

#define CS_USE   1

#else /* !BCONF_USE */

#define SPI_DEV_NAME       OPTION_GET(STRING, dev_name)

#define CS_PORT            OPTION_GET(NUMBER, cs_port)
#define CS_PIN             OPTION_GET(NUMBER, cs_pin)

#if OPTION_GET(NUMBER, cs_func) == 0xFF
#define CS_FUNC     -1
#else
#define CS_FUNC            OPTION_GET(NUMBER, cs_func)
#endif

#define CS_PULLUP          OPTION_GET(NUMBER, cs_pullup)
#define CS_PUSHPULL        OPTION_GET(NUMBER, cs_pushpull)

#if CS_PORT != 0xFF

#define CS_USE     1

#endif /* CS_PORT != 0xFF */

#endif /* BCONF_USE */

#if defined(CS_USE)

#include <drivers/gpio.h>

static const struct pin_description spi_cs_pin = {
	CS_PORT, CS_PIN, CS_FUNC
};

#define CS_PIN_DESC_PTR    (&spi_cs_pin)

#else /* !defined(CS_USE) */

#define CS_PIN_DESC_PTR    NULL

#endif /* defined(CS_USE) */

static int spi_dev0_probe(struct spi_device *dev) {
#if defined(CS_USE)
    uint32_t flags = GPIO_MODE_OUT;
    
#if CS_PULLUP
    flags |= GPIO_MODE_IN_PULL_UP;
#endif
#if CS_PUSHPULL
    flags |= GPIO_MODE_OUT_PUSH_PULL;
#endif
#if CS_FUNC != -1
    flags |= GPIO_MODE_ALT_SET(CS_FUNC);
#endif  /* CS_FUNC == -1 */

    gpio_setup_mode(CS_PIN_DESC_PTR->pd_port,
		(1 << CS_PIN_DESC_PTR->pd_pin),
		flags);
#endif /* defined(CS_USE) */

    return 0;
}

static const struct spid_ops spid_ops = {
    .init = spi_dev0_probe,
};

SPI_DEV_DEF(SPI_DEV_NAME, &spid_ops, NULL, SPI_BUS_NUM, SPI_DEV_NUM, CS_PIN_DESC_PTR);
