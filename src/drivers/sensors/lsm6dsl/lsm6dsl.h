/**
 * @file
 * @brief
 *
 * @date 21.04.2023
 * @author Andrew Bursian
 */

#ifndef LSM6DSL_H_
#define LSM6DSL_H_

#include <stdint.h>

struct lsm6dsl_s {int16_t g_x, g_y, g_z, xl_x, xl_y, xl_z;};

extern int lsm6dsl_init(void);
extern void lsm6dsl_get_data(struct lsm6dsl_s *data);
extern int16_t lsm6dsl_get_temp_x256(void);
extern int32_t lsm6dsl_get_timestamp_us(void);

#endif /* LSM6DSL_H_ */
