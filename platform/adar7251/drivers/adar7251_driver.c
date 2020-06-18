
#include <util/log.h>

#include <unistd.h>

#include <drivers/spi.h>
#include <drivers/gpio/gpio.h>

#include <drivers/adar7251_driver.h>
#include "adar7251.h"

#include <drivers/nucleo_f429zi_audio.h>

#define SPI_BUS_NUM      1

#define ADC_START_PORT   GPIO_PORT_B
#define ADC_START_PIN    (1 << 4)

#define CS_PORT          GPIO_PORT_A
#define CS_PIN			 (1 << 4)

#define RESET_PORT       GPIO_PORT_C
#define RESET_PIN        (1 << 0)

struct preset {
	uint16_t addr;
	uint16_t value;
};

static const struct preset preset_pll[] = {
		/* pairs: 16 bit register addres, then 16 bit data to write */

		/* set 20 * 5.76 = 112.5 MHz
		 *  16 MHz crystal => den 10, num 2, int div 7
		 *  20 MHz crystal => den 100, num 76, int div 5
		 **/
		//20 MHz
		{ PLL_DEN, 100 },
		{PLL_NUM, 76},

		{PLL_CTRL,
				PLL_TYPE_FRAKT | PLL_EN |\
				((1u << PLL_INPUT_PRESC_SHIFT) & PLL_INPUT_PRESC_MASK )|\
				((5u << PLL_INTEGER_DIV_SHIFT) & PLL_INTEGER_DIV_MASK )},

		//19.2 MHz
		 /*{PLL_CTRL,
				PLL_TYPE_INT | PLL_EN |\
				((1u << PLL_INPUT_PRESC_SHIFT) & PLL_INPUT_PRESC_MASK )|\
				((6u << PLL_INTEGER_DIV_SHIFT) & PLL_INTEGER_DIV_MASK )},*/

		{CLK_CTRL, CLK_USE_PLL}
};

static const struct preset preset[] = {
		{ ADC_ROUTING1_4,
				(LNA_PGA_EQ_BP << ADC1_OFFSET) |\
				(LNA_PGA_EQ_BP << ADC2_OFFSET) |\
				(LNA_PGA_EQ_BP << ADC3_OFFSET) |\
				(LNA_PGA_EQ_BP << ADC4_OFFSET) },

		{ DECIM_RATE, RATE_300K },

// TODO: HIGH_PASS
// TODO: DEJITTER


		{ OUTPUT_MODE, OUT_MODE_SERIAL | CONV_START_EN },

		{SERIAL_MODE, TDM_MODE_4PF | DATA_LJF | BCLK_POL_NEG | LRCLK_POL_NEG |\
             LRCLK_MODE_50X50 | SCLK_MASTER },

		{ SCLK_ADC_PIN, DRIVE_HIGHEST | PIN_PULLDOWN_EN },
		{ ADC_DOUT0_PIN, DRIVE_HIGHEST | PIN_PULLDOWN_EN },
		{ ADC_DOUT1_PIN, DRIVE_HIGHEST | PIN_PULLDOWN_EN },
		{ FS_ADC_PIN, DRIVE_HIGHEST | PIN_PULLDOWN_EN },

		{ ASIL_CLEAR, ASIL_CLEAR_ERR},

		{ ADC_ENABLE, ADC1_EN | ADC2_EN | ADC3_EN | ADC4_EN | LN_PG1_EN |\
			LN_PG2_EN | LN_PG3_EN | LN_PG4_EN },

		{ MASTER_ENABLE, MASTER_EN }
};


static void adar_ctrl_port_init(struct adar7251_dev *adar7251_dev) {
	struct spi_device *dev = adar7251_dev->spi_dev;
	uint8_t buf_in, buf_out;

	log_debug("in ADARinitControlPort");

	sleep(1);
	/* Chip must be deselected */
	gpio_set(CS_PORT, CS_PIN, 1);
	spi_set_master_mode(dev);

	/* Setup the spi for 8 bit data, high steady state clock,
	   second edge capture, with a 1MHz clock rate

	   spi.format(8,3);
	   spi.frequency(1000000);
	   */
   /* supply reset */
	gpio_set(RESET_PORT, RESET_PIN, 0);
	sleep(1);
	gpio_set(RESET_PORT, RESET_PIN, 1);
	sleep(1);

	/* let's disable CRC control */
	gpio_set(CS_PORT, CS_PIN, 0);

	buf_out = ADAR0_W;
	spi_transfer(dev, &buf_out, &buf_in, 1); /* write command */
	buf_out = UpperByte(CRC_EN);
	spi_transfer(dev, &buf_out, &buf_in, 1);
	buf_out = LowerByte(CRC_EN);
	spi_transfer(dev, &buf_out, &buf_in, 1);
	buf_out = UpperByte(CRC_DISABLE);
	spi_transfer(dev, &buf_out, &buf_in, 1);
	buf_out = LowerByte(CRC_DISABLE);
	spi_transfer(dev, &buf_out, &buf_in, 1);

	/* + that time with CRC */
	buf_out = UpperByte(CRC_DISABLE_CRC);
	spi_transfer(dev, &buf_out, &buf_in, 1);
	buf_out = LowerByte(CRC_DISABLE_CRC);
	spi_transfer(dev, &buf_out, &buf_in, 1);

	gpio_set(CS_PORT, CS_PIN, 1);
}

