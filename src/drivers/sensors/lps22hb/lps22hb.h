/**
 * @file
 * @brief
 *
 * @date 21.04.2023
 * @author Andrew Bursian
 */

#ifndef LPS22HB_H_
#define LPS22HB_H_

#include <stdint.h>

extern int lps22hb_init(void);
extern int32_t lps22hb_get_pressure_4096(void);
extern int16_t lps22hb_get_temp_x100(void);

#endif /* LPS22HB_H_ */
