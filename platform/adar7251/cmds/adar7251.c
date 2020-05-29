#include <stdio.h>

#include <drivers/adar7251_driver.h>


int main(int argc, char **argv) {
	static struct adar7251_dev adar7251_dev;

	printf("starting adar7251\n");
	adar7251_hw_init(&adar7251_dev);

	printf("conversation loop\n");

	return 0;
}
