//
// Created by meirelles on 2/14/26.
//

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
        const uint8_t data[] = {0xCD};
        const uint32_t can_id = 0x1FF;

        can_send(socket_fd, can_id, data);

        struct can_frame frame = {
            .can_id = 0x1FF,
            .data = 0xAB,
            .len = 1
        };

        write(socket_fd, &frame, sizeof(frame));

        struct can_frame frame2;

        read(socket_fd, &frame2, sizeof(frame2));

        continue;

        // ReSharper disable once CppDFAEndlessLoop
    } while (true);

    return EXIT_SUCCESS;
}
