#ifndef _CPU_CONF_
#define _CPU_CONF_

/* CPU frequency (Hz) */
#define CORE_FREQ   50000000
/* Amount of available register windows */
#define CORE_NWINDOWS   8
/* Desired uart baud rate */
#define UART_BAUD_RATE  38400

#define UART_SCALER_VAL  ((((CORE_FREQ*10) / (8 * UART_BAUD_RATE))-5)/10)
/* Timing Calculation */
#define TIMER_SCALER_VAL (CORE_FREQ/1000000 - 1)

#endif //_CPU_CONF_
