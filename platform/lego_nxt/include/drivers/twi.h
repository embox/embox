/**
 * @file
 * @brief TWI communication for Lego AVR
 *
 * @date 15.10.2010
 * @author Anton Kozlov
 */
#ifndef NXT_TWI_H_
#define NXT_TWI_H_

#define NXT_AVR_ADDRESS 1
#define NXT_AVR_N_OUTPUTS 4
#define NXT_AVR_N_INPUTS  4

typedef struct {
	// Raw values
	uint8_t power;
	uint8_t pwm_frequency;
	int8_t output_percent[NXT_AVR_N_OUTPUTS];
	uint8_t output_mode;
	uint8_t input_power;
} __attribute__((packed)) to_avr;

typedef struct {
	// Raw values
	uint16_t adc_value[NXT_AVR_N_INPUTS];
	uint16_t buttons_val;
	uint16_t extra;
	uint8_t csum;
} __attribute__((packed)) from_avr;

extern void twi_init(void);

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

