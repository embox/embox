
#include <stdio.h>
#include <unistd.h>

#include <stm32f4xx_gpio.h>

int main(int argc, char *argv[]) {
	char buf[128];
	int count = 0;
	int err;

	err = printf("HTTP/1.1 200 OK\r\n"
		"Content-Type: text/event-stream\r\n"
		"Cache-Control: no-cache\r\n"
		"Connection: keep-alive\r\n"
		"\r\n");

	while (err > 0) {
		int nwrite = snprintf(buf, sizeof(buf), "data: \"(%d) live!\"\n\n", ++count);
		err = write(STDOUT_FILENO, buf, nwrite);
		sleep(1);
	}

	return 0;
}
