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

// UDP server task function
void udp_echo_server_task(void *pvParameters)
{
    char addr_str[128];           // Buffer for client IP address
    int addr_family = AF_INET;    // IPv4
    int ip_protocol = IPPROTO_IP; // using IP protocol
    struct sockaddr_in dest_addr; // struct to hold destination address for incoming packets

    while (1)
    {
        // Prepare to receive packets from any IP address via port 4444 (should change to Raspi Address eventually)
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

        // Set a timeout of 10 seconds for receiving data (socket will restart if no data is received within timeout)
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
        struct sockaddr_storage source_addr; // stored the senders address
        socklen_t socklen = sizeof(source_addr);

        uint8_t buffer[sizeof(location_data_t) + 1]; // Buffer for the largest struct -> TODO method for getting largest struct ...

        while (1)
        {
            ESP_LOGI(TAG, "Waiting for data");
            // Attempt to receive packets and handle errors in receiving
            int len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&source_addr, &socklen);
            if (len < 0)
            {
                // On failure to receive data, log error and break loop
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                break;
            }
            else // Received data
            {
                // Log the size of the incoming data
                ESP_LOGI(TAG, "Received %d bytes", len);

                // Determine the type of struct
                struct_types_t struct_type = (struct_types_t)buffer[0]; // First byte indicates the type

                if (struct_type == command)
                {
                    command_t cmd_buffer;
                    memcpy(&cmd_buffer, &buffer[1], sizeof(command_t)); // Copy data into cmd_buffer

                    // Log received values
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1); // Convert IP address to string
                    ESP_LOGI(TAG, "Received command from %s: Direction = %c, Speed = %ld, Stop = %d",
                             addr_str, cmd_buffer.direction, cmd_buffer.speed, cmd_buffer.stop);

                    // Echo the command back
                    int err = sendto(sock, buffer, sizeof(command_t) + 1, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
                    if (err < 0)
                    {
                        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    }
                }
                else if (struct_type == location_data)
                {
                    location_data_t loc_data_buffer;
                    memcpy(&loc_data_buffer, &buffer[1], sizeof(location_data_t)); // Copy data into loc_data_buffer

                    // Log received values
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1); // Convert IP address to string
                    ESP_LOGI(TAG, "Received location data from %s: X = %ld, Y = %ld",
                             addr_str, loc_data_buffer.x, loc_data_buffer.y);

                    // Echo the location data back
                    int err = sendto(sock, buffer, sizeof(location_data_t) + 1, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
                    if (err < 0)
                    {
                        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    }
                }
                else
                {
                    ESP_LOGE(TAG, "Unknown struct type or damaged datagram received");
                }
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
