/*
 * @brief Interface for TFTP operations. Based on old version of cmds/net/tftp.c
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 * @version
 * @date 18.06.2019
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <lib/tftp.h>
#include <util/log.h>
#include <kernel/printk.h>

struct tftp_msg {
	uint16_t opcode;
	union {
		struct {
			char name_and_mode[2];
		} cmd /*__attribute__ ((packed))*/;
		struct {
			uint16_t block_num;
			char stuff[TFTP_SEGSIZE];
		} data /*__attribute__ ((packed))*/;
		struct {
			uint16_t block_num;
		} ack /*__attribute__ ((packed))*/;
		struct {
			uint16_t error_code;
			char error_msg[1];
		} err /*__attriibute__ ((packed))*/;
	} op /*__attribute__ ((packed))*/;
} __attribute__ ((packed));

struct tftp_stream {
	char filename[PATH_MAX];

	int pkg_number;

	struct tftp_msg snd;
	struct tftp_msg rcv;
	size_t snd_len;
	size_t rcv_len;

	struct sockaddr_storage rem_addr;
	socklen_t rem_addrlen;

	int sock;

	bool transmission_end;
};

static char *get_transfer_mode(bool binary_on) {
	return binary_on ? "octet" : "netascii";
}

static int tftp_build_msg_cmd(struct tftp_msg *msg, size_t *msg_len,
		uint16_t type, const char *filename, char *mode) {
	char *ptr;
	size_t sz;

	msg->opcode = htons(type);
	*msg_len = sizeof msg->opcode;

	ptr = &msg->op.cmd.name_and_mode[0];
	sz = strlen(filename) + 1;
	memcpy(ptr, filename, sz * sizeof(char));
	*msg_len += sz * sizeof(char);

	ptr += sz;
	sz = strlen(mode) + 1;
	memcpy(ptr, mode, sz * sizeof(char));
	*msg_len += sz * sizeof(char);

	return 0;
}

static int tftp_build_msg_data(struct tftp_msg *msg, size_t *msg_len,
		uint8_t *data, int data_len, uint16_t block_num) {
	msg->opcode = htons(TFTP_DATA);
	*msg_len = sizeof msg->opcode;

	msg->op.data.block_num = htons(block_num);
	*msg_len += sizeof msg->op.data.block_num;

	memcpy(msg->op.data.stuff, data, data_len);

	*msg_len += data_len;

	return 0;
}

static int tftp_build_msg_ack(struct tftp_msg *msg, size_t *msg_len, uint16_t block_num) {
	msg->opcode = htons(TFTP_ACK);
	*msg_len = sizeof msg->opcode;

	msg->op.ack.block_num = htons(block_num);
	*msg_len += sizeof msg->op.ack.block_num;

	return 0;
}

static int msg_with_correct_len(struct tftp_msg *msg, size_t msg_len) {
	size_t field_sz, left_sz;
	char *tmp;

	left_sz = msg_len;

	field_sz = sizeof msg->opcode;
	if (left_sz < field_sz) return 0;

	left_sz -= field_sz;

	switch (ntohs(msg->opcode)) {
	case TFTP_RRQ:
	case TFTP_WRQ:
		tmp = &msg->op.cmd.name_and_mode[0];
		/* filename */
		do
			if (left_sz-- == 0) return 0;
		while (*tmp++ != '\0');
		/* mode */
		do
			if (left_sz-- == 0) return 0;
		while (*tmp++ != '\0');
		break;
	case TFTP_DATA:
		/* block number */
		field_sz = sizeof msg->op.data.block_num;
		if (left_sz < field_sz) return 0;
		left_sz -= field_sz;
		/* data */
		left_sz = 0;
		break;
	case TFTP_ACK:
		/* block number */
		field_sz = sizeof msg->op.ack.block_num;
		if (left_sz < field_sz) return 0;
		left_sz -= field_sz;
		break;
	case TFTP_ERROR:
		/* error code */
		field_sz = sizeof msg->op.err.error_code;
		if (left_sz < field_sz) return 0;
		left_sz -= field_sz;
		/* error message */
		tmp = &msg->op.err.error_msg[0];
		do
			if (left_sz-- == 0) return 0;
		while (*tmp++ != '\0');
		break;
	default: /* unknown operation */
		return 0;
	}

	return !left_sz;
}

static int 
tftp_msg_send(struct tftp_msg *msg, size_t msg_len, struct tftp_stream *s) {
	int res;

	assert(s);
	assert(msg);
	 /* debug msg_with_correct_len */
	assert(msg_with_correct_len(msg, msg_len));

	res = sendto(s->sock, (char *)msg, msg_len, 0,
					(struct sockaddr *) &s->rem_addr, s->rem_addrlen);
	if (res == -1) {
		log_error("tftp: send() failure");
		return -errno;
	}

	return 0;
}

static int 
tftp_msg_recv(struct tftp_msg *msg, size_t *msg_len, struct tftp_stream *s) {
	ssize_t ret;

	assert(s);
	assert(msg);
	assert(msg_len);

	ret = recvfrom(s->sock, (char *) msg, sizeof (*msg), 0,
						(struct sockaddr *)&s->rem_addr, &s->rem_addrlen);
	if (ret == -1) {
		log_error("tftp: recv() failure");
		return -errno;
	}

	*msg_len = ret;

	return 0;
}

