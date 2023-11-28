#include <stdio.h>
#include <pcap.h>

int main(int argc, char **argv) {
    pcap_if_t *alldevs; /* Name of devices (e.g. eth0, wlan0) */
    char error_buffer[PCAP_ERRBUF_SIZE]; /* Size defined in pcap.h */

    /* Find a device */
    if (pcap_findalldevs(&alldevs, error_buffer) < 0) {
        printf("Error finding device: %s\n", error_buffer);
        return 1;
    }

    while (alldevs != NULL)
    {
        printf("Network device found: %s\n", alldevs->name);
        printf("Network device description: %s\n", alldevs->description);
        alldevs = alldevs->next;
    }

    return 0;
}
