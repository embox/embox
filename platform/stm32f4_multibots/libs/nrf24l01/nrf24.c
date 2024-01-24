/*
* ----------------------------------------------------------------------------
* “THE COFFEEWARE LICENSE” (Revision 1):
* <ihsan@kehribar.me> wrote this file. As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a coffee in return.
* -----------------------------------------------------------------------------
* This library is based on this library: 
*   https://github.com/aaronds/arduino-nrf24l01
* Which is based on this library: 
*   http://www.tinkerer.eu/AVRLib/nRF24L01
* -----------------------------------------------------------------------------
*/

/**
 * @file
 * @brief  Adaptation for STM32 Cube, add IRQ
 *
 * @date   27.06.18
 * @author Alexander Kalmuk
 */

#include "nrf24.h"
#include "nrf24_stm32_cube.h"

#include <lib/libds/ring_buff.h>
#include <hal/ipl.h>
#include <util/log.h>

#define RX_BUF_SZ OPTION_GET(NUMBER,rx_buf_sz)

#define NRF24_DO_IPL_LOCKED(job)      \
	ipl = ipl_save();                 \
	{                                 \
		nrf24_csn_digitalWrite(LOW);  \
		do {                          \
			job                       \
		} while (0);                  \
		nrf24_csn_digitalWrite(HIGH); \
	}                                 \
	ipl_restore(ipl);

struct nrf24 {
	uint8_t rx_storage[RX_BUF_SZ];
	struct ring_buff rx_buf;
	int volatile last_tx_status;
	uint8_t payload_len;
};

/* TODO Allocate this structure using pool or malloc */
static struct nrf24 nrf24;

/* init the hardware pins */
void nrf24_init() {
	ring_buff_init(&nrf24.rx_buf, sizeof(uint8_t),
					RX_BUF_SZ, nrf24.rx_storage);
	nrf24_hw_init();
	nrf24_ce_digitalWrite(LOW);
	nrf24_csn_digitalWrite(HIGH);
}

static bool nrf24_is_connected(void) {
	uint8_t values[3] = {0xB7, 0xC5, 0xA2};
	uint8_t in_buf[5];
	uint8_t out_buf[5];
	int i;

	/* Write and then read different values */
	for (i = 0; i < 3; i++) {
		int j;

		for (j = 0; j < 5; j++) {
			in_buf[j] = values[i];
		}

		nrf24_writeRegister(RX_ADDR_P0, in_buf, 5);
		nrf24_readRegister(RX_ADDR_P0, out_buf, 5);

		for (j = 0; j < 5; j++) {
			if (out_buf[j] != values[i]) {
				return false;
			}
		}
	}

	return true;
}

/* configure the module */
int nrf24_config(uint8_t channel, uint8_t pay_length) {
	/* Use static payload length ... */
	nrf24.payload_len = pay_length;

	// Set RF channel
	nrf24_configRegister(RF_CH,channel);

	// Set length of incoming payload 
	nrf24_configRegister(RX_PW_P0, 0x00); // Auto-ACK pipe ...
	nrf24_configRegister(RX_PW_P1, nrf24.payload_len); // Data payload pipe
	nrf24_configRegister(RX_PW_P2, 0x00); // Pipe not used 
	nrf24_configRegister(RX_PW_P3, 0x00); // Pipe not used 
	nrf24_configRegister(RX_PW_P4, 0x00); // Pipe not used 
	nrf24_configRegister(RX_PW_P5, 0x00); // Pipe not used 

	// 1 Mbps, TX gain: 0dbm
	nrf24_configRegister(RF_SETUP, (0<<RF_DR)|((0x03)<<RF_PWR));

	// CRC enable, 1 byte CRC length
	nrf24_configRegister(CONFIG,nrf24_CONFIG);

	// Auto Acknowledgment
	nrf24_configRegister(EN_AA,(1<<ENAA_P0)|(1<<ENAA_P1)|(0<<ENAA_P2)|(0<<ENAA_P3)|(0<<ENAA_P4)|(0<<ENAA_P5));

	// Enable RX addresses
	nrf24_configRegister(EN_RXADDR,(1<<ERX_P0)|(1<<ERX_P1)|(0<<ERX_P2)|(0<<ERX_P3)|(0<<ERX_P4)|(0<<ERX_P5));

	// Auto retransmit delay: 1000 us and Up to 15 retransmit trials
	nrf24_configRegister(SETUP_RETR,(0x04<<ARD)|(0x0F<<ARC));

	// Dynamic length configurations: No dynamic length
	nrf24_configRegister(DYNPD,(0<<DPL_P0)|(0<<DPL_P1)|(0<<DPL_P2)|(0<<DPL_P3)|(0<<DPL_P4)|(0<<DPL_P5));

	return nrf24_is_connected() ? 0 : -1;
}

