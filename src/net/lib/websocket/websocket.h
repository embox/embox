/**
 * @file
 * @brief WebSocket Core Header
 * 
 * @date Sept 10, 2025
 * @author Peize Li
 */

#ifndef NET_LIB_WEBSOCKET_H_
#define NET_LIB_WEBSOCKET_H_

#include <stdbool.h>
#include <stdint.h>
#include <sys/socket.h>

/* WebSocket protocol constants */
#define WS_GUID             "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define WS_KEY_LEN          16
#define WS_ACCEPT_LEN       28
#define WS_MAX_HEADER_SIZE  14
#define WS_MAX_PAYLOAD_SIZE 65536

/* WebSocket opcodes */
typedef enum {
	WS_OP_CONTINUATION = 0x0,
	WS_OP_TEXT = 0x1,
	WS_OP_BINARY = 0x2,
	WS_OP_CLOSE = 0x8,
	WS_OP_PING = 0x9,
	WS_OP_PONG = 0xA
} ws_opcode_t;

/* WebSocket states */
typedef enum {
	WS_STATE_CONNECTING,
	WS_STATE_OPEN,
	WS_STATE_CLOSING,
	WS_STATE_CLOSED
} ws_state_t;

/* Close codes */
typedef enum {
	WS_CLOSE_NORMAL = 1000,
	WS_CLOSE_GOING_AWAY = 1001,
	WS_CLOSE_PROTOCOL_ERROR = 1002,
	WS_CLOSE_UNSUPPORTED_DATA = 1003,
	WS_CLOSE_INVALID_PAYLOAD = 1007,
	WS_CLOSE_POLICY_VIOLATION = 1008,
	WS_CLOSE_MESSAGE_TOO_BIG = 1009,
	WS_CLOSE_INTERNAL_ERROR = 1011
} ws_close_code_t;

/* Forward declaration */
typedef struct ws_connection ws_connection_t;

/* Callback functions */
typedef struct ws_callbacks {
	void (*on_open)(ws_connection_t *ws);
	void (*on_message)(ws_connection_t *ws, const uint8_t *data, size_t len,
	    ws_opcode_t type);
	void (*on_close)(ws_connection_t *ws, uint16_t code, const char *reason);
	void (*on_error)(ws_connection_t *ws, const char *error);
	void (*on_ping)(ws_connection_t *ws, const uint8_t *data, size_t len);
	void (*on_pong)(ws_connection_t *ws, const uint8_t *data, size_t len);
} ws_callbacks_t;

/* WebSocket connection structure */
struct ws_connection {
	int sock_fd;
	ws_state_t state;
	ws_callbacks_t callbacks;

	/* Frame parsing state */
	uint8_t frame_buffer[WS_MAX_PAYLOAD_SIZE];
	size_t buffer_pos;
	size_t frame_size;

	/* Current frame info */
	struct {
		bool fin;
		ws_opcode_t opcode;
		bool masked;
		uint64_t payload_len;
		uint8_t mask_key[4];
	} current_frame;

	/* User data pointer */
	void *user_data;
};

/* Public API */
extern ws_connection_t *ws_connect(const char *url,
    const ws_callbacks_t *callbacks);
extern int ws_send_text(ws_connection_t *ws, const char *text);
extern int ws_send_binary(ws_connection_t *ws, const uint8_t *data, size_t len);
extern int ws_send_ping(ws_connection_t *ws, const uint8_t *data, size_t len);
extern int ws_close(ws_connection_t *ws, uint16_t code, const char *reason);
extern void ws_poll(ws_connection_t *ws);
extern void ws_free(ws_connection_t *ws);
extern void ws_random_bytes(uint8_t *buf, size_t len);

#endif /* NET_LIB_WEBSOCKET_H_ */