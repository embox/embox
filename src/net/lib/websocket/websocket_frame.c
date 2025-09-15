/**
 * @file
 * @brief WebSocket Frame Encoder/Decoder
 * 
 * @date Sept 10, 2025
 * @author Peize Li
 */

#include <arpa/inet.h>
#include <string.h>

#include <net/lib/websocket.h>

/* Helper for 64-bit network byte order */
static uint64_t htonll(uint64_t value) {
	static const int test = 1;
	if (*(char *)&test == 1) {
		/* Little endian - swap */
		return ((uint64_t)htonl(value & 0xFFFFFFFF) << 32) | htonl(value >> 32);
	}
	else {
		/* Big endian - no swap needed */
		return value;
	}
}

static uint64_t ntohll(uint64_t value) {
	return htonll(value);
}

/**
 * Build WebSocket frame header
 * @return Header size in bytes
 */
static size_t ws_build_frame_header(uint8_t *buffer, bool fin,
    ws_opcode_t opcode, bool masked, uint64_t payload_len) {
	size_t pos = 0;

	/* First byte: FIN (1 bit) + RSV (3 bits) + Opcode (4 bits) */
	buffer[pos++] = (fin ? 0x80 : 0x00) | (opcode & 0x0F);

	/* Second byte: MASK (1 bit) + Payload length (7 bits or extended) */
	if (payload_len < 126) {
		buffer[pos++] = (masked ? 0x80 : 0x00) | (uint8_t)payload_len;
	}
	else if (payload_len <= 0xFFFF) {
		buffer[pos++] = (masked ? 0x80 : 0x00) | 126;
		uint16_t len16 = htons((uint16_t)payload_len);
		memcpy(&buffer[pos], &len16, 2);
		pos += 2;
	}
	else {
		buffer[pos++] = (masked ? 0x80 : 0x00) | 127;
		uint64_t len64 = htonll(payload_len);
		memcpy(&buffer[pos], &len64, 8);
		pos += 8;
	}

	/* Masking key (client must mask) */
	if (masked) {
		ws_random_bytes(&buffer[pos], 4);
		pos += 4;
	}

	return pos;
}

/**
 * Parse WebSocket frame header
 * @return Bytes consumed or -1 on error, 0 if need more data
 */
int ws_parse_frame_header(ws_connection_t *ws, const uint8_t *data, size_t len) {
	if (len < 2)
		return 0;

	size_t pos = 0;

	/* Parse first byte */
	ws->current_frame.fin = (data[pos] & 0x80) != 0;
	ws->current_frame.opcode = (ws_opcode_t)(data[pos] & 0x0F);
	pos++;

	/* Validate opcode */
	if (ws->current_frame.opcode > WS_OP_PONG) {
		return -1;
	}

	/* Parse second byte */
	ws->current_frame.masked = (data[pos] & 0x80) != 0;
	uint8_t payload_len = data[pos] & 0x7F;
	pos++;

	/* Extended payload length */
	if (payload_len == 126) {
		if (len < pos + 2)
			return 0;
		uint16_t len16;
		memcpy(&len16, &data[pos], 2);
		ws->current_frame.payload_len = ntohs(len16);
		pos += 2;
	}
	else if (payload_len == 127) {
		if (len < pos + 8)
			return 0;
		uint64_t len64;
		memcpy(&len64, &data[pos], 8);
		ws->current_frame.payload_len = ntohll(len64);
		pos += 8;
	}
	else {
		ws->current_frame.payload_len = payload_len;
	}

	/* Masking key */
	if (ws->current_frame.masked) {
		if (len < pos + 4)
			return 0;
		memcpy(ws->current_frame.mask_key, &data[pos], 4);
		pos += 4;
	}

	return (int)pos;
}

/**
 * Send WebSocket frame
 */
int ws_send_frame(ws_connection_t *ws, ws_opcode_t opcode, const uint8_t *data,
    size_t len) {
	uint8_t header[WS_MAX_HEADER_SIZE];
	uint8_t masked_payload[WS_MAX_PAYLOAD_SIZE];

	if (!ws || ws->sock_fd < 0) {
		return -1;
	}

	if (ws->state != WS_STATE_OPEN && opcode != WS_OP_CLOSE) {
		return -1;
	}

	if (len > WS_MAX_PAYLOAD_SIZE) {
		return -1;
	}

	/* Build frame header (clients must mask) */
	size_t header_len = ws_build_frame_header(header, true, opcode, true, len);

	/* Get mask key from header */
	uint8_t *mask_key = &header[header_len - 4];

	/* Mask payload */
	if (data && len > 0) {
		for (size_t i = 0; i < len; i++) {
			masked_payload[i] = data[i] ^ mask_key[i % 4];
		}
	}

	/* Send header */
	if (send(ws->sock_fd, header, header_len, 0) != (ssize_t)header_len) {
		return -1;
	}

	/* Send masked payload */
	if (len > 0) {
		if (send(ws->sock_fd, masked_payload, len, 0) != (ssize_t)len) {
			return -1;
		}
	}

	return 0;
}
