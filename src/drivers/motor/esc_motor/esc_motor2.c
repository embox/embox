/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 31.08.2026
 */

 #include <drivers/pwm.h>

#include <drivers/motor.h>

#include <framework/mod/options.h>

#define MOTOR_ID        2

#define ESC_PWM_ID     OPTION_GET(NUMBER, pwm_id)
#define ESC_PWM_CHAN   OPTION_GET(NUMBER, pwm_chan)

extern const struct motor_ops esc_motor_ops;

MOTOR_DEV_DEF(MOTOR_ID, &esc_motor_ops, NULL, ESC_PWM_ID, ESC_PWM_CHAN);