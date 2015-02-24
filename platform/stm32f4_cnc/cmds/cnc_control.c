
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static void parse_com(char *com) {
	const char *sep = " \t\n\r";
	char *save;
	char cmd;
	int arg, frac;

	com = strtok_r(com, sep, &save);
	while (com) {
		cmd = *com;
		if (!('A' <= cmd && cmd <= 'Z')) {
			printf("Expected G-CODE command, but get '%c' (0x%hhu)\n",
					isprint(cmd) ? cmd : ' ', cmd);
			break;
		}
		if (2 > sscanf(com + 1, "%d.%d", &arg, &frac)) {
			frac = 0;
		}
		printf("cmd='%c', arg='%d', frac='%d'\n", cmd, arg, frac);
		com = strtok_r(NULL, sep, &save);
	}
	printf("newline\n");
}

int main(int argc, char *argv[]) {
	char line_buff[50];
	FILE *pf;

	if (argc < 2) {
		return -EINVAL;
	}

	pf = fopen(argv[1], "r");
	while (fgets(line_buff, sizeof(line_buff), pf)) {
		parse_com(line_buff);
		sleep(1);
	}

	return 0;
}
