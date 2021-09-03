/**
 * @file
 *
 * @date Aug 3, 2021
 * @author Anton Bondarev
 */

#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <lib/loop_file_logger.h>

#include <framework/mod/options.h>

#define LOGGER_FILE_NAME OPTION_STRING_GET(file_name)
#define RECORD_SIZE      OPTION_GET(NUMBER,record_size)
#define RECORD_QUANTITY  OPTION_GET(NUMBER,record_quantity)

static_assert(RECORD_QUANTITY <= 256, "");

static char loop_logger_buf[RECORD_SIZE];

static int find_current_record(int fd, int *label) {
	int res;
	int i;
	char cur_label;

	res = read(fd, loop_logger_buf, RECORD_SIZE);
	if (res != RECORD_SIZE) {
		*label = 0;
		return 0;
	}

	cur_label = loop_logger_buf[0];

	for(i = 1; i < RECORD_QUANTITY; i++) {
		res = read(fd, loop_logger_buf, RECORD_SIZE);
		if (res != RECORD_SIZE) {
			break;
		}
		if (loop_logger_buf[0] != cur_label) {
			*label = (loop_logger_buf[0] == '0') ? 1 : 0;
			return i;
		}
	}

	*label = (cur_label == '0') ? 1 : 0;
	return 0;
}

int loop_logger_write(char *message) {
	int fd;
	int start_idx = -1;
	int flip;
	char tmp_buf[4];
	int len;

	if (loop_logger_size() < RECORD_QUANTITY) {
		start_idx = 0;
	}
	fd = open(LOGGER_FILE_NAME, O_RDWR);
	if (fd < 0) {
		return -1;
	}
	if (start_idx == -1) {
		start_idx = find_current_record(fd, &flip);
	} else {
		start_idx = loop_logger_size();
		flip = 0;
	}
	lseek(fd, start_idx * RECORD_SIZE, SEEK_SET);
	memset(loop_logger_buf, 0, RECORD_SIZE);
	//snprintf(loop_logger_buf, "%d%02X:%s\n",flip, start_idx,message);
	itoa(flip, loop_logger_buf, 10);
	itoa(start_idx, tmp_buf, 16);
	if (strlen(tmp_buf) == 1) {
		strcat(loop_logger_buf,"0");
	}
	strcat(loop_logger_buf, tmp_buf);
	strcat(loop_logger_buf,":");
	strncat(loop_logger_buf, message, RECORD_SIZE - 1);

	len = strlen(loop_logger_buf);
	if (len < (RECORD_SIZE - 1)) {
		memset(&loop_logger_buf[len], ' ', (RECORD_SIZE - 1) - len);
	}
	loop_logger_buf[RECORD_SIZE - 1] = '\n';
	write(fd, loop_logger_buf, RECORD_SIZE);

	return 0;
}

int loop_logger_read(int idx_mes, char *out_mes, int buf_size) {
	int fd;
	int start_idx = -1;

	if (loop_logger_size() < RECORD_QUANTITY) {
		start_idx = 0;
	}
	fd = open(LOGGER_FILE_NAME, O_RDONLY);
	if (fd < 0) {
		return -1;
	}
	if (start_idx == -1) {
		int tmp;
		start_idx = find_current_record(fd, &tmp);
	}
	start_idx += idx_mes;
	start_idx %= RECORD_QUANTITY;
	lseek(fd, start_idx * RECORD_SIZE + 4, SEEK_SET);

	read(fd, out_mes, buf_size);
	close(fd);

	return 0;
}

int loop_logger_size(void) {
	struct stat st;

	stat(LOGGER_FILE_NAME, &st);
	if (st.st_size < RECORD_SIZE * RECORD_QUANTITY) {
		return st.st_size / RECORD_SIZE;
	}

	return RECORD_QUANTITY;
}

int loop_logger_message_size(void) {
	return RECORD_SIZE - (4 + 1);
}

