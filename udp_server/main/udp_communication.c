#include "udp_communication.h"
#include <string.h>
#include <sys/param.h>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "esp_log.h"
#include "protocol_examples_common.h"

#define PORT 4444

static const char *TAG = "udp_layer";
static int udp_socket = -1; // Static variable for the UDP socket

// Initialize the UDP socket and bind to the specified port
int udp_init_socket(void)
{
    ESP_LOGI("UDP", "Socket created");
    // if (udp_socket != -1)
    // {
    //     return udp_socket; // If the socket is already initialized, return it
    // }

    udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (udp_socket < 0)
    {
        ESP_LOGE("UDP", "Unable to create socket: errno %d", errno);
        return -1;
    }

    ESP_LOGI("UDP", "Socket created");

    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);

    int err = bind(udp_socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err < 0)
    {
        ESP_LOGE("UDP", "Socket unable to bind: errno %d", errno);
        close(udp_socket);
        udp_socket = -1;
        return -1;
    }

    ESP_LOGI("UDP", "Socket bound, port %d", PORT);
    return udp_socket;
}

// Receive a UDP packet and return its size
int udp_receive_packet(uint8_t *buffer, int buffer_size, struct sockaddr_storage *source_addr)
{
    socklen_t socklen = sizeof(*source_addr);
    int packet_len = recvfrom(udp_socket, buffer, buffer_size, 0, (struct sockaddr *)source_addr, &socklen);
    if (packet_len < 0)
    {
        ESP_LOGE("UDP", "Failed to receive data: errno %d", errno);
    }
    return packet_len;
}

// Send a UDP packet to a specified destination address
int udp_send_packet(const uint8_t *data, int data_size, const struct sockaddr_storage *dest_addr)
{
    int err = sendto(udp_socket, data, data_size, 0, (struct sockaddr *)dest_addr, sizeof(*dest_addr));
    if (err < 0)
    {
        ESP_LOGE("UDP", "Error occurred during sending: errno %d", errno);
        return -1;
    }
    return err;
}

// Close the UDP socket
void udp_close_socket(void)
{
    if (udp_socket != -1)
    {
        close(udp_socket);
        udp_socket = -1;
        ESP_LOGI("UDP", "Socket closed");
    }
}