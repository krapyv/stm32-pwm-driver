#include "stm32f411.h"
#include "pwm_driver.h"

void led_init(PWM_HandleTypeDef *pwm)
{
    if (pwm->pin > 15)
        return;

    // enable clock for the GPIOA
    // using global RCC, setting bit 0 to 1
    if (pwm->Port == GPIOA)
    {
        RCC->AHB1ENR |= (1 << 0);
    }
    else if (pwm->Port == GPIOB)
    {
        RCC->AHB1ENR |= (1 << 1);
    }

    // set MODER to 10 (Alternate Function)

    // clear the bits
    // 0x3 = 0011 in binary
    pwm->Port->MODER &= ~(0x3 << (pwm->pin * 2));

    // set the bits
    pwm->Port->MODER |= (0x2 << (pwm->pin * 2));

    // set the alternate function
    // for the bits 0 to 7 -> AFRL
    // for the bits 8 to 15 -> AFRH
    // one pin takes 4 bits inside AFRL/AFRH

    if (pwm->pin <= 7)
    {
        // clear the bits
        //  1111 = 2^3 + 2^2 + 2^1 + 2^0 = 15 = 0xF

        // e.g., pin 5 takes bits 20-23
        pwm->Port->AFRL &= ~(0xF << (pwm->pin * 4));

        // set the bits
        pwm->Port->AFRL |= (pwm->af << (pwm->pin * 4));
    }
    else
    {
        // clear the bits
        //  1111 = 2^3 + 2^2 + 2^1 + 2^0 = 15 = 0xF

        // e.g., pin 5 takes bits 20-23
        pwm->Port->AFRH &= ~(0xF << ((pwm->pin - 8) * 4));

        // set the bits
        pwm->Port->AFRH |= (pwm->af << ((pwm->pin - 8) * 4));
    }
}

void pwm_init(PWM_HandleTypeDef *pwm)
{
    // enable the clock of the timer
    // since we could have TIM2 to TIM5, we need to enable corresponding clock
    if (pwm->Instance == TIM2)
    {
        RCC->APB1ENR |= (1 << 0);
    }
    else if (pwm->Instance == TIM3)
    {
        RCC->APB1ENR |= (1 << 1);
    }
    else if (pwm->Instance == TIM4)
    {
        RCC->APB1ENR |= (1 << 2);
    }
    else if (pwm->Instance == TIM5)
    {
        RCC->APB1ENR |= (1 << 3);
    }

    // set the ARR (Auto-Reload)
    pwm->Instance->ARR = pwm->Period;

    // set the CCR (Capture/Compare)
    pwm->Instance->CCR[pwm->channel - 1] = pwm->Pulse;

    uint32_t pos = ((pwm->channel & 1) == 0) ? 12 : 4;

    // set the Output compare mode to PWM Mode 1 (110) (bits 6:4)
    if (pwm->channel == 1 || pwm->channel == 2)
    {

        // clear the bits
        // 111 = 2^2 + 2^1 + 2^0 = 7 = 0x7
        pwm->Instance->CCMR1 &= ~(0x7 << pos);

        // set the bits
        // 110 = 6
        pwm->Instance->CCMR1 |= (0x6 << pos);
    }
    else if (pwm->channel == 3 || pwm->channel == 4)
    {
        // clear the bits
        // 111 = 2^2 + 2^1 + 2^0 = 7 = 0x7
        pwm->Instance->CCMR2 &= ~(0x7 << pos);

        // set the bits
        // 110 = 6
        pwm->Instance->CCMR2 |= (0x6 << pos);
    }

    uint32_t ccer_shift = (pwm->channel - 1) * 4;
    // enable the capture/compare output
    pwm->Instance->CCER |= (1 << ccer_shift);

    // enable the counter
    pwm->Instance->CR1 |= 1; // (1 << 0) = 1
}

void pwm_set_duty_cycle(PWM_HandleTypeDef *pwm, uint32_t pulse)
{
    pwm->Pulse = pulse;
    pwm->Instance->CCR[pwm->channel - 1] = pulse;
}