static struct tftp_stream stream;

static int make_remote_addr(const char *hostname,
		struct sockaddr_storage *out_raddr, socklen_t *out_raddr_len) {
	int ret;
	struct sockaddr_in *raddr_in;

	raddr_in = (struct sockaddr_in *)out_raddr;
	memset(out_raddr, 0, sizeof *out_raddr);
	raddr_in->sin_family = AF_INET;
	raddr_in->sin_port = htons(TFTP_TRANSFER_PORT);
	ret = inet_aton(hostname, &raddr_in->sin_addr);
	if (!ret) {
		fprintf(stderr, "Can't parse remote address '%s`\n", hostname);
		return -EINVAL;
	}

	*out_raddr_len = sizeof *raddr_in;

	return 0;
}

static int tftp_resend_cmd(struct tftp_stream *s) {
	if (0 != tftp_msg_send(&s->snd, s->snd_len, s)) {
		return -1;
	}

	if (0 != tftp_msg_recv(&s->rcv, &s->rcv_len, s)) {
		return -2;
	}

	return 0;
}

struct tftp_stream *tftp_new_stream(const char *host, const char *file, int dir, bool binary_mode) {
	struct tftp_stream *s = &stream;

	memset(s, 0, sizeof(*s));

	if (0 != make_remote_addr(host,
				&s->rem_addr,
				&s->rem_addrlen)) {
		goto fail;
	}
	if (-1 == (s->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))) {
		goto fail;
	}

	if (dir == TFTP_DIR_PUT) {
		if (0 != tftp_build_msg_cmd(&s->snd,
				&s->snd_len,
				TFTP_WRQ,
				file,
				get_transfer_mode(binary_mode))) {
			goto fail;
		}
	} else {
		if (0 != tftp_build_msg_cmd(&s->snd,
				&s->snd_len,
				TFTP_RRQ,
				file,
				get_transfer_mode(binary_mode))) {
			goto fail;
		}
	}

	do {
		if (tftp_resend_cmd(s)) {
			log_error("ERROR\n");
			break;
		}
	} while (!msg_with_correct_len(&s->rcv, s->rcv_len));

	strncpy(s->filename, file, sizeof(s->filename));

	return s;

fail:
	if (s->sock >= 0) {
		close(s->sock);
	}

	memset(s, 0, sizeof(*s));

	return NULL;
}

int tftp_delete_stream(struct tftp_stream *s) {
	if (s == NULL) {
		return -EINVAL;
	}

	if (s->sock >= 0) {
		close(s->sock);
	}

	memset(s, 0, sizeof(*s));

	return 0;
}

int tftp_stream_write(struct tftp_stream *s, uint8_t *buf, size_t len) {
	if (s == NULL) {
		log_warning("stream is NULL, do nothing");
		return 0;
	}

	switch (ntohs(s->rcv.opcode)) {
		case TFTP_ACK:
			while (ntohs(s->rcv.op.ack.block_num) != s->pkg_number) {
				tftp_resend_cmd(s); /* invalid acknowledgement, send again */
			}
			break;
		case TFTP_ERROR:
			return -s->rcv.op.err.error_code;
		default:
			/* error */
			return 0;
	}

	/* whether we have more data to transfer? */
	if ((s->pkg_number != 0) && (s->snd_len != sizeof s->snd)) {
		/* end of transmission */
	}

	/* get next stuff of data */
	if (0 != tftp_build_msg_data(&s->snd, &s->snd_len, buf, len, ++s->pkg_number)) {
		goto fail;
	}

	/* send request / data */
	if (0 != tftp_msg_send(&s->snd, s->snd_len, s)) {
		goto fail;
	}

	return s->snd_len;
fail:
	return 0;
}

int tftp_stream_read(struct tftp_stream *s, uint8_t *buf) {
	int data_len;

	if (s->transmission_end) {
		return 0;
	}

	switch (ntohs(s->rcv.opcode)) {
		case TFTP_DATA:
			while (ntohs(s->rcv.op.ack.block_num) != s->pkg_number + 1) {
				tftp_resend_cmd(s);
			}

			data_len = s->rcv_len - (sizeof s->rcv - sizeof s->rcv.op.data.stuff);
			memcpy(buf, s->rcv.op.data.stuff, data_len);

			if (s->rcv_len != sizeof(s->rcv)) {
				s->transmission_end = true;
			}
			break;
		case TFTP_ERROR:
			return -s->rcv.op.err.error_code;
			goto fail;
		default:
			goto fail;
	}

	if (0 != tftp_build_msg_ack(&s->snd, &s->snd_len, ++s->pkg_number)) {
		goto fail;
	}

	if (0 != tftp_msg_send(&s->snd, s->snd_len, s)) {
		goto fail;
	}

	return data_len;
fail:
	return 0;
}

const char *tftp_error(struct tftp_stream *s) {
	assert(s);

	return s->rcv.op.err.error_msg;
}
