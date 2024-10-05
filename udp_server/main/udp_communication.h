// udp_communication.h

#ifndef UDP_COMMUNICATION_H
#define UDP_COMMUNICATION_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// Port definition
#define PORT 4444

// Function to start the UDP server task
void udp_echo_server_task(void *pvParameters);

#endif // UDP_COMMUNICATION_H
