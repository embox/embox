/**
 * @file
 *
 * @author Efim Perevalov
 * @date 8.12.2025
 */
#include <drivers/irqctrl.h>

int irqctrl_get_intid(void) {
	return 0;
}

int irqctrl_set_level(unsigned int irq, int level) {
	return 0;
}

static int matrixint_init(void) {
	return 0;
}

IRQCTRL_DEF(matrixint, matrixint_init);
