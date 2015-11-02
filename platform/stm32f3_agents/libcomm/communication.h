/**
 * @file communication.h
 * @brief Library for communication between agents, currently using usart
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-11-02
 */

/**
 * @brief Initialize communication interface
 *
 * @return Interface num
 */
extern int comm_init(void);

extern int send_byte(uint8_t b, int i_num);
extern uint8_t get_byte(int i_num);
