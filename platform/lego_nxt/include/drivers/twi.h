/**
 * @file
 * @brief TWI communication for Lego AVR
 *
 * @date 15.10.2010
 * @author Anton Kozlov
 */
#ifndef NXT_TWI_H_
#define NXT_TWI_H_

#include <drivers/nxt_avr.h>

extern void twi_init(void);

enum twi_state_t {
	TWI_IDLE,
	TWI_READ,
	TWI_READ_BUT_LAST,
	TWI_WRITE,
	TWI_COMP
};

extern enum twi_state_t twi_state;

/**
 * Send through TWI, should be used to normal communication
 */
extern int twi_send(uint32_t dev_addr, const uint8_t *data, uint32_t count);

/**
 * Raw send through TWI, should be used in special cases,
 * e.g. first initializing, etc.
 */
extern int twi_write(uint32_t dev_addr, const uint8_t *data, uint32_t nBytes);

/**
 * Receive from TWI
 *
 * @retval 0 -- checksum is correct
 * @retval 1 -- checksum is incorrect
 */
extern int twi_receive(uint32_t dev_addr, uint8_t *data, uint32_t count);

#endif /* NXT_TWI_H_ */

