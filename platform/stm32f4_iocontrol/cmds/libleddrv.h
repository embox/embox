/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.11.2014
 */

#ifndef LIBLEDDRV_H_
#define LIBLEDDRV_H_

#define LEDDRV_LED_N 4

extern int leddrv_init(void);

extern int leddrv_set(unsigned int led_n);
extern int leddrv_clr(unsigned int led_n);

extern unsigned char *leddrv_getstates(void);
extern void leddrv_updatestates(void);

#endif /* LIBLEDDRV_H_ */
