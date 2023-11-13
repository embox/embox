/**
 * @file
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-04-04
 */
#ifndef _FEATHER_LIBLEDS_H_
#define _FEATHER_LIBLEDS_H_

extern void libleds_init(void);
extern void libleds_led_on(int led_num);
extern void libleds_led_off(int led_num);
extern int libleds_leds_quantity(void);

#endif /* _FEATHER_LIBLEDS_H_ */