int adar7251_hw_init(struct adar7251_dev *dev) {

	dev->sai_dev = sai_init();

	gpio_setup_mode(ADC_START_PORT, ADC_START_PIN, GPIO_MODE_OUTPUT);
	gpio_setup_mode(CS_PORT, CS_PIN, GPIO_MODE_OUTPUT);
	gpio_setup_mode(RESET_PORT, RESET_PIN, GPIO_MODE_OUTPUT);

	dev->spi_bus = SPI_BUS_NUM;

	dev->spi_dev = spi_dev_by_id(dev->spi_bus);
	if (!dev->spi_dev) {
		return -1;
	}

	dev->spi_dev->flags = 0;

	gpio_set(ADC_START_PORT, ADC_START_PIN, GPIO_PIN_HIGH);

	adar_ctrl_port_init(dev);
    sleep(1);
	adar_scan_flash(dev);
	sleep(1);
	adar_preset_pll(dev);
	sleep(1);
	adar_preset(dev);
	sleep(1);

	gpio_set(ADC_START_PORT, ADC_START_PIN, GPIO_PIN_LOW);

	return 0;
}

/* write individual register without readback check */
void adar_set_reg(struct spi_device *dev, uint16_t addr, uint16_t value) {
	uint8_t buf_in, buf_out;

	gpio_set(CS_PORT, CS_PIN, 0);

	buf_out = ADAR0_W;
	spi_transfer(dev, &buf_out, &buf_in, 1); /* write command */
	buf_out = UpperByte(addr);
	spi_transfer(dev, &buf_out, &buf_in, 1);
	buf_out = LowerByte(addr);
	spi_transfer(dev, &buf_out, &buf_in, 1);
	buf_out = UpperByte(value);
	spi_transfer(dev, &buf_out, &buf_in, 1);
	buf_out = LowerByte(value);
	spi_transfer(dev, &buf_out, &buf_in, 1);

	gpio_set(CS_PORT, CS_PIN, 1);
}


/* read individual register */
uint16_t adar_get_reg(struct spi_device *dev, uint16_t addr) {
	uint16_t reg = 0;
	uint8_t buf_in, buf_out;

	gpio_set(CS_PORT, CS_PIN, 0);

	buf_out = ADAR0_R;
	spi_transfer(dev, &buf_out, &buf_in, 1); /* write command */
	buf_out = UpperByte(addr);
	spi_transfer(dev, &buf_out, &buf_in, 1);
	buf_out = LowerByte(addr);
	spi_transfer(dev, &buf_out, &buf_in, 1);
	buf_out = 0xFF;
	spi_transfer(dev, &buf_out, &buf_in, 1);
	reg = (uint16_t)(buf_in) << 8;
	buf_out = 0xFF;
	spi_transfer(dev, &buf_out, &buf_in, 1);
	reg |= (uint16_t)(buf_in);

	gpio_set(CS_PORT, CS_PIN, 1);

	return reg;
}

/* write down batch of registers */
void adar_preset_pll(struct adar7251_dev *dev) {
	int i;

	for(i = 0; i < (sizeof(preset_pll) / sizeof(preset_pll[0])); i ++) {
			adar_set_reg(dev->spi_dev, preset_pll[i].addr, preset_pll[i].value);
			log_debug("REG: 0x%04X, DATA write 0x%04X, read 0x%04X",
					preset_pll[i].addr, preset_pll[i].value,
					adar_get_reg(dev->spi_dev, preset_pll[i].addr));
	}
}

/* write down batch of registers */
void adar_preset(struct adar7251_dev *dev) {
	int i;
	for(i = 0; i < sizeof(preset) / sizeof(preset[0]); i ++) {
		adar_set_reg(dev->spi_dev, preset[i].addr, preset[i].value);
		log_debug("REG: 0x%04X, DATA write 0x%04X, read 0x%04X",
				 preset[i].addr, preset[i].value,
				 adar_get_reg(dev->spi_dev, preset[i].addr));
	}
}


void adar_scan_flash(struct adar7251_dev *dev) {
	int i;
	int r[] = {
		0x0000, 0x0001, 0x0002, 0x0003,
		0x0005, 0x0040, 0x0041, 0x0042,
		0x0080, 0x0081, 0x0082, 0x0083,
		0x0084, 0x0084, 0x0085, 0x0086,
		0x0087, 0x0088, 0x0089, 0x0100,
		0x0101, 0x0102, 0x0140, 0x0141,
		0x0143, 0x0144, 0x01C0, 0x01C1,
		0x01C2, 0x0200, 0x0201, 0x0210,
		0x0211, 0x0250, 0x0251, 0x0260,
		0x0261, 0x0270, 0x0280, 0x0281,
		0x0282, 0x0283, 0x0284, 0x0285,
		0x0286, 0x0287, 0x0288, 0x0289,

		0x028A, 0x028B, 0x028C, 0x028D,	
		0x028E, 0x0291, 0x0292, 0x0301,
		0x0308, 0x030A, 0x030E, 0xFD00			
		};

	for(i = 0; i < (sizeof(r) / sizeof(r[0])); i ++) {
		log_debug("REG: 0x%04X, read 0x%04X",r[i] , adar_get_reg(dev->spi_dev, r[i]));
	}
}

