#include <stdio.h>

#include <drivers/adar7251_driver.h>
#include <drivers/nucleo_f429zi_audio.h>

static uint8_t rx_buf[SAI_SAMPLES_BUFFER * 2]; /* half of sai buffer */
static struct adar7251_dev adar7251_dev;

int main(int argc, char **argv) {
	int data_len;

	printf("starting adar7251\n");
	adar7251_hw_init(&adar7251_dev);

	printf("conversation loop\n");

	while(1) {
		data_len = sai_receive(adar7251_dev.sai_dev, rx_buf, sizeof(rx_buf));
		printf("%d\n", data_len);
	}

	return 0;
}
