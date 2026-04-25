#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>

#include <util/math.h>

int main(void) {
    int s;
    struct sockaddr_can addr = {0};
    struct ifreq ifr = {0};
    struct can_frame frame = {0};

    // 1. Create Socket (PF_CAN, Raw Socket, CAN_RAW)
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        printf("Socket failed\n");
        return 1;
    }

    // 2. Specify Interface
    strcpy(ifr.ifr_name, "vcan0");
    if (0 > ioctl(s, SIOCGIFINDEX, &ifr)) {
        printf("ioctl SIOCGIFINDEX failed\n");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    // 3. Bind the socket to the CAN interface
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("Bind failed\n");
        return 1;
    }

    /* read */
    if (read(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        printf("read failed\n");
        return 1;
    }
    printf("Recv: can_id = 0x%08x, can_dlc=%d\n"
        "data(%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x)\n",
        frame.can_id, frame.can_dlc,
        frame.data[0], frame.data[1], frame.data[2], frame.data[3],
        frame.data[4] , frame.data[5], frame.data[6], frame.data[7]);
    // 6. Close socket
    close(s);
    return 0;
}