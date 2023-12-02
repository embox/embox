/*
 *
 * @date 20.01.15
 * @author Alexander Kalmuk
 */

#ifndef LIB_CRC16_H_
#define LIB_CRC16_H_

#include <stddef.h>
#include <stdint.h>

extern uint16_t crc16(uint16_t crc, uint8_t const *buffer, size_t len);

#endif /* LIB_CRC16_H_ */

