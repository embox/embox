/**
 * @file
 * @brief
 *
 * @date 21.04.2023
 * @author Andrew Bursian
 */

#ifndef HTS221_H_
#define HTS221_H_

#include <stdint.h>

extern int hts221_init(void);
extern int16_t hts221_get_humidity(void);
extern int16_t hts221_get_temp_x10(void);

#endif /* HTS221_H_ */
