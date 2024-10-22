#include <string.h>
#include <sys/param.h>

#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "wifi_handler.h"
#include "udp_communication.h"
#include "app_protocol.h"

// Remove include for datastrucutures ....
// Move this method somewhere else ....?
void udp_server_task(void *pvParameters)
{
    uint8_t packet_buffer[1024];
    struct sockaddr_storage source_addr;

    // Initialize the UDP socket
    if (udp_init_socket() < 0)
    {
        ESP_LOGE("UDP", "Failed to initialize UDP socket");
        vTaskDelete(NULL);
        return;
    }

    while (1)
    {
        int packet_len = udp_receive_packet(packet_buffer, sizeof(packet_buffer), &source_addr);
        if (packet_len > 0)
        {
            // Pass the received packet to the application layer for processing
            app_protocol_handle_message(&source_addr, packet_buffer, packet_len);
        }
    }

    udp_close_socket();
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_static_ip(); // Connect to the Raspberry PI's Wi-Fi

    xTaskCreate(udp_server_task, "udp_server", 4096, NULL, 5, NULL);
}