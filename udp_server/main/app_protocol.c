#include "app_protocol.h"
#include "udp_communication.h"
#include "data_structures.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "app_protocol";

void app_protocol_handle_message(struct sockaddr_storage *source_addr, uint8_t *buffer, int packet_len)
{
    if (packet_len < sizeof(header_t))
    {
        ESP_LOGE(TAG, "Received packet smaller than header size");
        return;
    }

    header_t received_header;
    memcpy(&received_header, buffer, sizeof(header_t));

    ESP_LOGI(TAG, "Received header: Version = %d, Message Type = %d, Flags = %d, Length = %d",
             received_header.version_number, received_header.message_type, received_header.flags, received_header.length);

    if (received_header.flags == ACK_FLAG && received_header.message_type != ACK)
    {
        send_ack(source_addr, received_header.version_number);
    }

    if (packet_len < sizeof(header_t) + received_header.length)
    {
        ESP_LOGE(TAG, "Received packet smaller than expected size");
        return;
    }

    switch (received_header.message_type)
    {
    case ERR:
        ESP_LOGI(TAG, "Recieved ERR message.");
        break;
    case ACK:
        ESP_LOGI(TAG, "Recieved ACK message.");
        break;
    case CONTROL_COMMAND:
        handle_control_command(buffer + sizeof(header_t));
        break;
    case MOVE_TO_COMMAND:
        handle_move_to_command(buffer + sizeof(header_t));
        break;
    default:
        ESP_LOGE(TAG, "Unknown message type received");
        break;
    }
}

void handle_control_command(const uint8_t *payload)
{
    control_command_t cmd_buffer;
    memcpy(&cmd_buffer, payload, sizeof(control_command_t));
    ESP_LOGI(TAG, "Control Command: Direction = %c, Speed = %ld, Stop = %d", cmd_buffer.direction, cmd_buffer.speed, cmd_buffer.stop);
}

void handle_move_to_command(const uint8_t *payload)
{
    move_to_command_t move_buffer;
    memcpy(&move_buffer, payload, sizeof(move_to_command_t));
    ESP_LOGI(TAG, "Move To Command: X = %ld, Y = %ld", move_buffer.x, move_buffer.y);
}

void send_ack(struct sockaddr_storage *source_addr, uint8_t version_number)
{
    header_t ack_header = {version_number, ACK, NACK_FLAG, 0};
    ESP_LOGI(TAG, "Got ACK flag, sending ACK response.");
    udp_send_packet((uint8_t *)&ack_header, sizeof(header_t), source_addr);
}
