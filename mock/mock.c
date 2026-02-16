// ecu_fake_rpm.c
// Fake ECU responding to OBD-II Mode 01 PID 0C (Engine RPM) over SocketCAN (CAN_RAW)

#define _GNU_SOURCE
#include <errno.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

static volatile sig_atomic_t g_stop = 0;

static void on_sigint(int sig) {
    (void)sig;
    g_stop = 1;
}

static int open_can_raw(const char *ifname) {
    int fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd < 0) {
        perror("socket(PF_CAN)");
        return -1;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);

    if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl(SIOCGIFINDEX)");
        close(fd);
        return -1;
    }

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind(AF_CAN)");
        close(fd);
        return -1;
    }

    // (Opcional) Você pode decidir se quer receber as próprias msgs.
    // Para ECU fake, tanto faz. Vou deixar loopback on por padrão.

    return fd;
}

static bool is_obd_rpm_request(const struct can_frame *f) {
    // OBD-II single frame (ISO-TP simplificado via CAN 8 bytes)
    // Request comum:
    //  Byte0: 0x02 (2 bytes de dado: Mode + PID)
    //  Byte1: 0x01 (Mode 01: current data)
    //  Byte2: 0x0C (PID 0C: Engine RPM)
    //  Rest: padding 0x00
    if (f->len != 8) return false;

    const uint8_t *d = f->data;
    if (d[0] != 0x02) return false;
    if (d[1] != 0x01) return false;
    if (d[2] != 0x0C) return false;

    return true;
}

static void build_rpm_response(struct can_frame *out, uint16_t rpm) {
    // Response OBD-II:
    // Byte0: 0x04 (4 bytes de dado: 41 0C A B)
    // Byte1: 0x41 (Mode 01 response = 0x40 + 0x01)
    // Byte2: 0x0C (PID)
    // Byte3: A
    // Byte4: B
    // RPM = ((A*256) + B) / 4  => raw = rpm * 4
    uint16_t raw = (uint16_t)(rpm * 4);
    uint8_t A = (uint8_t)((raw >> 8) & 0xFF);
    uint8_t B = (uint8_t)(raw & 0xFF);

    memset(out, 0, sizeof(*out));
    out->can_id = 0x7E8; // resposta típica do ECM
    out->len    = 8;

    out->data[0] = 0x04;
    out->data[1] = 0x41;
    out->data[2] = 0x0C;
    out->data[3] = A;
    out->data[4] = B;
    out->data[5] = 0x00;
    out->data[6] = 0x00;
    out->data[7] = 0x00;
}

static int set_filters(int fd) {
    // Só aceita 0x7DF (functional) e 0x7E0 (ECM request típico)
    struct can_filter filters[2];
    filters[0].can_id   = 0x7DF;
    filters[0].can_mask = CAN_SFF_MASK;

    filters[1].can_id   = 0x7E0;
    filters[1].can_mask = CAN_SFF_MASK;

    if (setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, &filters, sizeof(filters)) < 0) {
        perror("setsockopt(CAN_RAW_FILTER)");
        return -1;
    }
    return 0;
}

int main(int argc, char **argv) {
    const char *ifname = "vcan0";
    uint16_t rpm = 3000;

    // args: ecu_fake_rpm [ifname] [rpm]
    if (argc >= 2) ifname = argv[1];
    if (argc >= 3) {
        long v = strtol(argv[2], NULL, 10);
        if (v < 0) v = 0;
        if (v > 16383) v = 16383; // limite razoável (raw 65532 /4)
        rpm = (uint16_t)v;
    }

    signal(SIGINT, on_sigint);

    int fd = open_can_raw(ifname);
    if (fd < 0) return 1;

    if (set_filters(fd) != 0) {
        close(fd);
        return 1;
    }

    printf("Fake ECU RPM running on %s. Responding RPM=%u. Ctrl+C to stop.\n", ifname, rpm);

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;

    while (!g_stop) {
        int pr = poll(&pfd, 1, 500); // 500ms
        if (pr < 0) {
            if (errno == EINTR) continue;
            perror("poll");
            break;
        }
        if (pr == 0) continue; // timeout

        if (pfd.revents & POLLIN) {
            struct can_frame req;
            ssize_t n = read(fd, &req, sizeof(req));
            if (n < 0) {
                if (errno == EINTR) continue;
                perror("read");
                break;
            }
            if (n != (ssize_t)sizeof(req)) {
                fprintf(stderr, "Short read: %zd\n", n);
                continue;
            }

            // ID 0x7DF/0x7E0 com payload 01 0C => responde
            if (is_obd_rpm_request(&req)) {
                struct can_frame resp;
                build_rpm_response(&resp, rpm);

                ssize_t w = write(fd, &resp, sizeof(resp));
                if (w != (ssize_t)sizeof(resp)) {
                    perror("write");
                } else {
                    // log leve
                    // printf("REQ %03X -> RESP %03X RPM=%u\n", req.can_id, resp.can_id, rpm);
                }
            }
        }
    }

    close(fd);
    printf("Stopped.\n");
    return 0;
}

