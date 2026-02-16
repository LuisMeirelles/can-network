//
// Created by meirelles on 2/15/26.
//

#include <stdint.h>
#include <linux/can.h>

#include "obd.h"
#include "can.h"

#define BROADCAST_ID 0x7DF

uint16_t get_rpm(const int socket_fd) {
    const uint8_t data[8] = {0x02, 0x01, 0x0C};
    const uint32_t can_id = BROADCAST_ID;

    CAN_SEND_ARRAY(socket_fd, can_id, data);

    struct can_frame frame;

    can_recv(socket_fd, &frame);

    const uint8_t a = frame.data[3];
    const uint8_t b = frame.data[4];

    /// (256A + B) / 4
    return (a << 6) + (b >> 2);
}
