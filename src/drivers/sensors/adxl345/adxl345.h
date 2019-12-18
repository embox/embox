/**
 * @file adxl345.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 18.12.2019
 */
#ifndef ADXL345_H
#define ADXL345_H

extern int adxl345_init(void);
extern int16_t adxl345_get_x(void);
extern int16_t adxl345_get_y(void);
extern int16_t adxl345_get_z(void);

#endif /* ADXL345_H */
