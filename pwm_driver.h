#ifndef PWM_DRIVER_H
#define PWM_DRIVER_H

#include <stdint.h>
#include "stm32f411.h"

typedef struct
{
    TIM_RegDef_t *Instance; // timer
    GPIO_RegDef_t *Port;    // gpio
    uint32_t pin;
    GPIO_AF_t af;
    uint32_t Period;
    uint32_t Pulse; // duty cycle
    uint32_t channel;
} PWM_HandleTypeDef;

void led_init(PWM_HandleTypeDef *pwm);
void pwm_init(PWM_HandleTypeDef *pwm);
void pwm_set_duty_cycle(PWM_HandleTypeDef *pwm, uint32_t pulse);

#endif