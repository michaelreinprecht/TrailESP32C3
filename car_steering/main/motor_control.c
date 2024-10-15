#include "motor_control.h"

int8_t MOTOR1_PIN_1, MOTOR1_PIN_2, MOTOR2_PIN_1, MOTOR2_PIN_2;


void motor_init(int8_t motor1_pin_1, int8_t motor1_pin_2, int8_t motor2_pin_1, int8_t motor2_pin_2) {
    MOTOR1_PIN_1 = motor1_pin_1;
    MOTOR1_PIN_2 = motor1_pin_2;
    MOTOR2_PIN_1 = motor2_pin_1;
    MOTOR2_PIN_2 = motor2_pin_2;
}

void motor_pin_mode(gpio_mode_t mode){
    gpio_set_direction(MOTOR1_PIN_1, mode);
    gpio_set_direction(MOTOR1_PIN_2, mode);
    gpio_set_direction(MOTOR2_PIN_1, mode);
    gpio_set_direction(MOTOR2_PIN_2, mode);
}

// Function to set motor direction by setting IN1 and IN2 levels
void motor_starting_direction(int8_t in1_level, int8_t in2_level) {
    gpio_set_level(MOTOR1_PIN_1, in1_level);
    gpio_set_level(MOTOR1_PIN_2, in1_level);
    gpio_set_level(MOTOR2_PIN_1, in2_level);
    gpio_set_level(MOTOR2_PIN_2, in2_level);
}

void drive_stop(){
    gpio_set_level(MOTOR1_PIN_1, 0);
    gpio_set_level(MOTOR1_PIN_2, 0);
    gpio_set_level(MOTOR2_PIN_1, 0);
    gpio_set_level(MOTOR2_PIN_2, 0);
}

void drive_forward(int8_t channel1, int8_t channel2, int8_t duty_cycle_motor){
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel1, duty_cycle_motor);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel1);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel2, duty_cycle_motor);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel2);

    gpio_set_level(MOTOR1_PIN_1, 1);
    gpio_set_level(MOTOR1_PIN_2, 0); 
    gpio_set_level(MOTOR2_PIN_1, 1);
    gpio_set_level(MOTOR2_PIN_2, 0); 
}

void drive_right(int8_t channel1, int8_t channel2, int8_t duty_cycle_motor1, int8_t duty_cycle_motor2){
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel1, duty_cycle_motor1);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel1);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel2, duty_cycle_motor2);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel2);

    gpio_set_level(MOTOR1_PIN_1, 0);
    gpio_set_level(MOTOR1_PIN_2, 1); 
    gpio_set_level(MOTOR2_PIN_1, 1);
    gpio_set_level(MOTOR2_PIN_2, 0); 
}

void drive_backward(int8_t channel1, int8_t channel2, int8_t duty_cycle_motor){
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel1, duty_cycle_motor);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel1);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel2, duty_cycle_motor);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel2);

    gpio_set_level(MOTOR1_PIN_1, 0);
    gpio_set_level(MOTOR1_PIN_2, 1); 
    gpio_set_level(MOTOR2_PIN_1, 0);
    gpio_set_level(MOTOR2_PIN_2, 1); 
}

void drive_left(int8_t channel1, int8_t channel2, int8_t duty_cycle_motor1, int8_t duty_cycle_motor2){
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel1, duty_cycle_motor1);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel1);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel2, duty_cycle_motor2);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel2);

    gpio_set_level(MOTOR1_PIN_1, 1);
    gpio_set_level(MOTOR1_PIN_2, 0); 
    gpio_set_level(MOTOR2_PIN_1, 0);
    gpio_set_level(MOTOR2_PIN_2, 1); 
}

void dive_forward_diagonally(int8_t channel1, int8_t channel2, int8_t duty_cycle_motor1, int8_t duty_cycle_motor2){
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel1, duty_cycle_motor1);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel1);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel2, duty_cycle_motor2);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel2);

    gpio_set_level(MOTOR1_PIN_1, 1);
    gpio_set_level(MOTOR1_PIN_2, 0); 
    gpio_set_level(MOTOR2_PIN_1, 1);
    gpio_set_level(MOTOR2_PIN_2, 0); 
}

void drive_backward_diagonally(int8_t channel1, int8_t channel2, int8_t duty_cycle_motor1, int8_t duty_cycle_motor2){
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel1, duty_cycle_motor1);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel1);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel2, duty_cycle_motor2);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel2);

    gpio_set_level(MOTOR1_PIN_1, 0);
    gpio_set_level(MOTOR1_PIN_2, 1); 
    gpio_set_level(MOTOR2_PIN_1, 0);
    gpio_set_level(MOTOR2_PIN_2, 1); 
}