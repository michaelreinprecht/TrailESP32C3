#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "driver/gpio.h"

// Initializes motor pins
void motor_init(gpio_num_t motor1_pin_1, gpio_num_t motor1_pin_2, gpio_num_t motor2_pin_1, gpio_num_t motor2_pin_2);

// Configures motor pins as input/output mode
void motor_pin_mode(gpio_mode_t mode);

// Sets the starting direction for the motors
void motor_starting_direction(uint32_t in1_level, uint32_t in2_level);

// Stops both motors
void drive_stop(void);

// Drives both motors forward at the specified duty cycle
void drive_forward(uint8_t channel1, uint8_t channel2, uint8_t duty_cycle_motor);

// Turns the vehicle right by adjusting the duty cycle of the motors
void drive_right(uint8_t channel1, uint8_t channel2, uint8_t duty_cycle_motor1, uint8_t duty_cycle_motor2);

// Drives both motors backward at the specified duty cycle
void drive_backward(uint8_t channel1, uint8_t channel2, uint8_t duty_cycle_motor);

// Turns the vehicle left by adjusting the duty cycle of the motors
void drive_left(uint8_t channel1, uint8_t channel2, uint8_t duty_cycle_motor1, uint8_t duty_cycle_motor2);

// Moves the vehicle forward and turns right simultaneously
void drive_forward_diagonally(uint8_t channel1, uint8_t channel2, uint8_t duty_cycle_motor1, uint8_t duty_cycle_motor2);

// Moves the vehicle backward and turns right simultaneously
void drive_backward_diagonally(uint8_t channel1, uint8_t channel2, uint8_t duty_cycle_motor1, uint8_t duty_cycle_motor2);

#endif // MOTOR_CONTROL_H
