#ifndef SOUND_HANDLER_BASE_H
#define SOUND_HANDLER_BASE_H

#include <stdint.h>

// TODO: implement sound buffering

namespace AutoTuner
{
enum class PitchStatus
{
    UNKNOWN      = 0,
    FAR_LOWER    = 1,
    CLOSE_LOWER  = 2,
    TUNED        = 3,
    CLOSE_HIGHER = 4,
    FAR_HIGHER   = 5
};

struct SoundHandlerBaseConfig
{
    float    freq_th1          {2};
    float    freq_th2          {5};
    float    ampl_th           {1900};
    uint32_t sampling_frequency{4096};
};

class SoundHandlerBase
{
    public:
        SoundHandlerBase(const SoundHandlerBaseConfig &_config=SoundHandlerBaseConfig());
        void init(void);
        void fftInit(void);
        void process();
        float getFrequency();
        float getAmpl();
        PitchStatus getPitchStatus(float goalFreq);

        virtual void takeSample()                             = 0;

    protected:
        virtual void deviceInit(void)                         = 0;
        void plot(int maxi, float maxm, float maxf);

    public:
        static constexpr uint32_t number_of_samples         = 2048; // Amount of real input samples
        static const char         *TAG;

    protected:
        float                     x1[number_of_samples]     = {0};
        float                     window[number_of_samples] = {0};

        int                       number_of_samples_copy{number_of_samples};

        float                     lastFreq{0};
        float                     lastAmpl{0};
        SoundHandlerBaseConfig    base_config;
};
}

#endif // SOUND_HANDLER_BASE_H
