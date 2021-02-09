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

#include <kernel/printk.h>

int main(int argc, char *argv[]) {
	unsigned int light;

	printk("Enter getlight\n");

	printf(
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/event-stream\r\n"
		"Cache-Control: no-cache\r\n"
		"Connection: keep-alive\r\n"
		"\r\n"
	);

	while (1) {
		light = rand() % 15000;

		if (0 > printf("data: {\"light\" : \"%d\"}\n\n", light)) {
			printk("gellight exit!\n");
			break;
		}

		sleep(1);
	}

	printk("Exit getlight\n");

	return 0;
}
