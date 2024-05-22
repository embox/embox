/**
 * @file
 *
 * @date 06.02.2020
 * @author Alexander Kalmuk
 */

#ifndef L3G4200D_H_
#define L3G4200D_H_

#include <stdint.h>

extern int l3g4200d_init(void);
extern int16_t l3g4200d_get_angular_rate_x(void);
extern int16_t l3g4200d_get_angular_rate_y(void);
extern int16_t l3g4200d_get_angular_rate_z(void);

#endif /* L3G4200D_H_ */