/* Set the RX address */
void nrf24_rx_address(uint8_t * adr) {
	nrf24_ce_digitalWrite(LOW);
	nrf24_writeRegister(RX_ADDR_P1,adr,nrf24_ADDR_LEN);
	nrf24_ce_digitalWrite(HIGH);
}

/* Returns the payload length */
uint8_t nrf24_payload_length() {
	return nrf24.payload_len;
}

/* Set the TX address */
void nrf24_tx_address(uint8_t* adr) {
	/* RX_ADDR_P0 must be set to the sending addr for auto ack to work. */
	nrf24_writeRegister(RX_ADDR_P0,adr,nrf24_ADDR_LEN);
	nrf24_writeRegister(TX_ADDR,adr,nrf24_ADDR_LEN);
}

/* Checks if data is available for reading */
/* Returns 1 if data is ready ... */
static uint8_t nrf24_rxDataReady() {
	// See note in getData() function - just checking RX_DR isn't good enough
	uint8_t status = nrf24_getStatus();

	// We can short circuit on RX_DR, but if it's not set, we still need
	// to check the FIFO for any pending packets
	if ( status & (1 << RX_DR) ) {
		if ((status & (0x7 << RX_P_NO)) == 0xE) {
			nrf24_configRegister(STATUS,(1<<RX_DR));
			return 0;
		} else {
			return 1;
		}
	}

	return !nrf24_rxFifoEmpty();
}

/* Checks if receive FIFO is empty or not */
uint8_t nrf24_rxFifoEmpty() {
	uint8_t fifoStatus;

	nrf24_readRegister(FIFO_STATUS,&fifoStatus,1);

	return (fifoStatus & (1 << RX_EMPTY));
}

/* Returns the length of data waiting in the RX fifo */
uint8_t nrf24_payloadLength()
{
	ipl_t ipl;
	uint8_t cmd = R_RX_PL_WID;
	uint8_t nop = 0x0;
	uint8_t status;
	NRF24_DO_IPL_LOCKED(
		nrf24_spi_transmit(&cmd, 1);
		nrf24_spi_transfer(&nop, &status, 1);
	);
	return status;
}

/* Reads payload bytes into data array */
static void nrf24_readData(uint8_t* data) {
	ipl_t ipl;
	uint8_t cmd = R_RX_PAYLOAD;

	NRF24_DO_IPL_LOCKED(
		/* Send cmd to read rx payload */
		nrf24_spi_transmit(&cmd, 1);
		/* Read payload */
		nrf24_transferSync(data, data, nrf24.payload_len);
	);

	/* Reset status register */
	nrf24_configRegister(STATUS,(1<<RX_DR));
}

/* Returns the number of retransmissions occured for the last message */
uint8_t nrf24_retransmissionCount(void) {
	uint8_t rv;
	nrf24_readRegister(OBSERVE_TX,&rv,1);
	rv = rv & 0x0F;
	return rv;
}

static void nrf24_delay(int delay) {
	while (delay--)
		;
}

// Sends a data package to the default address. Be sure to send the correct
// amount of bytes as configured as payload on the receiver.
void nrf24_send(uint8_t* value) 
{
	uint8_t cmd;
	ipl_t ipl;

	nrf24.last_tx_status = 0;

	/* Go to Standby-I first */
	nrf24_ce_digitalWrite(LOW);

	/* Set to transmitter mode , Power up if needed */
	nrf24_powerUpTx();

	nrf24_delay(1000000);

	/* Do we really need to flush TX fifo each time ? */
	#if 1
	NRF24_DO_IPL_LOCKED(
		/* Write cmd to flush transmit FIFO */
		cmd = FLUSH_TX;
		nrf24_spi_transmit(&cmd, 1);
	);
	#endif

	NRF24_DO_IPL_LOCKED(
		/* Write cmd to write payload */
		cmd = W_TX_PAYLOAD;
		nrf24_spi_transmit(&cmd, 1);

		/* Write payload */
		nrf24_transmitSync(value, nrf24.payload_len);
	);

	/* Start the transmission */
	nrf24_ce_digitalWrite(HIGH);
	nrf24_delay(100000);
	nrf24_ce_digitalWrite(LOW);
}

uint8_t nrf24_isSending() {
	return (nrf24.last_tx_status == 0);
}

