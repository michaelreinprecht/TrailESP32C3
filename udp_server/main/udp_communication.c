// udp_communication.c

#include "udp_communication.h"
#include <string.h>
#include <sys/param.h>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "protocol_examples_common.h"
#include "data_structures.h" // Include for your data structures

#define PORT 4444

static const char *TAG = "udp_echo_server";

/*
UDP: Kontrolliert Socket und schickt Packete. Erhält nur z.B. Adresse und Byte pointer zum versenden ...
Beim empfangen empfängt UDP nur Packete von bestimmter Adresse und Port und gibt diese zum verarbeiten an AppLayer weiter.
Application Layer Protocol: Bauen oder Zerlegen von Packeten und weiterleiten an unterliegende Schicht
*/

// Function to send an acknowledgment response to the sender
void send_ack(int sock, struct sockaddr_storage *source_addr, uint8_t version_number)
{
    header_t ack_header;
    ack_header.version_number = version_number; // Use the same version number as the incoming packet
    ack_header.message_type = ACK;              // Set message type to ACK
    ack_header.flags = NACK_FLAG;               // No ACK needed when sending an ACK
    ack_header.length = 0;                      // No payload for ACK

    // Send the ACK header back to the sender
    int err = sendto(sock, &ack_header, sizeof(header_t), 0, (struct sockaddr *)source_addr, sizeof(*source_addr));
    if (err < 0)
    {
        ESP_LOGE(TAG, "Error occurred during sending ACK: errno %d", errno);
    }
}

// Function to echo back the message to the sender
void echo_message(int sock, struct sockaddr_storage *source_addr, header_t *header, uint8_t *payload, int payload_len)
{
    // Allocate a buffer for the response that includes both the header and the payload
    uint8_t *response_buffer = (uint8_t *)malloc(sizeof(header_t) + payload_len);
    if (response_buffer)
    {
        // Copy the header and payload into the response buffer
        memcpy(response_buffer, header, sizeof(header_t));
        if (payload && payload_len > 0)
        {
            memcpy(response_buffer + sizeof(header_t), payload, payload_len);
        }

        // Send the response back to the sender
        int err = sendto(sock, response_buffer, sizeof(header_t) + payload_len, 0, (struct sockaddr *)source_addr, sizeof(*source_addr));
        if (err < 0)
        {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        }

        free(response_buffer); // Free the response buffer memory
    }
    else
    {
        ESP_LOGE(TAG, "Failed to allocate memory for response buffer");
    }
}

// UDP server task function
void udp_echo_server_task(void *pvParameters)
{
    char addr_str[128];           // Buffer for client IP address
    int addr_family = AF_INET;    // IPv4
    int ip_protocol = IPPROTO_IP; // using IP protocol
    struct sockaddr_in dest_addr; // struct to hold destination address for incoming packets

    while (1)
    {
        // Prepare to receive packets from any IP address via port 4444
        dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);

        // Attempt to create socket and handle errors in socket creation
        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        // Set a timeout of 10 seconds for receiving data
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

        // Bind the socket to the destination address and specified port
        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0)
        {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        }
        ESP_LOGI(TAG, "Socket bound, port %d", PORT);

        // Enter loop waiting to receive UDP packets
        struct sockaddr_storage source_addr; // stored the sender's address
        socklen_t socklen = sizeof(source_addr);

// Adjust size based on max expected payload
#define MAX_PAYLOAD_SIZE (sizeof(control_command_t) > sizeof(move_to_command_t) ? sizeof(control_command_t) : sizeof(move_to_command_t))

        // Buffer for the entire packet (header + maximum payload size)
        uint8_t packet_buffer[sizeof(header_t) + MAX_PAYLOAD_SIZE];

        while (1)
        {
            ESP_LOGI(TAG, "Waiting for data");
            // Receive the entire packet into packet_buffer
            int packet_len = recvfrom(sock, packet_buffer, sizeof(packet_buffer), 0, (struct sockaddr *)&source_addr, &socklen);
            if (packet_len < 0)
            {
                ESP_LOGE(TAG, "Failed to receive data: errno %d", errno);
                break;
            }

            // Ensure we have received at least the size of the header
            if (packet_len < sizeof(header_t))
            {
                ESP_LOGE(TAG, "Received packet smaller than header size: %d", packet_len);
                continue; // Ignore this packet
            }

            // Parse the header
            header_t received_header;
            memcpy(&received_header, packet_buffer, sizeof(header_t));

            ESP_LOGI(TAG, "Received header: Version = %d, Message Type = %d, Flags = %d, Length = %d",
                     received_header.version_number, received_header.message_type, received_header.flags, received_header.length);

            // If an ack flag is set, send an acknowledgment response, but never send an ack for an ack
            if (received_header.flags == ACK_FLAG && received_header.message_type != ACK)
            {
                ESP_LOGI(TAG, "ACK flag set, sending acknowledgment response");
                send_ack(sock, &source_addr, received_header.version_number);
            }

            // Ensure we received enough data for the expected payload
            if (packet_len < sizeof(header_t) + received_header.length)
            {
                ESP_LOGE(TAG, "Received packet smaller than expected size: expected %d, got %d",
                         sizeof(header_t) + received_header.length, packet_len);
                continue; // Ignore this packet
            }

            // Handle payload based on message type
            switch (received_header.message_type)
            {
            case ERR:
            {
                // TODO: Handle error message
                ESP_LOGE(TAG, "Received error message");
                break;
            }
            case ACK:
            {
                // TODO: Handle acknowledgment message
                ESP_LOGI(TAG, "Received acknowledgment message");
                break;
            }
            case CONTROL_COMMAND:
            {
                control_command_t cmd_buffer;
                memcpy(&cmd_buffer, packet_buffer + sizeof(header_t), sizeof(control_command_t));

                // Log the received command
                inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
                ESP_LOGI(TAG, "Received control command from %s: Direction = %c, Speed = %ld, Stop = %d",
                         addr_str, cmd_buffer.direction, cmd_buffer.speed, cmd_buffer.stop);

                // Echo the command back to the sender
                // echo_message(sock, &source_addr, &received_header, packet_buffer + sizeof(header_t), received_header.length);
                break;
            }
            case MOVE_TO_COMMAND:
            {
                move_to_command_t move_buffer;
                memcpy(&move_buffer, packet_buffer + sizeof(header_t), sizeof(move_to_command_t));

                // Log the received move to command
                inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
                ESP_LOGI(TAG, "Received move to command from %s: X = %ld, Y = %ld",
                         addr_str, move_buffer.x, move_buffer.y);

                // Echo the move to command back to the sender
                // echo_message(sock, &source_addr, &received_header, packet_buffer + sizeof(header_t), received_header.length);
                break;
            }
            default:
                ESP_LOGE(TAG, "Unknown message type or damaged datagram received");
                break;
            }
        }

        if (sock != -1)
        {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}
