#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
//#include <linux/can/raw.h>

int main(void) {
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    // 1. Create Socket (PF_CAN, Raw Socket, CAN_RAW)
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        printf("Socket failed\n");
        return 1;
    }

    // 2. Specify Interface
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(s, SIOCGIFINDEX, &ifr);

    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    // 3. Bind the socket to the CAN interface
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("Bind failed\n");
        return 1;
    }

    // 4. Prepare frame
    frame.can_id = 0x123;
    frame.can_dlc = 2;
    frame.data[0] = 0xDE;
    frame.data[1] = 0xAD;

    // 5. Write frame
    if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        printf("Write failed\n");
        return 1;
    }

    // 6. Close socket
    close(s);
    return 0;
}