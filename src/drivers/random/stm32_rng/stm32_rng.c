/**
 * @file
 *
 * @date Apr 28, 2021
 * @author Anton Bondarev
 */
#include <stdlib.h>

#include <util/log.h>

#include <drivers/rng/stm32cube_rng.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(stm32_rng_init);

static RNG_HandleTypeDef            hRNG;

extern uint32_t trng_read(void);
static int stm32_rng_init(void) {
	static int inited = 0;

	if (inited) {
		return 0;
	}

	inited++;

	hRNG.Instance = RNG;
	__HAL_RCC_RNG_CLK_ENABLE();
	HAL_RNG_Init(&hRNG);

	srand(trng_read());
	return 0;
}

uint32_t trng_read(void) {
	uint32_t value;

	HAL_RNG_GenerateRandomNumber(&hRNG, &value);

	return value;
}
