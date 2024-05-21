#include "servo/servo_ortho.h"
#include <driver/gpio.h>
#include <driver/ledc.h>

#include <string.h>

namespace servo
{

void omega_init(void)
{
    ledc_timer_config_t ledc_timer;
    memset(&ledc_timer, 0, sizeof(ledc_timer_config_t));
        ledc_timer.speed_mode       = LEDC_LOW_SPEED_MODE;
        ledc_timer.timer_num        = LEDC_TIMER_0;
        ledc_timer.duty_resolution  = LEDC_TIMER_10_BIT;
        ledc_timer.freq_hz          = 50;  // Set output frequency at 50 Hz
        ledc_timer.clk_cfg          = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel;
    memset(&ledc_channel, 0, sizeof(ledc_channel));
        ledc_channel.speed_mode     = LEDC_LOW_SPEED_MODE;
        ledc_channel.channel        = LEDC_CHANNEL_0;
        ledc_channel.timer_sel      = LEDC_TIMER_0;
        ledc_channel.intr_type      = LEDC_INTR_DISABLE;
        ledc_channel.gpio_num       = GPIO_NUM_21;
        ledc_channel.duty           = 78; // Set 90 degree
        ledc_channel.hpoint         = 0;
    ledc_channel_config(&ledc_channel);
    printf("Omega initialized\n");
}

void theta_init(void)
{
    ledc_timer_config_t ledc_timer;
    memset(&ledc_timer, 0, sizeof(ledc_timer_config_t));
        ledc_timer.speed_mode       = LEDC_LOW_SPEED_MODE;
        ledc_timer.timer_num        = LEDC_TIMER_1;
        ledc_timer.duty_resolution  = LEDC_TIMER_10_BIT;
        ledc_timer.freq_hz          = 50;  // Set output frequency at 50 Hz
        ledc_timer.clk_cfg          = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel;
    memset(&ledc_channel, 0, sizeof(ledc_channel));
        ledc_channel.speed_mode     = LEDC_LOW_SPEED_MODE;
        ledc_channel.channel        = LEDC_CHANNEL_1;
        ledc_channel.timer_sel      = LEDC_TIMER_1;
        ledc_channel.intr_type      = LEDC_INTR_DISABLE;
        ledc_channel.gpio_num       = GPIO_NUM_22;
        ledc_channel.duty           = 78; // Set 90 degree
        ledc_channel.hpoint         = 0;
    ledc_channel_config(&ledc_channel);
    printf("Theta initialized\n");
}

void set_omega_angle(unsigned int grad)
{
    printf("Setting omega angle: %u\n", grad);
    if(grad > 180)
        return;
    grad /= 2;
    grad += 35;

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, grad);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void set_theta_angle(unsigned int grad)
{
    printf("Setting theta angle: %u\n", grad);
    if(grad > 180)
        return;
    grad /= 2;
    grad += 35;

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, grad);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
}

}
