/**
 * @file
 *
 * @date Aug 3, 2021
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lib/loop_file_logger.h>

static void print_log(void) {
	int log_size;
	int i;
	char *buf;
	int mes_size;

	mes_size = loop_logger_message_size();

	buf = malloc(mes_size + 1);

	log_size = loop_logger_size();
	for (i = 0; i < log_size; i++) {
		memset(buf, 0, mes_size + 1);
		loop_logger_read(i, buf, mes_size);
		puts(buf);
	}

	free(buf);
}

static void record_log(char *message) {
	loop_logger_write(message);
}

int main(int argc, char **argv) {
	if (argc == 2) {
		record_log(argv[1]);
	} else {
		print_log();
	}
	return 0;
}
