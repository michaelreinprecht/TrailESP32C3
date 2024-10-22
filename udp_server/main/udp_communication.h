#ifndef UDP_COMMUNICATION_H
#define UDP_COMMUNICATION_H

#include <stdint.h>
#include <sys/socket.h>
#include <lwip/netdb.h>

// Function prototypes
int udp_init_socket(void);
int udp_receive_packet(uint8_t *buffer, int buffer_size, struct sockaddr_storage *source_addr);
int udp_send_packet(const uint8_t *data, int data_size, const struct sockaddr_storage *dest_addr);
void udp_close_socket(void);

#endif // UDP_COMMUNICATION_H
