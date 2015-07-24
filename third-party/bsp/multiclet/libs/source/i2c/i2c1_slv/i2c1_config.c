#include "i2c.h"
void i2c1_config(char enable, char transmit_always_valid, char transmit_valid, char tmod, char rmod)
{
	I2C1->CR = (rmod<<4)|(tmod<<3)|(transmit_valid<<2)|(transmit_always_valid<<1)|(enable); //config control register
}
