#ifndef PWM_CONTROL_H
#define PWM_CONTROL_H

#include "driver/ledc.h"
#include "esp_err.h"

// Function to initialize PWM timer with specified parameters
esp_err_t pwm_init_timer(ledc_timer_bit_t duty_resolution, uint32_t frequency, ledc_timer_t timer_num);

// Function to initialize PWM channel for a motor
esp_err_t pwm_init_channel(int gpio_num, int channel, int duty);

#endif // PWM_CONTROL_H
