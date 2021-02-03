/**
 * @file
 * @brief Stub to simulate light sensor
 *
 * @date 02.02.2021
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	unsigned int x, y, z;

	printf(
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/event-stream\r\n"
		"Cache-Control: no-cache\r\n"
		"Connection: keep-alive\r\n"
		"\r\n"
	);

	while (1) {
		x = rand() % 15000;
		y = rand() % 15000;
		z = rand() % 15000;

		if (0 > printf("data: {\"rate\" : \"x:%d y:%d z:%d\"}\n\n", x, y, z)) {
			break;
		}

		sleep(1);
	}

	return 0;
}
