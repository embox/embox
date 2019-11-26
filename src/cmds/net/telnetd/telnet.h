/**
 * @file
 * @brief Tiny telnetd server
 *
 * @date 04.12.2019
 * @author Alexander Kalmuk
 */

#ifndef TELNET_H_
#define TELNET_H_

/* http://www.tcpipguide.com/free/t_TelnetProtocolCommands-3.htm */
#define T_WILL      251
#define T_WONT      252
#define T_DO        253
#define T_DONT      254
#define T_IAC       255
#define T_INTERRUPT 244

#define O_ECHO      1     /* Manage ECHO, RFC 857 */
#define O_GO_AHEAD  3     /* Disable GO AHEAD, RFC 858 */

#define DATA_BUF_LEN 128

struct data_buffer {
	unsigned char data[DATA_BUF_LEN];
	unsigned char *p; /* Reader position in buf. */
	size_t count; /* Bytes available for read */
};

struct telnet_session {
	int sockfd;
	int ptyfd;
	int pid; /* PID of current session. */

#ifndef TELNETD_FOR_LINUX
	int pptyfds[2];
#endif

	/* These buffers are _not_ ring buffers. The logic of the buffer
	 * is easy: we push data to buffer only when buffer is empty.
	 */
	struct data_buffer sock_buff;
	struct data_buffer pty_buff;
};

extern void telnet_cmd(int sock, unsigned char op, unsigned char param);
extern size_t telnet_handle_client_data(unsigned char *data, size_t len,
		int sock);

extern int telnet_open_master_pty(struct telnet_session *ts);
extern int telnet_open_slave_pty(struct telnet_session *ts,
	int master_pty);

#endif /* TELNET_H_ */
