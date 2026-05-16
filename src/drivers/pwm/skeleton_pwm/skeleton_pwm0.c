/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 15.05.2026
 */

#include <stddef.h>

#include <util/macro.h>



#include <drivers/pwm.h>
#include <drivers/pin_description.h>

#include <framework/mod/options.h>


#define PWM_DEV_ID                0
#define PWM_DEV_PRIV_STRUCT_NAME  MACRO_CONCAT(pwm_dev_priv, PWM_DEV_ID)

#define CONF_PWM                  MACRO_CONCAT(CONF_PWM,PWM_DEV_ID)

#define USE_BCONF        OPTION_GET(BOOLEAN, use_bconf)

#if USE_BCONF
#include <config/board_config.h>
#include <drivers/gpio.h>

#define PWM_BASE_ADDR  MACRO_CONCAT(CONF_PWM,_REGION_BASE_ADDR)
#define PWM_PORT       MACRO_CONCAT(CONF_PWM,_PIN_OUT_PORT)
#define PWM_PIN        MACRO_CONCAT(CONF_PWM,_PIN_OUT_NR)
#define PWM_FUNC       MACRO_CONCAT(CONF_PWM,_PIN_OUT_AF)
#define PWM_CLK_NAME   MACRO_CONCAT(CONF_PWM,_CLK_ENABLE)
#define PWM_CHANNEL_NR MACRO_CONCAT(CONF_PWM,_CHANNEL)

static const struct pin_description pwm_pin_desc = {
        .pd_port = PWM_PORT,
        .pd_pin = PWM_PIN,
        .pd_func = PWM_FUNC
};

#define PTR_PIN_DESC  (&pwm_pin_desc)

#else
#define PWM_BASE_ADDR     0
#define PWM_PORT          0
#define PWM_PIN           0
#define PWM_FUNC          0
#define PWM_CLK_NAME()   "PWM0_CLK"
#define PWM_CHANNEL_NR()  0
#define PTR_PIN_DESC      (NULL)
#endif

struct skeleton_pwm_priv {
	const struct pin_description *pin_desc;
	const uintptr_t               base_addr;
	const char                   *clk_name;
	const int                     channel;
};

extern struct pwm_ops skeleton_pwm_ops;

static struct skeleton_pwm_priv PWM_DEV_PRIV_STRUCT_NAME = {
    .pin_desc  = PTR_PIN_DESC,
    .base_addr = PWM_BASE_ADDR,
};

PWM_DEV_DEF(PWM_DEV_ID, &skeleton_pwm_ops, &PWM_DEV_PRIV_STRUCT_NAME,
                        PTR_PIN_DESC, PWM_BASE_ADDR, 1 << 0 /* 0 chan avail */);