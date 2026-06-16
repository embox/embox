/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 26.05.26
 */

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/can.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <drivers/char_dev.h>

static void print_usage(void) {
	printf("Usage: cansend <device> <can_frame>\n\n");
	printf("<can_frame>:\n");
	printf(" <can_id>#{data}         (CAN 2.0)\n");
	printf(" <can_id>##<flags>{data} (CAN FD)\n\n");
}

static void print_error(const char *func, const char *err_msg) {
	printf("%s(): %s\n", func, err_msg);
}

static uint8_t char2int(unsigned char c) {
	if (isdigit(c)) {
		c -= '0';
	}
	else {
		c -= isupper(c) ? 'A' - 10 : 'a' - 10;
	}

	return c & 0xf;
}

static uint32_t str2int(const char *str, size_t len) {
	uint32_t res;
	int i;

	res = 0;

	if (len > 8) {
		len = 8;
	}

	for (i = 0; i < len; i++) {
		res |= char2int(str[i]) << (4 * (len - i - 1));
	}

	return res;
}

static int parse_canframe(const char *str, struct canfd_frame *frame) {
	size_t data_max;
	size_t data_len;
	size_t hdr_len;
	size_t str_len;
	size_t id_len;
	size_t mtu;
	canid_t id;
	canid_t id_mask;
	uint8_t flags;
	int i;

	str_len = strlen(str);

	if ((str_len > 3) && (str[3] == '#')) {
		id = 0;
		id_len = 3;
		id_mask = CAN_SFF_MASK;
	}
	else if ((str_len > 8) && (str[8] == '#')) {
		id = CAN_EFF_FLAG;
		id_len = 8;
		id_mask = CAN_EFF_MASK;
	}
	else {
		return -1;
	}

	for (i = 0; i < id_len; i++) {
		if (!isxdigit(str[i])) {
			return -1;
		}
	}

	hdr_len = id_len;
	id |= str2int(str, id_len) & id_mask;

	if (str[id_len + 1] != '#') {
		hdr_len += 1;
		mtu = CAN_MTU;
		data_max = CAN_MAX_DLEN * 2;
		flags = 0;
	}
	else if (isxdigit(str[id_len + 2])) {
		hdr_len += 3;
		mtu = CANFD_MTU;
		data_max = CANFD_MAX_DLEN * 2;
		flags = char2int(str[id_len + 2]);
	}
	else {
		return -1;
	}

	data_len = str_len - hdr_len;

	if ((data_len > data_max) || (data_len & 1)) {
		return -1;
	}

	for (i = hdr_len; i < str_len; i++) {
		if (!isxdigit(str[i])) {
			return -1;
		}
	}

	memset(frame, 0, sizeof(struct canfd_frame));

	frame->can_id = id;
	frame->flags = flags;
	frame->len = data_len / 2;

	for (i = 0; i < frame->len; i++) {
		frame->data[i] = str2int(str + hdr_len + 2 * i, 2);
	}

	return mtu;
}

int main(int argc, char const *argv[]) {
	struct canfd_frame frame;
	ssize_t written;
	int mtu;
	int fd;

	if (argc != 3) {
		print_usage();
		return 1;
	}

	mtu = parse_canframe(argv[2], &frame);
	if (mtu < 0) {
		print_usage();
		print_error("parse_canframe", "Invalid <can_frame> format");
		return 1;
	}

	fd = char_dev_open(argv[1], O_RDWR);
	if (fd < 0) {
		print_error("char_dev_open", strerror(-fd));
		return 1;
	}

	written = write(fd, &frame, mtu);

	close(fd);

	if (written != mtu) {
		print_error("write", strerror(errno));
		return 1;
	}

	return 0;
}
