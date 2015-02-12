
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
	int cnc_program_file;
	char line_buff[20];

	if (argc < 2) {
		return -EINVAL;
	}
	cnc_program_file = open(argv[1], O_RDONLY);
	read(cnc_program_file, line_buff, 19);
	line_buff[19] = '\0';

	printf("%s", line_buff);

	return 0;
}
