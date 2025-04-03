#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <drivers/gpio.h>
#include <drivers/serial/uart_dev.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>

static void print_usage(const char *cmd) {
	printf("Usage: %s [-h] [-i] ttyS<0/1>\n\t-i means \"use interrupt\"\n", cmd);
}

int main(int argc, char **argv) {
	char * msg = "Test message!";
	char buff[40];
	int use_int=0;
	
	if (argc < 2) {
		print_usage(argv[0]);
		return 0;
	}
	for (int opt; -1 != (opt = getopt(argc, argv, "hi"));) {
		switch (opt) {
		case 'i':
			use_int=1;
			break;
		case 'h':
		default:
			print_usage(argv[0]);
			return 0;
		}
	}
	
	printf("%s in/out test\n", argv[argc - 1]);
	
	struct uart *udev = uart_dev_lookup(argv[argc - 1]);
	if (udev == NULL) {
		printf("Failed to lookup %s", argv[argc - 1]);
		return -1;
	}
	
	if (use_int) udev->params.uart_param_flags |=  UART_PARAM_FLAGS_USE_IRQ;
		else udev->params.uart_param_flags &= ~UART_PARAM_FLAGS_USE_IRQ;
	
	uart_open(udev);

	printf("Writing: \"%s\"... & reading: ", msg);fflush(stdout);
	for (int i=0; i<strlen(msg); i++) {	
		uart_putc(udev, (int)msg[i]);
		for (clock_t t0 = clock(); !uart_hasrx(udev); usleep(5000)) if ((clock() - t0) > 50) break;
		buff[i] = uart_getc(udev);
		printf("%c", buff[i]);
		fflush(stdout);
	}
	printf("\nOK\n");

	uart_close(udev);

	return 0;
}
