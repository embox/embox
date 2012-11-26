/*
 * web_server.h
 *
 *  Created on: Nov 23, 2012
 *      Author: vita
 */

#ifndef WEB_SERVER_H_
#define WEB_SERVER_H_

extern void client_process(int sock, struct sockaddr_in addr,
		socklen_t addr_len);

#endif /* WEB_SERVER_H_ */
