//
// Created by meirelles on 2/14/26.
//

#ifndef CAN_NETWORK_CAN_H
#define CAN_NETWORK_CAN_H

#include <stddef.h>

int start_socket(void);

void bind_to(int socket_fd, const char *iface_name);

void can_send(int socket_fd, uint32_t can_id, const uint8_t data[], size_t len);

void can_recv(int socket_fd, struct can_frame *frame);

#define CAN_SEND_ARRAY(fd, id, arr) \
    can_send((fd), (id), (arr), sizeof(arr))

#endif //CAN_NETWORK_CAN_H