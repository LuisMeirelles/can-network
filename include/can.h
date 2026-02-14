//
// Created by meirelles on 2/14/26.
//

#ifndef CAN_NETWORK_CAN_H
#define CAN_NETWORK_CAN_H

int start_socket(void);

void bind_to(int socket_fd, const char *iface_name);

#endif //CAN_NETWORK_CAN_H