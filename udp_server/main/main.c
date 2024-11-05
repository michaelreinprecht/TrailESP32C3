#include <stdio.h>
#include <string.h>
#include <sys/param.h>

#include "driver/ledc.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_rom_sys.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "wifi_handler.h"
#include "udp_communication.h"
#include "app_protocol.h"
#include "motor_control.h"
#include "pwm_control.h"

// Define GPIO pins for PWM and direction for two motors
#define MOTOR1_PIN_ENA GPIO_NUM_19 // PWM Pin for speed control of motor 1
#define MOTOR1_PIN_IN1 GPIO_NUM_0  // Pin for IN1 (direction) of motor 1
#define MOTOR1_PIN_IN2 GPIO_NUM_1  // Pin for IN2 (direction) of motor 1
#define MOTOR2_PIN_ENB GPIO_NUM_18 // PWM Pin for speed control of motor 2
#define MOTOR2_PIN_IN1 GPIO_NUM_2  // Pin for IN1 (direction) of motor 2
#define MOTOR2_PIN_IN2 GPIO_NUM_3  // Pin for IN2 (direction) of motor 2

#define PWM_FREQUENCY 5000              // 5 kHz PWM frequency
#define PWM_RESOLUTION LEDC_TIMER_8_BIT // 8-bit resolution (0-255)
#define PWM_CHANNEL_1 LEDC_CHANNEL_0    // PWM channel 0 for motor 1
#define PWM_CHANNEL_2 LEDC_CHANNEL_1    // PWM channel 1 for motor 2

void init_car_steering()
{
    // Define Pins
    motor_init(MOTOR1_PIN_IN1, MOTOR1_PIN_IN2, MOTOR2_PIN_IN1, MOTOR2_PIN_IN2);
    // Setting all pins to Output
    motor_pin_mode(GPIO_MODE_OUTPUT);
    // Set pins to low, so the car doesnt drive on start
    motor_starting_direction(0, 0);

    // Configure Timer 0 for PWM
    esp_err_t err1 = pwm_init_timer(PWM_RESOLUTION, PWM_FREQUENCY, LEDC_TIMER_0);
    if (err1 != ESP_OK)
    {
        printf("Error configuring PWM timer 0: %d\n", err1);
        return;
    }

    // Configure Timer 1 for PWM
    esp_err_t err2 = pwm_init_timer(PWM_RESOLUTION, PWM_FREQUENCY, LEDC_TIMER_1);
    if (err2 != ESP_OK)
    {
        printf("Error configuring PWM timer 1: %d\n", err2);
        return;
    }

    // Initialize PWM channel for motor 1
    err1 = pwm_init_channel(MOTOR1_PIN_ENA, PWM_CHANNEL_1, LEDC_TIMER_0, 0);
    if (err1 != ESP_OK)
    {
        printf("Error configuring PWM channel for motor 1: %d\n", err1);
        return;
    }

    // Initialize PWM channel for motor 2
    err2 = pwm_init_channel(MOTOR2_PIN_ENB, PWM_CHANNEL_2, LEDC_TIMER_1, 0);
    if (err2 != ESP_OK)
    {
        printf("Error configuring PWM channel for motor 2: %d\n", err2);
        return;
    }
}

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
    init_car_steering();

    xTaskCreate(udp_server_task, "udp_server", 4096, NULL, 5, NULL);
}