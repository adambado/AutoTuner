#ifndef ANALOG_SOUND_HANDLER_H
#define ANALOG_SOUND_HANDLER_H

#include "sound_handler_base.h"

#include "driver/adc.h"

#include <stdint.h>

namespace AutoTuner
{
struct AnalogSoundHandlerConfig
{
    uint32_t       sampling_rate{1000};
    adc1_channel_t adc_channel{ADC1_CHANNEL_0};
};

class AnalogSoundHandler: public SoundHandlerBase
{
    public:
        AnalogSoundHandler(const AnalogSoundHandlerConfig &_config=AnalogSoundHandlerConfig());
        void takeSample() override;

    protected:
        void deviceInit(void) override;

    public:
        static const char        *TAG;

    private:
        AnalogSoundHandlerConfig config;
};
}

#endif // ANALOG_SOUND_HANDLER_H
