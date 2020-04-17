/**
 * @file
 * @brief
 *
 * @date 26.04.20
 * @author Alexander Kalmuk
 */

#ifndef DRIVERS_INPUT_TOUCHSCREEN_TOUCHSCREEN_H_
#define DRIVERS_INPUT_TOUCHSCREEN_TOUCHSCREEN_H_

/* Touchscreen common event types */

#define TS_TOUCH_1              1
#define TS_TOUCH_2              2
#define TS_TOUCH_1_RELEASED     3
#define TS_TOUCH_2_RELEASED     4
#define TS_TOUCH_PRESSURE       5

/* TODO Should we move it to the common part input_dev.h? */
#define TS_EVENT_NEXT           (1 << 31)

#endif /* DRIVERS_INPUT_TOUCHSCREEN_TOUCHSCREEN_H_ */
