/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#ifndef DRIVERS_PWM_NIIET_NIIET_PWM_PRIV_H_
#define DRIVERS_PWM_NIIET_NIIET_PWM_PRIV_H_

#include <stdint.h>

#include <drivers/pwm.h> /* only for struct pin_description  */

#define NIIET_PWM_CHAN_MAX    (4)

struct niiet_pwm_priv {
	const struct pin_description *pin_desc;
	const uintptr_t               base_addr;
	const char                   *clk_name;
	int                           clk_div;
	uint32_t                      comp_mask;
	int                           idx;
};

#endif /* DRIVERS_PWM_NIIET_NIIET_PWM_PRIV_H_ */
