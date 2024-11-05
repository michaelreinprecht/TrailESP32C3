#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h" // For esp_rom_delay_us()
#include "motor_control.h"
#include "pwm_control.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Define GPIO pins for PWM and direction for two motors
#define MOTOR1_PIN_ENA GPIO_NUM_19  // PWM Pin for speed control of motor 1
#define MOTOR1_PIN_IN1 GPIO_NUM_0  // Pin for IN1 (direction) of motor 1
#define MOTOR1_PIN_IN2 GPIO_NUM_1  // Pin for IN2 (direction) of motor 1
#define MOTOR2_PIN_ENB GPIO_NUM_18  // PWM Pin for speed control of motor 2
#define MOTOR2_PIN_IN1 GPIO_NUM_2   // Pin for IN1 (direction) of motor 2
#define MOTOR2_PIN_IN2 GPIO_NUM_3   // Pin for IN2 (direction) of motor 2 

#define PWM_FREQUENCY 5000              // 5 kHz PWM frequency
#define PWM_RESOLUTION LEDC_TIMER_8_BIT // 8-bit resolution (0-255)
#define PWM_CHANNEL_1 LEDC_CHANNEL_0    // PWM channel 0 for motor 1
#define PWM_CHANNEL_2 LEDC_CHANNEL_1    // PWM channel 1 for motor 2

void InitDevice()
{
    //Define Pins
    motor_init(MOTOR1_PIN_IN1, MOTOR1_PIN_IN2, MOTOR2_PIN_IN1, MOTOR2_PIN_IN2);
    // Setting all pins to Output
    motor_pin_mode(GPIO_MODE_OUTPUT);
    // Set pins to low, so the car doesnt drive on start
    motor_starting_direction(0, 0);

    // Configure Timer 0 for PWM
    esp_err_t err1 = pwm_init_timer(PWM_RESOLUTION, PWM_FREQUENCY, LEDC_TIMER_0);
    if (err1 != ESP_OK) {
        printf("Error configuring PWM timer 0: %d\n", err1);
        return;
    }

    // Configure Timer 1 for PWM
    esp_err_t err2 = pwm_init_timer(PWM_RESOLUTION, PWM_FREQUENCY, LEDC_TIMER_1);
    if (err2 != ESP_OK) {
        printf("Error configuring PWM timer 1: %d\n", err2);
        return;
    }

    // Initialize PWM channel for motor 1
    err1 = pwm_init_channel(MOTOR1_PIN_ENA, PWM_CHANNEL_1, LEDC_TIMER_0, 0);
    if (err1 != ESP_OK) {
        printf("Error configuring PWM channel for motor 1: %d\n", err1);
        return;
    }

    // Initialize PWM channel for motor 2
    err2 = pwm_init_channel(MOTOR2_PIN_ENB, PWM_CHANNEL_2, LEDC_TIMER_1, 0);
    if (err2 != ESP_OK) {
        printf("Error configuring PWM channel for motor 2: %d\n", err2);
        return;
    }
}

void app_main(void)
{
    InitDevice();

    int8_t movement = 0;
    while (1)
    {
        //UDP Message here

        switch (movement)
        {
            case 0:
                drive_stop();
                printf("Motors stopped\n");
                break;
            case 1:
                drive_forward(PWM_CHANNEL_1, PWM_CHANNEL_2, 255);
                printf("Motors forward\n");
                break;  
            case 2:
                drive_forward_diagonally(PWM_CHANNEL_1, PWM_CHANNEL_2, 128, 255);
                printf("Motors forward right\n");
                break;
            case 3:
                drive_right(PWM_CHANNEL_1, PWM_CHANNEL_2, 255, 255);
                printf("Motors right\n");
                break;
            case 4:
                drive_backward_diagonally(PWM_CHANNEL_1, PWM_CHANNEL_2, 128, 255);
                printf("Motors backward right\n");
                break;
            case 5:
                drive_backward(PWM_CHANNEL_1, PWM_CHANNEL_2, 255);
                printf("Motors backward\n");
                break;
            case 6:
                drive_backward_diagonally(PWM_CHANNEL_1, PWM_CHANNEL_2, 255, 128);
                printf("Motors backward left\n");
                break;
            case 7:
                drive_left(PWM_CHANNEL_1, PWM_CHANNEL_2, 255, 255);
                printf("Motors left\n");
                break;
            case 8:
                drive_forward_diagonally(PWM_CHANNEL_1, PWM_CHANNEL_2, 255, 128);
                printf("Motors forward left\n");
                break;      
            default:
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(5000)); // Delay for 5000 ms
        movement = (movement + 1) % 9; 
    }  
}
