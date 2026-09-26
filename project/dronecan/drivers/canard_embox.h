#ifndef PROJECT_DRONECAN_DRIVERS_CAN_EMBOX_H
#define PROJECT_DRONECAN_DRIVERS_CAN_EMBOX_H

#include <canard.h>
#include <stdint.h>

typedef struct
{
    int fd;
} CanardEmboxInstance;

int canardEmboxInit(CanardEmboxInstance* out_ins, const char* can_iface_name);

int canardEmboxClose(CanardEmboxInstance* ins);

int canardEmboxTransmit(CanardEmboxInstance* ins, const CanardCANFrame* frame, int timeout_msec);

int canardEmboxReceive(CanardEmboxInstance* ins, CanardCANFrame* out_frame, int timeout_msec);

int canardEmboxGetDeviceFileDescriptor(const CanardEmboxInstance* ins);

#endif
