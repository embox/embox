#ifndef SPL06_H_
#define SPL06_H_

#include <stdint.h>

extern int spl06_init(void);
extern int32_t spl06_get_temperature(void);
extern int32_t spl06_get_humidity(void);
extern int32_t spl06_get_pressure(void);

#endif /* SPL06_H_ */
