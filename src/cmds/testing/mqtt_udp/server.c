#include <stdio.h>
#include <mqtt_udp.h>

int main(int argc, const char **argv) {

	int rc = mqtt_udp_recv_loop(mqtt_udp_dump_any_pkt);

	if (rc)
		printf("rc = %d\n", rc);

	return 0;
}
