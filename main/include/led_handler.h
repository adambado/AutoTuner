#ifndef LED_HANDLING_H
#define LED_HANDLING_H

#include "driver/ledc.h"
#include "esp_timer.h"

#include <stdint.h>

namespace AutoTuner
{
struct LedDuty
{
    void normalize();

    double r;
    double g;
    double b;
};

struct LedConfig
{
    ledc_timer_t     ledc_timer{LEDC_TIMER_0};
    ledc_mode_t      ledc_mode{LEDC_LOW_SPEED_MODE};
    gpio_num_t       red_gpio{GPIO_NUM_17};
    gpio_num_t       green_gpio{GPIO_NUM_16};
    gpio_num_t       blue_gpio{GPIO_NUM_15};
    ledc_channel_t   red_channel{LEDC_CHANNEL_0};
    ledc_channel_t   green_channel{LEDC_CHANNEL_1};
    ledc_channel_t   blue_channel{LEDC_CHANNEL_2};
    ledc_timer_bit_t duty_resolution{LEDC_TIMER_13_BIT}; // Set duty resolution to 13 bits
    uint32_t         ledc_duty{4096};                    // Set duty to 100%. (2 ** 13) - 1 = 8191
    uint32_t         frequency{4000};                    // Frequency in Hertz. Set frequency at 4 kHz
};

class LedHandler
{
    public:
        LedHandler(const LedConfig &_config=LedConfig(), LedDuty _duty=LedDuty());
        void init(void);
        void setDuty(const LedDuty &_duty);
        void blink(uint32_t times, uint32_t delay, const LedDuty &_duty);

    public:
        static const char *TAG;

    private:
        const LedConfig   config;
        LedDuty           duty;
};
}

#endif // LED_HANDLING_H
