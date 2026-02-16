//
// Created by meirelles on 2/14/26.
//

// ReSharper disable CppDFAEndlessLoop

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "can.h"
#include "io/obd.h"

int main(const int argc, const char **argv) {
    if (argc < 2) {
        fprintf(stderr, "An argument is required\nUsage: %s <interface>\n", argv[0]);
        return -1;
    }

    const int socket_fd = start_socket();

    bind_to(socket_fd, argv[1]);

    do {
        const unsigned short rpm = get_rpm(socket_fd);

        printf("RPM: %hu\n", rpm);
        fflush(stdout);

        sleep(1);
    } while (true);

    return EXIT_SUCCESS;
}
