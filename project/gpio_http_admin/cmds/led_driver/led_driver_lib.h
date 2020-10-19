/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    21.08.2019
 */

#ifndef LED_DRIVER_LIB_H_
#define LED_DRIVER_LIB_H_

extern int led_driver_init(void);
extern int led_driver_set(unsigned int nr);
extern int led_driver_clear(unsigned int nr);
extern int led_driver_get_states(unsigned char *states, unsigned int n);
extern int led_driver_set_states(const unsigned char *states,
	unsigned int n);
extern int led_driver_serialize(void);

#endif /* LED_DRIVER_LIB_H_ */
