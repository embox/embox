#include <stdint.h>
#include <hal/reg.h>

#include <drivers/gpio/gpio_driver.h>

#include <framework/mod/options.h>
#include <K1921VK035.h>
#include <plib035.h>

#define GPIO_CHIP_ID OPTION_GET(NUMBER, gpio_chip_id)

static int k1921vk035_gpio_clock_setup(unsigned char port) {
	if (port == GPIO_PORT_A) {
		RCU_AHBClkCmd(RCU_AHBClk_GPIOA, ENABLE);
		RCU_AHBRstCmd(RCU_AHBRst_GPIOA, ENABLE);

	} else { // GPIO_PORT_B
		RCU_AHBClkCmd(RCU_AHBClk_GPIOB, ENABLE);
		RCU_AHBRstCmd(RCU_AHBRst_GPIOB, ENABLE);
	}
	return 0;
}

static int k1921vk035_gpio_setup_mode(unsigned char port, gpio_mask_t pins, int mode) {
	/* Enable port */
	k1921vk035_gpio_clock_setup(port);

	GPIO_TypeDef* GPIO = port == GPIO_PORT_A ? GPIOA : GPIOB;

	GPIO_Init_TypeDef gpio_init_struct;
	GPIO_StructInit(&gpio_init_struct);
	gpio_init_struct.Pin = pins;
	gpio_init_struct.Out = (mode & GPIO_MODE_OUT_SECTION) > 0 ? ENABLE : DISABLE;
	gpio_init_struct.AltFunc = (mode & GPIO_MODE_OUT_ALTERNATE) > 0 ? ENABLE : DISABLE;
	gpio_init_struct.OutMode = (mode & GPIO_MODE_OUT_OPEN_DRAIN) > 0 ? GPIO_OutMode_OD : GPIO_OutMode_PP;
	gpio_init_struct.InMode = (mode & GPIO_MODE_IN_SCHMITT) > 0 ? GPIO_InMode_Schmitt : GPIO_InMode_CMOS;
	gpio_init_struct.PullMode = (mode & GPIO_MODE_IN_PULL_UP) > 0 ? GPIO_PullMode_PU :
		(mode & GPIO_MODE_IN_PULL_DOWN) > 0 ? GPIO_PullMode_PD : GPIO_PullMode_Disable;
	gpio_init_struct.DriveMode = GPIO_DriveMode_HighFast;
	gpio_init_struct.Digital = ENABLE;
	GPIO_Init(GPIO, &gpio_init_struct);
	return 0;
}

static void k1921vk035_gpio_set(unsigned char port, gpio_mask_t pins, char level) {
	GPIO_TypeDef* GPIO = port == GPIO_PORT_A ? GPIOA : GPIOB;
	if (level){
		GPIO_SetBits(GPIO, pins);
	} else {
		GPIO_ClearBits(GPIO, pins);
	}
}

static gpio_mask_t k1921vk035_gpio_get(unsigned char port, gpio_mask_t pins) {
	GPIO_TypeDef* GPIO = port == GPIO_PORT_A ? GPIOA : GPIOB;
	return GPIO_ReadPort(GPIO) & pins;
}

static struct gpio_chip k1921vk035_gpio_chip = {
	.setup_mode = k1921vk035_gpio_setup_mode,
	.get = k1921vk035_gpio_get,
	.set = k1921vk035_gpio_set,
	.nports = 2};

GPIO_CHIP_DEF(&k1921vk035_gpio_chip);
