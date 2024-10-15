#include "pwm_control.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

// Function to initialize the PWM timer
esp_err_t pwm_init_timer(ledc_timer_bit_t duty_resolution, uint32_t frequency, ledc_timer_t timer_num) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,    // Low-speed mode
        .timer_num = timer_num,               // Use the specified timer number
        .duty_resolution = duty_resolution,   // PWM resolution (bit depth)
        .freq_hz = frequency,                 // PWM frequency
        .clk_cfg = LEDC_AUTO_CLK              // Auto-select clock source
    };
    
    return ledc_timer_config(&ledc_timer);
}

// Function to initialize a PWM channel for a motor
esp_err_t pwm_init_channel(int gpio_num, int channel, int duty) {
    ledc_channel_config_t ledc_channel = {
        .gpio_num = gpio_num,                 // PWM pin for the motor
        .speed_mode = LEDC_LOW_SPEED_MODE,    // Low-speed mode
        .channel = channel,                   // PWM channel
        .timer_sel = LEDC_TIMER_0,            // Use timer 0
        .duty = duty,                         // Initial duty cycle
        .hpoint = 0                           // Default hpoint
    };

    return ledc_channel_config(&ledc_channel);
}
