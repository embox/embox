#include "i2c.h"
void i2c0_init(char enable, int clock_speed, int system_freq)
{
	I2C0->CR = (enable << 7); //enable or disable
	I2C0->PSC = system_freq/(5*clock_speed)-1; //set prescaler for I2C
}
