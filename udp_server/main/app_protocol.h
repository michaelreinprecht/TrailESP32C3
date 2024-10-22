#ifndef APP_PROTOCOL_H
#define APP_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include <sys/socket.h>

// Function to handle received message
void app_protocol_handle_message(struct sockaddr_storage *source_addr, uint8_t *buffer, int packet_len);

// Functions to handle specific message types
void handle_control_command(const uint8_t *payload);
void handle_move_to_command(const uint8_t *payload);

// Function to send an ACK message
void send_ack(struct sockaddr_storage *source_addr, uint8_t version_number);

#endif // APP_PROTOCOL_H
