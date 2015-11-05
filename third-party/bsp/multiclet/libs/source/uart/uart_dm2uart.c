#include "uart.h"
/*
Variable for DM2UART function:

DM_START - start block address for send by UART
BLOCK_SIZE - end block address for send by UART
iterator1 - number one byte for send by UART
iterator2 - counter fifo UART
byte_DM - contain byte for send by UART
*/

int read_DM(int *iterator1, int *iterator2, int block_size, char *byte_dm, int dm_start)
{
		if(block_size + dm_start - *iterator1 != 0)
		{
			if(*iterator1 == 0) *iterator1 = *iterator1 + dm_start;
			*byte_dm = *((char*)(*iterator1)); //read 1 byte from DM
			*iterator1 = *iterator1 + 1;
			*iterator2 = *iterator2 - 1;
			return 0;
		}
		else
		{
			return 1;
		}
}

int full_fifo(int *iterator2)
{
	if(*iterator2 == 0)
	{
		*iterator2 = 32;
		return 1;
	}
	else
	{
		return 0;
	}
}

void DM2UART(UART_TypeDef *UART, int start_address, int size_block)
{
	int iterator1, iterator2;
	int	dm_start, block_size;
	char byte_dm;
	iterator2 = 32;
	iterator1 = 0;
	dm_start = start_address;
	block_size = size_block;
    while(UART_FIFO_TX_EMPTY(UART) != 1);

	while(read_DM(&iterator1, &iterator2, block_size,&byte_dm, dm_start) == 0)
	{
		UART_SEND_BYTE(byte_dm, UART);

		if(full_fifo(&iterator2) == 1)
		{
			while(UART_FIFO_TX_EMPTY(UART) != 1);

		}
	}
}