uint8_t nrf24_getStatus() {
	ipl_t ipl;
	uint8_t cmd = CONFIG;
	uint8_t rv;
	NRF24_DO_IPL_LOCKED(
		nrf24_spi_transfer(&cmd, &rv, 1);
	);
	return rv;
}

uint8_t nrf24_lastMessageStatus() {
	/* Transmission went OK */
	if ((nrf24.last_tx_status & ((1 << TX_DS)))) {
		return NRF24_TRANSMISSON_OK;
	}
	/* Maximum retransmission count is reached */
	/* Last message probably went missing ... */
	else if ((nrf24.last_tx_status & ((1 << MAX_RT)))) {
		return NRF24_MESSAGE_LOST;
	}
	/* Probably still sending ... */
	else {
		return 0xFF;
	}
}

void nrf24_powerUpRx() {
	ipl_t ipl;
	uint8_t cmd = FLUSH_RX;

	NRF24_DO_IPL_LOCKED(
		nrf24_spi_transmit(&cmd, 1);
	);

	nrf24_configRegister(STATUS,(1<<RX_DR)|(1<<TX_DS)|(1<<MAX_RT));

	nrf24_ce_digitalWrite(LOW);
	nrf24_configRegister(CONFIG,nrf24_CONFIG|((1<<PWR_UP)|(1<<PRIM_RX)));
	nrf24_ce_digitalWrite(HIGH);
}

void nrf24_powerUpTx() {
	nrf24_configRegister(STATUS,(1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));

	nrf24_configRegister(CONFIG,nrf24_CONFIG | ((1 << PWR_UP)|(0 << PRIM_RX)));
}

void nrf24_powerDown() {
	nrf24_ce_digitalWrite(LOW);
	nrf24_configRegister(CONFIG,nrf24_CONFIG);
}

/* send and receive multiple bytes over SPI */
void nrf24_transferSync(uint8_t* dataout,uint8_t* datain,uint8_t len) {
	nrf24_spi_transfer(dataout, datain, len);
}

/* send multiple bytes over SPI */
void nrf24_transmitSync(uint8_t* dataout,uint8_t len) {
	nrf24_spi_transmit(dataout, len);
}

/* Clocks only one byte into the given nrf24 register */
void nrf24_configRegister(uint8_t reg, uint8_t value) {
	ipl_t ipl;
	uint8_t cmd = W_REGISTER | (REGISTER_MASK & reg);

	NRF24_DO_IPL_LOCKED(
		nrf24_spi_transmit(&cmd, 1);
		nrf24_spi_transmit(&value, 1);
	);
}

/* Read single register from nrf24 */
void nrf24_readRegister(uint8_t reg, uint8_t* value, uint8_t len) {
	ipl_t ipl;
	uint8_t cmd = R_REGISTER | (REGISTER_MASK & reg);

	NRF24_DO_IPL_LOCKED(
		nrf24_spi_transmit(&cmd, 1);
		nrf24_spi_receive(value, len);
	);
}

/* Write to a single register of nrf24 */
void nrf24_writeRegister(uint8_t reg, uint8_t* value, uint8_t len) {
	ipl_t ipl;
	uint8_t cmd = W_REGISTER | (REGISTER_MASK & reg);

	NRF24_DO_IPL_LOCKED(
		nrf24_spi_transmit(&cmd, 1);
		nrf24_spi_transmit(value, len);
	);
}

int nrf24_getData(uint8_t *data) {
	int ret;
	ipl_t ipl;

	ipl = ipl_save();
	ret = ring_buff_dequeue(&nrf24.rx_buf, data, nrf24.payload_len);
	ipl_restore(ipl);

	return ret;
}

uint8_t nrf24_dataReady(void) {
	return ring_buff_get_cnt(&nrf24.rx_buf) != 0;
}

int nrf24_handle_interrupt(void) {
	uint8_t status;
	uint8_t data_array[32]; /* 32 is a max payload size */

	status = nrf24_getStatus();

	if (status & (1 << RX_DR)) {
		nrf24_configRegister(STATUS, (1 << RX_DR));

		while (nrf24_rxDataReady()) {
			nrf24_readData(data_array);
			ring_buff_enqueue(&nrf24.rx_buf, &data_array, nrf24.payload_len);
		}
	} else if (status & (1 << TX_DS)) {
		nrf24.last_tx_status |= (1 << TX_DS);
		nrf24_configRegister(STATUS, (1 << TX_DS));
	} else if (status & (1 << MAX_RT)) {
		nrf24.last_tx_status |= (1 << MAX_RT);
		nrf24_configRegister(STATUS, (1 << MAX_RT));
	} else {
		log_error("Unknown interrupt");
		return -1;
	}

	return 0;
}
