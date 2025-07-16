/**
 * @file
 * @brief TWI communication for Lego AVR
 *
 * @date 15.10.10
 * @author Anton Kozlov
 */

#ifndef NXT_TWI_H_
#define NXT_TWI_H_

#include <stdint.h>

/**
 * Send through TWI, should be used to normal communication
 */
extern void twi_send(uint32_t dev_addr, const uint8_t *data, uint32_t count);

/**
 * Raw send through TWI, should be used in special cases,
 * e.g. first initializing, etc.
 */
extern void twi_write(uint32_t dev_addr, const uint8_t *data, uint32_t nBytes);

/**
 * Receive from TWI
 *
 * @retval 0 -- checksum is correct
 * @retval 1 -- checksum is incorrect
 */
extern int twi_receive(uint32_t dev_addr, uint8_t *data, uint32_t count);

#endif /* NXT_TWI_H_ */
