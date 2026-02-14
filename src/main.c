//
// Created by meirelles on 2/14/26.
//

#include <stdbool.h>
#include <stdlib.h>
#include <linux/can.h>
#include <unistd.h>

#include "can.h"

int main(int argc, char **argv) {
    const int socket_fd = start_socket();

    bind_to(socket_fd, "vcan0");

    do {
        struct can_frame frame;

        read(socket_fd, &frame, sizeof(frame));

        continue;
    } while (true);

    return EXIT_SUCCESS;
}
