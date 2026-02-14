//
// Created by meirelles on 2/14/26.
//

#ifndef CAN_NETWORK_CAN_H
#define CAN_NETWORK_CAN_H

int start_socket(void);

void bind_to(int socket_fd, const char *iface_name);

void can_send(int socket_fd, canid_t can_id, const uint8_t data[CAN_MAX_DLEN], int len);

#define CAN_SEND_ARRAY(fd, id, arr) \
    can_send((fd), (id), (arr), (uint8_t)sizeof(arr))

#endif //CAN_NETWORK_CAN_H