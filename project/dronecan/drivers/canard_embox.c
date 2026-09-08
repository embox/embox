#include <dronecan/canard_embox.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/can.h>
#include <poll.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <drivers/char_dev.h>

int canardEmboxInit(CanardEmboxInstance* out_ins, const char* can_iface_name)
{
    const int fd = char_dev_open(can_iface_name, O_RDWR | O_NONBLOCK);
    if (fd < 0)
    {
        return -1;
    }

    out_ins->fd = fd;
    return 0;
}

int canardEmboxClose(CanardEmboxInstance* ins)
{
    const int close_result = close(ins->fd);
    ins->fd = -1;
    return close_result;
}

int canardEmboxTransmit(CanardEmboxInstance* ins, const CanardCANFrame* frame, int timeout_msec)
{
    struct pollfd fds;
    memset(&fds, 0, sizeof(fds));
    fds.fd = ins->fd;
    fds.events |= POLLOUT;

    const int poll_result = poll(&fds, 1, timeout_msec);
    if (poll_result < 0)
    {
        return -1;
    }
    if (poll_result == 0)
    {
        return 0;
    }
    if ((fds.revents & POLLOUT) == 0)
    {
        return -1;
    }

    struct can_frame tx_frame;
    memset(&tx_frame, 0, sizeof(tx_frame));
    tx_frame.can_id = frame->id;
    tx_frame.len = frame->data_len;
    memcpy(tx_frame.data, frame->data, frame->data_len);

    const ssize_t nbytes = write(ins->fd, &tx_frame, sizeof(tx_frame));
    if (nbytes < 0 || (size_t)nbytes != sizeof(tx_frame))
    {
        return -1;
    }

    return 1;
}

int canardEmboxReceive(CanardEmboxInstance* ins, CanardCANFrame* out_frame, int timeout_msec)
{
    struct pollfd fds;
    memset(&fds, 0, sizeof(fds));
    fds.fd = ins->fd;
    fds.events = POLLIN;

    const int poll_result = poll(&fds, 1, timeout_msec);
    if (poll_result < 0)
    {
        return -1;
    }
    if (poll_result == 0)
    {
        return 0;
    }
    if ((fds.revents & POLLIN) == 0)
    {
        return -1;
    }

    struct can_frame rx_frame;
    const ssize_t nbytes = read(ins->fd, &rx_frame, sizeof(rx_frame));
    if (nbytes < 0 || (size_t)nbytes != sizeof(rx_frame) || rx_frame.len > CAN_MAX_DLEN)
    {
        return -1;
    }

    out_frame->id = rx_frame.can_id;
    out_frame->data_len = rx_frame.len;
    memcpy(out_frame->data, rx_frame.data, rx_frame.len);

    return 1;
}

int canardEmboxGetDeviceFileDescriptor(const CanardEmboxInstance* ins)
{
    return ins->fd;
}
