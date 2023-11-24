#include <stdint.h>
#include <hal/reg.h>
#include <kernel/irq.h>

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

static int k1921vk035_gpio_int_setup(unsigned char port) {
	if (port == GPIO_PORT_A) {
		NVIC_EnableIRQ(GPIOA_IRQn);
	} else { // GPIO_PORT_B
		NVIC_EnableIRQ(GPIOB_IRQn);
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
	gpio_init_struct.Out = (mode & GPIO_MODE_OUT_SECTION) ? ENABLE : DISABLE;
	gpio_init_struct.AltFunc = (mode & GPIO_MODE_OUT_ALTERNATE) ? ENABLE : DISABLE;
	gpio_init_struct.OutMode = (mode & GPIO_MODE_OUT_OPEN_DRAIN) ? GPIO_OutMode_OD : GPIO_OutMode_PP;
	gpio_init_struct.InMode = (mode & GPIO_MODE_IN_SCHMITT) ? GPIO_InMode_Schmitt : GPIO_InMode_CMOS;
	gpio_init_struct.PullMode = (mode & GPIO_MODE_IN_PULL_UP) ? GPIO_PullMode_PU :
		(mode & GPIO_MODE_IN_PULL_DOWN) ? GPIO_PullMode_PD : GPIO_PullMode_Disable;
	if (port == GPIO_PORT_A) {
		gpio_init_struct.DriveMode = GPIO_DriveMode_LowFast;
	} else {
		gpio_init_struct.DriveMode = GPIO_DriveMode_HighFast;
	}
	gpio_init_struct.Digital = ENABLE;
	GPIO_Init(GPIO, &gpio_init_struct);

	k1921vk035_gpio_int_setup(port);

	GPIO_IntType_TypeDef GPIO_IntType =
		((mode & GPIO_MODE_INT_MODE_LEVEL0) || (mode & GPIO_MODE_INT_MODE_LEVEL1)
			? GPIO_IntType_Level : GPIO_IntType_Edge);
	GPIO_IntEdge_TypeDef GPIO_IntEdge =
		(((mode & GPIO_MODE_INT_MODE_RISING) && (mode & GPIO_MODE_INT_MODE_FALLING))
			? GPIO_IntEdge_Any : GPIO_IntEdge_Polarity);
	GPIO_IntPol_TypeDef GPIO_IntPol =
		((mode & GPIO_MODE_INT_MODE_RISING) || (mode & GPIO_MODE_INT_MODE_LEVEL1)
			? GPIO_IntPol_Positive : GPIO_IntPol_Negative);

	GPIO_ITPolConfig(GPIO, pins, GPIO_IntPol);
	GPIO_ITEdgeConfig(GPIO, pins, GPIO_IntEdge);
	GPIO_ITTypeConfig(GPIO, pins, GPIO_IntType);

	// XXX GPIO_MODE_IN_INT_DIS unused (default)
	GPIO_ITCmd(GPIO, pins, (mode & GPIO_MODE_IN_INT_EN) != 0);

	return 0;
}

static void k1921vk035_gpio_set(unsigned char port, gpio_mask_t pins, char level) {
	GPIO_TypeDef* GPIO = port == GPIO_PORT_A ? GPIOA : GPIOB;
	if (level) {
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

static irq_return_t gpio_irq_handler(unsigned int irq_nr, void *gpio_) {
	GPIO_TypeDef* gpio = (GPIO_TypeDef*)gpio_;
	uint32_t mask = gpio->INTSTATUS;
	// GPIO_PORT_A = 0; GPIO_PORT_B = 1
	gpio_handle_irq(&k1921vk035_gpio_chip, gpio == GPIOB, mask);
	GPIO_ITStatusClear(gpio, mask);
	return IRQ_HANDLED;
}

#define GPIOA_IRQn 3
#define GPIOB_IRQn 4

STATIC_IRQ_ATTACH(GPIOA_IRQn, gpio_irq_handler, GPIOA);
STATIC_IRQ_ATTACH(GPIOB_IRQn, gpio_irq_handler, GPIOB);
