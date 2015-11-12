/**
 * @file communication.h
 * @brief Library for communication between agents, currently using usart
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-11-02
 */

#define UART_BUFF_SZ     16
#define UART_BAUD_RATE   9600
/**
 * @brief Initialize communication interface
 *
 * @return Interface num
 */
extern int comm_init(void);

extern int send_byte(uint8_t b, int i_num);
extern uint8_t get_byte(int i_num);
