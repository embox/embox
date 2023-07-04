/**
 * @brief Spi driver for MCU k1921vk035
 * @author kenny5660(Liamaev Mikhail)
 * @date 2023.07.04
 */
#include <errno.h>

#include <drivers/common/memory.h>
#include <drivers/spi.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <util/log.h>
#include <assert.h>
#include <drivers/gpio/gpio.h>
#include <plib035_spi.h>
#include <plib035_rcu.h>


#define K1921VK035_SPI_DEFAULT_FREQ_DIV OPTION_GET(NUMBER,default_freq_div)

#define K1921VK035_SPI_CS_UNDEFINED 255
#define K1921VK035_SPI_MAX_CS 5
#define K1921VK035_SPI_CS_MAP_PORT 0
#define K1921VK035_SPI_CS_MAP_PIN 1
typedef struct  {
	int cs; /* current CS */
	uint8_t cs_map[K1921VK035_SPI_MAX_CS][2];
}k1921vk035_spi_dev_t;

k1921vk035_spi_dev_t k1921vk035_spi0_dev = {.cs = 0,
											.cs_map = {{OPTION_GET(NUMBER, port_cs0), OPTION_GET(NUMBER, pin_cs0)},
													   {OPTION_GET(NUMBER, port_cs1), OPTION_GET(NUMBER, pin_cs1)},
													   {OPTION_GET(NUMBER, port_cs2), OPTION_GET(NUMBER, pin_cs2)},
													   {OPTION_GET(NUMBER, port_cs3), OPTION_GET(NUMBER, pin_cs3)},
													   {OPTION_GET(NUMBER, port_cs4), OPTION_GET(NUMBER, pin_cs4)}}};

void k1921vk035_spi_config(struct spi_device *dev){
	int clk_div;

	SPI_Cmd(DISABLE);
	SPI_DataWidthConfig(SPI_DataWidth_8);
	SPI_FrameFormatConfig(SPI_FrameFormat_SPI);
	SPI_ModeConfig(SPI_Mode_Master);
	SPI_SCKPhase_TypeDef SPI_SCKPhase =
		(dev->flags & SPI_CS_MODE(SPI_MODE_1)) > 0 ? SPI_SCKPhase_CaptureFall : SPI_SCKPhase_CaptureRise;
	SPI_SCKPolarity_TypeDef SPI_SCKPolarity =
		(dev->flags & SPI_CS_MODE(SPI_MODE_2)) > 0 ? SPI_SCKPolarity_SteadyHigh : SPI_SCKPolarity_SteadyLow;
	SPI_SCKConfig(SPI_SCKPhase, SPI_SCKPolarity);

	clk_div = (dev->flags >> 16) & 0xFFFF;
	if (clk_div == 0){
		clk_div = K1921VK035_SPI_DEFAULT_FREQ_DIV;
	}
	assert(clk_div > 1 && clk_div <= 512);
	if (clk_div > 1) {
		SPI_SCKDivConfig((clk_div / 2) - 1,2);
	}

	SPI_Cmd(ENABLE);


}
static int k1921vk035_spi_init(struct spi_device *dev) {
	//Init SPI_TX - PB7, SPI_RX - PB6 , SPI_SCK - PB5
	k1921vk035_spi_dev_t* k1921vk035_spi_dev = dev->priv;
	int pin = 0;
	int port = 0;

	pin = k1921vk035_spi_dev->cs_map[k1921vk035_spi_dev->cs][K1921VK035_SPI_CS_MAP_PIN];
	port = k1921vk035_spi_dev->cs_map[k1921vk035_spi_dev->cs][K1921VK035_SPI_CS_MAP_PORT];

	gpio_setup_mode(GPIO_PORT_B, ( 1 << 5 ) | ( 1 << 6 ) | ( 1 << 7 ), GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(0));

	for (int i = 0; i < K1921VK035_SPI_MAX_CS; i++) {
		if (port != K1921VK035_SPI_CS_UNDEFINED && pin != K1921VK035_SPI_CS_UNDEFINED ) {
		gpio_setup_mode(port, 1 << pin, GPIO_MODE_OUT_PUSH_PULL);
		gpio_set(port, 1 << pin, GPIO_PIN_HIGH);
		}
	}

	RCU_SPIClkConfig(RCU_PeriphClk_PLLClk, 0, DISABLE);
	RCU_SPIRstCmd(ENABLE);
	RCU_SPIClkCmd(ENABLE);
	dev->flags = SPI_CS_DIVSOR(K1921VK035_SPI_DEFAULT_FREQ_DIV);

	k1921vk035_spi_config(dev);

	return 0;
}

