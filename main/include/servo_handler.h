#ifndef SERVO_HANDLER_H
#define SERVO_HANDLER_H

#include "driver/mcpwm_prelude.h"

#include <stdint.h>
// TODO: check servo movement 0

namespace AutoTuner
{
struct ServoConfig
{
    int      min_pulse_width{700};          // Minimum pulse width in microsecond
    int      max_pulse_width{2300};         // Maximum pulse width in microsecond
    int      default_pulse_width{1500};     // Default pulse width in microsecond
    uint32_t timebase_resolution{1000000U}; // 1MHz, 1us per tick
    uint32_t timebase_period{20000U};       // 20000 ticks, 20ms
    int      servo_gpio{21U};               // GPIO connects to the PWM signal line
};

class ServoHandler
{
    public:
        ServoHandler(const ServoConfig &config=ServoConfig());
        void init(void);
        void rotate(int rotation);

    public:
        static const char   *TAG;

    private:
        const ServoConfig   config;
        mcpwm_cmpr_handle_t comparator;
        int                 last_rotation;
};
}

#endif // SERVO_HANDLER_H
