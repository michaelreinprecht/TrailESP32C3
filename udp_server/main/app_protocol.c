#include "app_protocol.h"
#include "udp_communication.h"
#include "data_structures.h"
#include "esp_log.h"
#include <string.h>

#include "motor_control.h"

static const char *TAG = "app_protocol";
static uint8_t last_received_sequence_number = 255; // Global variable to store last received sequence number
static uint8_t sending_sequence_number = 0;         // Global variable to store the sequence number of the next packet to send
static bool is_first_packet = true;

bool is_sequence_number_valid(uint8_t sequence_number)
{
    if (is_first_packet)
    {
        is_first_packet = false;
        return true;
    }
    // If the new sequence number is greater than the last one, it's valid
    if (sequence_number > last_received_sequence_number)
    {
        return true;
    }

    uint8_t distance = sequence_number - last_received_sequence_number;
    // Special case for wraparound
    if (distance < 127)
    {
        return true;
    }
    // Otherwise, ignore the packet
    return false;
}

void app_protocol_handle_message(struct sockaddr_storage *source_addr, uint8_t *buffer, int packet_len)
{
    if (packet_len < sizeof(header_t))
    {
        ESP_LOGE(TAG, "Received packet smaller than header size");
        return;
    }

    header_t received_header;
    memcpy(&received_header, buffer, sizeof(header_t));

    ESP_LOGI(TAG, "Received header: Version = %d, Message Type = %d, ACK-Flag = %d, Sequence Number = %d, Length = %d",
             received_header.version_number, received_header.message_type, received_header.flags.bits.ack, received_header.sequence_number, received_header.length);

    // Respond with ACK if ACK flag is set (and message type is not ACK)
    if (received_header.flags.bits.ack == 1 && received_header.message_type != ACK)
    {
        send_ack(source_addr, received_header.version_number);
    }

    if (packet_len < sizeof(header_t) + received_header.length)
    {
        ESP_LOGE(TAG, "Received packet smaller than expected size, expected: %d, got: %d", sizeof(header_t) + received_header.length, packet_len);
        return;
    }

    if (is_sequence_number_valid(received_header.sequence_number))
    {
        last_received_sequence_number = received_header.sequence_number;

        switch (received_header.message_type)
        {
        case ERR:
            ESP_LOGI(TAG, "Received ERR message.");
            break;
        case ACK:
            ESP_LOGI(TAG, "Received ACK message.");
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
    else
    {
        ESP_LOGI(TAG, "Ignoring packet with sequence number %d, last received sequence number %d", received_header.sequence_number, last_received_sequence_number);
    }
}

void handle_control_command(const uint8_t *payload)
{
    control_command_t cmd_buffer;
    memcpy(&cmd_buffer, payload, sizeof(control_command_t));
    ESP_LOGI(TAG, "Control Command: Direction = %c, Speed = %ld, Stop = %d", cmd_buffer.direction, cmd_buffer.speed, cmd_buffer.stop);

    switch (cmd_buffer.direction)
    {
    case 0:
        drive_stop();
        printf("Motors stopped\n");
        break;
    case 1:
        drive_forward(255);
        printf("Motors forward\n");
        break;
    case 2:
        drive_forward_diagonally(128, 255);
        printf("Motors forward right\n");
        break;
    case 3:
        drive_right(255, 255);
        printf("Motors right\n");
        break;
    case 4:
        drive_backward_diagonally(128, 255);
        printf("Motors backward right\n");
        break;
    case 5:
        drive_backward(255);
        printf("Motors backward\n");
        break;
    case 6:
        drive_backward_diagonally(255, 128);
        printf("Motors backward left\n");
        break;
    case 7:
        drive_left(255, 255);
        printf("Motors left\n");
        break;
    case 8:
        drive_forward_diagonally(255, 128);
        printf("Motors forward left\n");
        break;
    default:
        ESP_LOGW(TAG, "Unknown direction for Movement Command: Direction = %c, Speed = %ld, Stop = %d", cmd_buffer.direction, cmd_buffer.speed, cmd_buffer.stop);
        break;
    }
}

void handle_move_to_command(const uint8_t *payload)
{
    move_to_command_t move_buffer;
    memcpy(&move_buffer, payload, sizeof(move_to_command_t));
    ESP_LOGI(TAG, "Move To Command: X = %ld, Y = %ld", move_buffer.x, move_buffer.y);
}

void send_ack(struct sockaddr_storage *source_addr, uint8_t version_number)
{
    header_t ack_header;
    ack_header.version_number = version_number;
    ack_header.message_type = ACK;
    ack_header.sequence_number = sending_sequence_number++;
    ack_header.length = 0;
    ack_header.flags.flagstorage = 0; // Clear all flags
    ack_header.flags.bits.ack = 1;    // Set ACK flag
    ESP_LOGI(TAG, "Got ACK flag, sending ACK response.");
    udp_send_packet((uint8_t *)&ack_header, sizeof(header_t), source_addr);
}
