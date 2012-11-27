/**
 * @file
 * @brief Simple HTTP server
 * @date 23.11.12
 * @author Vita Loginova
 */

#ifndef WEB_SERVER_H_
#define WEB_SERVER_H_

/**
 * Receives data from connected socket and sends a response
 *
 * @param sock
 *   socket file descriptor
 * @param addr
 *   address of connected socket
 * @param addr_len
 *   length of addr struct
 *
 * @see memcpy()
 */
extern void client_process(int sock, struct sockaddr_in addr, socklen_t addr_len)

#endif /* WEB_SERVER_H_ */
