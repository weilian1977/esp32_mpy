#ifndef _IRMP_IRSND_CONFIG_H_
#define _IRSNDCONFIG_H_

#include "soc/ledc_reg.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

#define F_CPU 0

#define IRSND_TIME          LEDC_TIMER_2
#define IRSND_CHANNEL       LEDC_CHANNEL_0
#define IRSND_SPEED_MODE    LEDC_LOW_SPEED_MODE
#define IRMP_INPUT_PIN      40
#define IRSND_OUTPUT_PIN    42

#if ! defined(IRSND_IR_FREQUENCY)
#define IRSND_IR_FREQUENCY          38000
#endif

#  undef  F_INTERRUPTS
#  define F_INTERRUPTS              (IRSND_IR_FREQUENCY / 2)   // 19000 interrupts per second

void irmp_pin_init(void)
{
    gpio_reset_pin(IRMP_INPUT_PIN);
    gpio_set_direction(IRMP_INPUT_PIN, GPIO_MODE_INPUT);
}

void irsnd_pin_init(uint32_t freq)
{
    ledc_timer_config_t ledc_timer = 
    {
        .duty_resolution = LEDC_TIMER_10_BIT, // resolution of PWM duty
        .freq_hz = freq,                      // frequency of PWM signal
        .speed_mode = IRSND_SPEED_MODE,    // timer mode
        .timer_num = IRSND_TIME,              // timer index
        .clk_cfg = LEDC_AUTO_CLK,             // Auto select the source clock
    };
    // Set configuration of timer0 timer1 for high speed channels
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t irsnd_pwm_channel[1] = 
    {
        {
            .channel    = IRSND_CHANNEL,
            .duty       = 0,
            .gpio_num   = IRSND_OUTPUT_PIN,
            .speed_mode = IRSND_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = IRSND_TIME
        },
    };
    ledc_channel_config(&irsnd_pwm_channel[0]);
}

void irsnd_set_duty(uint32_t duty)
{
    ledc_set_duty(IRSND_SPEED_MODE, IRSND_CHANNEL, duty);
}

#endif // _IRSNDCONFIG_H_