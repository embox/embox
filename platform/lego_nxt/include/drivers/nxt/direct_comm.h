/**
 * @file
 *
 * @date 24.10.10
 * @author Anton Kozlov
 */

#ifndef NXT_DIRECT_COMM_H_
#define NXT_DIRECT_COMM_H_

/* DC_MSG
 * bytes | 1    | 2      | 3     | 4	| ...
 *       | type | command| depends on types and length
 */

#define DC_MAX_LEN 64

struct lego_dc_msg {
	__le16 len;
	uint8_t type;
	uint8_t command;
	uint8_t body[DC_MAX_LEN - 2];
};

#define MSG_SIZE_BYTE_CNT 2 //TODO rename it DC_HEADER_LENGTH

#define DC_SET_OUTPUT_STATE 0x04
#define DC_GET_INPUT_VALUES 0x07
#define DC_KEEP_ALIVE 0x0d

#define DC_EX_SET_M_OUTPUT_STATE 0x20

#endif /* NXT_DIRECT_COMM_H */
