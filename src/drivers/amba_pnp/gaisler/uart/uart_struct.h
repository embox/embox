/**
 * \file uart_struct.h
 */

#ifndef _UART_STRUCT_H_
#define _UART_STRUCT_H_

typedef struct _UART_STRUCT
{
    volatile UINT32 data;	//0x70
    volatile UINT32 status;
    volatile UINT32 ctrl;
    volatile UINT32 scaler;
} UART_STRUCT;

#endif /* _UART_STRUCT_H_ */
