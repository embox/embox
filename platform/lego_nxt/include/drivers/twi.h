/**
 * @file
 * @brief TWI communication for Lego AVR
 *
 * @date 15.10.2010
 * @author Anton Kozlov
 */

#define NXT_AVR_ADDRESS 1
#define NXT_AVR_N_OUTPUTS 4
#define NXT_AVR_N_INPUTS  4

typedef struct{
  // Raw values
	uint8_t power;
	uint8_t pwm_frequency;
	int8_t output_percent[NXT_AVR_N_OUTPUTS];
	uint8_t output_mode;
	uint8_t input_power;
} __attribute__((packed)) to_avr;

void twi_init(void);

/**
 * Send through TWI, should be used to normal communication
 */
void twi_send(uint32_t dev_addr, const uint8_t *data, uint32_t count);

/**
 * Raw send through TWI, should be used in special cases,
 * e.g. first initializing, etc.
 */
void twi_write(uint32_t dev_addr, const uint8_t *data, uint32_t nBytes);
