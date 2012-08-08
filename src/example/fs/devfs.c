/**
 * @file
 *
 * @brief
 *
 * @date 30.11.2011
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <errno.h>
#include <framework/example/self.h>

/**
 * This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(run);

/**
 * Example's execution routing
 * It has been declare with macro EMBOX_EXAMPLE
 */
static int run(int argc, char **argv) {
	FILE *file;
	char out_buff[] = "stdlib output (fwrite)\n";
	char in_buff[20] = "";
	int i = 0;

	printf("example devfs stdlib style\n");

	if(NULL == (file = fopen("/dev/uart", "rw"))) {
		printf("file couldn't open\n");
		return -1;
	}
	fwrite(out_buff, sizeof(out_buff), 1, file);

	do {
		if(i++ >= sizeof(in_buff) - 1) {
			break;
		}
		fread(&in_buff[i-1], 1, 1, file);
	} while(in_buff[i-1] != '\n');
	in_buff[i] = '\0';

	printf("read from uart: %s\n", in_buff);
	fclose(file);

	return ENOERR;
}
