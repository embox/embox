
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "cnc_ipc.h"

static int cnc_g_ipc_out_fd = -1;
static void cnc_ctrl_ipc_init(void) {
	const char *cnc_out = getenv(CNCIPC_OUT);
	if (cnc_out) {
		cnc_g_ipc_out_fd = atoi(cnc_out);
	}
}

static void cnc_status_update(const char *str) {
	if (cnc_g_ipc_out_fd != -1) {
		write(cnc_g_ipc_out_fd, str, strlen(str));
	}
}

int main(int argc, char *argv[]) {
	int cnc_program_file;
	char line_buff[20];

	cnc_ctrl_ipc_init();

	cnc_status_update("cnc_control runned!");

	if (argc < 2) {
		return -EINVAL;
	}
	cnc_program_file = open(argv[1], O_RDONLY);
	read(cnc_program_file, line_buff, 19);
	line_buff[19] = '\0';

	printf("%s", line_buff);

	return 0;
}
