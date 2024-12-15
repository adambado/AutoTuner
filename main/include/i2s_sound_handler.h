#ifndef I2S_SOUND_HANDLER_H
#define I2S_SOUND_HANDLER_H

#include "sound_handler_base.h"

#include "driver/i2s.h"
#include "driver/i2s_std.h"
#include "driver/gpio.h"

#include "esp_dsp.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "fastmath.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

// TODO: check with different sample sizes
// TODO: check i2s byte order compared to docs
// TODO: microphone responsiveness check

namespace AutoTuner
{
struct I2SSoundHandlerConfig
{
    gpio_num_t clock_pin{GPIO_NUM_10};
    gpio_num_t ws_pin{GPIO_NUM_12};
    gpio_num_t in_pin{GPIO_NUM_11};
};

class I2SSoundHandler: public SoundHandlerBase
{
    public:
        I2SSoundHandler(const I2SSoundHandlerConfig &_config=I2SSoundHandlerConfig());
        void takeSample() override;

    protected:
        void deviceInit(void) override;

    public:
        static const char     *TAG;

    private:
        I2SSoundHandlerConfig config;
        i2s_chan_handle_t     rx_chan;
        uint32_t              samples[number_of_samples];
};
}

#endif // I2S_SOUND_HANDLER_H
