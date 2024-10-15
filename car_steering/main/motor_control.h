#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "driver/gpio.h"
#include "driver/ledc.h"
#include <stdint.h>

// Motor GPIO Pin Definitions
extern int8_t MOTOR1_PIN_1, MOTOR1_PIN_2, MOTOR2_PIN_1, MOTOR2_PIN_2;

// Function Prototypes

// Initializes motor pins
void motor_init(int8_t motor1_pin_1, int8_t motor1_pin_2, int8_t motor2_pin_1, int8_t motor2_pin_2);

// Configures motor pins as input/output mode
void motor_pin_mode(gpio_mode_t mode);

// Sets the starting direction for the motors
void motor_starting_direction(int8_t in1_level, int8_t in2_level);

// Stops both motors
void drive_stop(void);

// Drives both motors forward at the specified duty cycle
void drive_forward(int8_t channel1, int8_t channel2, int8_t duty_cycle_motor);

// Turns the vehicle right by adjusting the duty cycle of the motors
void drive_right(int8_t channel1, int8_t channel2, int8_t duty_cycle_motor1, int8_t duty_cycle_motor2);

// Drives both motors backward at the specified duty cycle
void drive_backward(int8_t channel1, int8_t channel2, int8_t duty_cycle_motor);

// Turns the vehicle left by adjusting the duty cycle of the motors
void drive_left(int8_t channel1, int8_t channel2, int8_t duty_cycle_motor1, int8_t duty_cycle_motor2);

// Moves the vehicle forward and turns right simultaneously
void drive_forward_diagonally(int8_t channel1, int8_t channel2, int8_t duty_cycle_motor1, int8_t duty_cycle_motor2);

// Moves the vehicle backward and turns right simultaneously
void drive_backward_diagonally(int8_t channel1, int8_t channel2, int8_t duty_cycle_motor1, int8_t duty_cycle_motor2);

#endif // MOTOR_CONTROL_H
