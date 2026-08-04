/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#ifndef DRIVERS_PWM_SKELETON_PWM_PRIV_H_
#define DRIVERS_PWM_SKELETON_PWM_PRIV_H_

#include <stdint.h>

#include <drivers/pwm.h> /* only for struct pin_description  */

#define SKELETON_PWM_CHAN_MAX    (4)

struct skeleton_pwm_priv {
	const struct pin_description *pin_desc;
	const uintptr_t               base_addr;
	const char                   *clk_name;
	int                           idx;
	//const int                     channel;
};

#endif /* DRIVERS_PWM_SKELETON_PWM_PRIV_H_ */
