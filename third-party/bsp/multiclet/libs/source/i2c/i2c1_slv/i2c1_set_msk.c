#include "i2c.h"

void i2c1_set_msk(char msk_send_nack, char msk_send_byte, char msk_get_byte)
{

	I2C1->MSK = (msk_get_byte<<2)|(msk_send_byte<<1)|(msk_send_nack);

}
