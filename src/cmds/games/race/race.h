/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    08.04.2015
 */

#ifndef CMD_RACE_H_
#define CMD_RACE_H_

#define RACE_ROAD_LEN 16
#define RACE_ROAD_WDT 2

#define RACE_LVLUP_MS 5
#define RACE_ROAD_UPD_MS 150
#define RACE_CAR_UPD_MS 130

#define RACE_OBSTACLE_STEP 4
#define RACE_LVL_STEP 15

#define RACE_CAR '>'
#define RACE_OBSTACLE 'X'
#define RACE_SPACE ' '

extern void race_print_road(const char *);
extern void race_print_score(int);
extern int race_is_car_moved(void);

#endif /* CMD_RACE_H_ */
