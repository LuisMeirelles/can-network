//
// Created by meirelles on 2/14/26.
//

// ReSharper disable CppDFAEndlessLoop

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <linux/can.h>
#include <unistd.h>

#include "can.h"

int main(int argc, char **argv) {
    const int socket_fd = start_socket();

    bind_to(socket_fd, "vcan0");

    do {
        const uint8_t data[2] = {0xAB, 0xCD};
        const uint32_t can_id = 0x1FF;

        CAN_SEND_ARRAY(socket_fd, can_id, data);

        struct can_frame frame2;

        read(socket_fd, &frame2, sizeof(frame2));

        continue;
    } while (true);

    return EXIT_SUCCESS;
}
