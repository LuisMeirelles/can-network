//
// Created by meirelles on 2/14/26.
//

// ReSharper disable CppDFAEndlessLoop

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/can.h>
#include <unistd.h>

#include "can.h"

int main(const int argc, const char **argv) {
    if (argc < 2) {
        fprintf(stderr, "An argument is required\nUsage: %s <interface>\n", argv[0]);
        return -1;
    }

    const int socket_fd = start_socket();

    bind_to(socket_fd, argv[1]);

    do {
        const uint8_t data[8] = {0x02, 0x01, 0x0C};
        const uint32_t can_id = 0x7DF;

        CAN_SEND_ARRAY(socket_fd, can_id, data);

        struct can_frame frame;

        can_recv(socket_fd, &frame);

        sleep(1);
    } while (true);

    return EXIT_SUCCESS;
}
