/**
 * @file
 * @brief
 *
 * @date 21.04.2023
 * @author Andrew Bursian
 */

#ifndef LIS3MDL_H_
#define LIS3MDL_H_

#include <stdint.h>

struct lis3mdl_s {int16_t x,y,z,sens;};

extern int lis3mdl_init(void);
extern void lis3mdl_get_data(struct lis3mdl_s *data);
extern int16_t lis3mdl_get_temp_x8(void);

#endif /* LIS3MDL_H_ */
