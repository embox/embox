
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int count;
	char line_buff[20];
	int cnc_program_file;

	if (argc < 2) {
		return -EINVAL;
	}

	cnc_program_file = open(argv[1], O_RDONLY);
	read(cnc_program_file, line_buff, 19);
	line_buff[19] = '\0';

	printf("cnc_control runned!\n");

	count = 20;
	while(count-- > 0) {
		printf("cnc_control: %d\n", count);
		sleep(1);
	}

	return 0;
}
