#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mqtt_udp.h>

#define MSG_LEN 256

int main(int argc, const char **argv) {
	char topic[MSG_LEN], value[MSG_LEN];

	strcpy(topic, "/temp");
	strcpy(value, "40");

	for (int i = 1; i <= 10; ++i) {
		sprintf(value, "%i", 40 - i);

		int rc = mqtt_udp_send_publish(topic, value);
		
		if (rc)
			printf("rc = %d\n", rc);

		sleep(1);
	}
	
	return 0;
}
