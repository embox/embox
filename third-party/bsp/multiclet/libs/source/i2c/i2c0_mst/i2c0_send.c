#include "i2c.h"
void i2c0_send(char byte, char slv_addr, int addr)
{
	I2C0_TX_DATA(slv_addr); //set Slave Address for transmit
	I2C0_CMD(I2C0_START_WR); //send START + WR
	while(I2C0_TIP); //wait end of data transmit
	while(I2C0_ACK_RX); //wait ACK

	I2C0_TX_DATA((addr & 0xFF00)>>8); //1 address byte for transmit
	I2C0_CMD(I2C0_WR); //send WR
	while(I2C0_TIP); //wait end of data transmit
	while(I2C0_ACK_RX); //wait ACK

	I2C0_TX_DATA(addr & 0xFF); //2 address byte for transmit
	I2C0_CMD(I2C0_WR); //send WR
	while(I2C0_TIP); //wait end of data transmit
	while(I2C0_ACK_RX); //wait ACK

	I2C0_TX_DATA(byte); //set byte for transmit
	I2C0_CMD(I2C0_WR); //send WR
	while(I2C0_TIP); //wait end of data transmit
	while(I2C0_ACK_RX); //wait ACK

	I2C0_CMD(I2C0_STOP_WR); //send STOP + WR
	while(I2C0_TIP); //wait end of data transmit
	while(I2C0_ACK_RX); //wait ACK
}