static void k1921vk035_spi_set_cs(const k1921vk035_spi_dev_t* k1921vk035_spi_dev, int state) {
	int pin = 0;
	int port = 0;

	assert(k1921vk035_spi_dev->cs < K1921VK035_SPI_MAX_CS);

	pin = k1921vk035_spi_dev->cs_map[k1921vk035_spi_dev->cs][K1921VK035_SPI_CS_MAP_PIN];
	port = k1921vk035_spi_dev->cs_map[k1921vk035_spi_dev->cs][K1921VK035_SPI_CS_MAP_PORT];
	
	assert(pin != K1921VK035_SPI_CS_UNDEFINED  && pin != K1921VK035_SPI_CS_UNDEFINED);
	
	gpio_set(port, 1 << pin, state);
}

static int k1921vk035_spi_select(struct spi_device *dev, int cs) {
	k1921vk035_spi_dev_t* k1921vk035_spi_dev = dev->priv;
	int res = 0;
	int pin = 0;
	int port = 0;

	pin = k1921vk035_spi_dev->cs_map[k1921vk035_spi_dev->cs][K1921VK035_SPI_CS_MAP_PIN];
	port = k1921vk035_spi_dev->cs_map[k1921vk035_spi_dev->cs][K1921VK035_SPI_CS_MAP_PORT];

	if (cs < 0 || cs > 4) {
		log_error("Only cs=0..4 are available!");
		return -EINVAL;
	}
	if(pin == K1921VK035_SPI_CS_UNDEFINED && port == K1921VK035_SPI_CS_UNDEFINED){
		log_error("cs=%d, not configured. port_cs=%d, pin_cs=%d", 
				  cs, port, pin);
		return -EINVAL;
	}
	k1921vk035_spi_dev->cs = cs;
	k1921vk035_spi_config(dev);
	return res;
}

static uint8_t k1921vk035_spi_transfer_byte(const k1921vk035_spi_dev_t* k1921vk035_spi_dev, uint8_t val) {
	SPI_SendData(val);
	while (SPI_FlagStatus(SPI_Flag_Busy)) {}
	return SPI_RecieveData();
}

static int k1921vk035_spi_transfer(struct spi_device *dev, uint8_t *inbuf,
		uint8_t *outbuf, int count) {
	k1921vk035_spi_dev_t* k1921vk035_spi_dev = dev->priv;
	uint8_t val;

	if (dev->flags & SPI_CS_ACTIVE) {
		k1921vk035_spi_set_cs(k1921vk035_spi_dev, 0);
	}

	while (count--) {
		val = inbuf ? *inbuf++ : 0;
		log_debug("tx %02x", val);
		val = k1921vk035_spi_transfer_byte(k1921vk035_spi_dev, val);

		log_debug("rx %02x", val);
		if (outbuf)
			*outbuf++ = val;
	}

	if (dev->flags & SPI_CS_INACTIVE) {
		k1921vk035_spi_set_cs(k1921vk035_spi_dev, 1);
	}

	return 0;
}

struct spi_ops k1921vk035_spi_ops = {
	.init     = k1921vk035_spi_init,
	.select   = k1921vk035_spi_select,
	.transfer = k1921vk035_spi_transfer
};


SPI_DEV_DEF("spi0", &k1921vk035_spi_ops, &k1921vk035_spi0_dev, 0);
