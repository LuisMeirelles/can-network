//
// Created by meirelles on 2/14/26.
//

#include <arpa/inet.h>
#include <linux/can.h>
#include <net/if.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "can.h"

#include <unistd.h>

int start_socket() {
    const int fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (fd == -1) {
        perror("socket()");
        exit(errno);
    }

    return fd;
}

void bind_to(const int socket_fd, const char *iface_name) {
    struct sockaddr_can addr;
    struct ifreq ifr;

    strcpy(ifr.ifr_name, iface_name);
    ioctl(socket_fd, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    const int bind_status = bind(socket_fd, (const struct sockaddr *) &addr, sizeof(addr));

    if (bind_status == -1) {
        perror("bind()");
        exit(errno);
    }
}

void can_send(const int socket_fd, const uint32_t can_id, const uint8_t data[]) {
    const uint8_t len = sizeof(*data);

    const struct can_frame frame = {
        .can_id = can_id,
        .data = *data,
        .len = len
    };

    write(socket_fd, &frame, sizeof(frame));
}
