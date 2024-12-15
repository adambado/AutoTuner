#include "sound_handler_base.h"

#include "esp_dsp.h"
#include "esp_timer.h"
#include "esp_err.h"

#include "fastmath.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdint.h>

namespace AutoTuner
{
const char *SoundHandlerBase::TAG = "SoundHandlerBase";

SoundHandlerBase::SoundHandlerBase(const SoundHandlerBaseConfig &_config) : base_config(_config)
{
}

void SoundHandlerBase::init(void)
{
    ESP_LOGI(TAG, "init");
    number_of_samples_copy = number_of_samples;

    deviceInit();
    fftInit();

    takeSample();
    process();
    base_config.ampl_th    = lastAmpl;
    ESP_LOGI(TAG, "ampl_th %f", lastAmpl);
}

void SoundHandlerBase::fftInit(void)
{
    esp_err_t ret = dsps_fft2r_init_fc32(NULL, number_of_samples_copy >> 1);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Not possible to initialize FFT2R. Error = %i", ret);
    }

    ret = dsps_fft4r_init_fc32(NULL, number_of_samples_copy >> 1);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Not possible to initialize FFT2R. Error = %i", ret);
    }

    dsps_wind_hann_f32(window, number_of_samples_copy);
}

// void dsps_view(const float *data, int32_t len, int width, int height, float min, float max, char view_char)
void SoundHandlerBase::plot(int maxi, float maxm, float maxf)
{
    ESP_LOGW(TAG, "Signal x1");
    dsps_view(x1, number_of_samples / 2, 128, 42, -60, 300, '|');
    ESP_LOGI(TAG, "maxI = %i", maxi);
    ESP_LOGI(TAG, "maxM = %f", maxm);
    ESP_LOGI(TAG, "maxF = %f", maxf);
}

void SoundHandlerBase::process()
{
    int   maxi    = 0;
    float maxm    = -10000000000;
    float maxAmpl = -10000000000;
    float minAmpl = 10000000000;

    // Convert two input vectors to one complex vector
    for (int i = 0; i < number_of_samples_copy; i++)
    {
        x1[i] = x1[i] * window[i];

        if (i > 2)
        {
            if (x1[i] > maxAmpl)
            {
                maxAmpl = x1[i];
            }
            if (x1[i] < minAmpl)
            {
                minAmpl = x1[i];
            }
        }
    }

    lastAmpl = maxAmpl - minAmpl;

    // FFT Radix-2                                          //radix 2 egy eljaras ahol ket n/2-es reszre bontjak
    // unsigned int start_r2 = dsp_get_cpu_cycle_count();    //csak cpu countot nez hogy ki tudja irni mennyi ido volt az fft
    dsps_fft2r_fc32(x1, number_of_samples_copy >> 1);
    // Bit reverse
    dsps_bit_rev2r_fc32(x1, number_of_samples_copy >> 1);

    // Convert one complex vector withreshold lengthreshold N/2 to one real spectrum vector withreshold lengthreshold N/2
    dsps_cplx2real_fc32(x1, number_of_samples_copy >> 1);

    // a magnitude szamitas ahol frekvencia szamolhato az i-bol
    for (int i = 2; i < number_of_samples_copy / 2; i++)
    {
        if (x1[i] > maxm)
        {
            maxi = i;
            maxm = x1[i];
        }
        x1[i] = 10 * log10f((x1[i * 2 + 0] * x1[i * 2 + 0] + x1[i * 2 + 1] * x1[i * 2 + 1] + 0.0000001) / number_of_samples_copy);
    }

    lastFreq = maxi * base_config.sampling_frequency / (2.F * number_of_samples);

    ESP_LOGI(TAG, "lastAmpl %f", lastAmpl);
    ESP_LOGI(TAG, "lastFreq %f", lastFreq);
    ESP_LOGI(TAG, "maxi %d", maxi);
    ESP_LOGI(TAG, "maxm %f", maxm);

    // plot(maxi, maxm, lastFreq);
}

float SoundHandlerBase::getFrequency()
{
    return lastFreq;
}

float SoundHandlerBase::getAmpl()
{
    return lastAmpl;
}

PitchStatus SoundHandlerBase::getPitchStatus(float goalFreq)
{
    PitchStatus result = PitchStatus::UNKNOWN;

    if ((lastFreq > (goalFreq * 0.5)) && (lastFreq < (goalFreq * 3.1)) && (lastAmpl > base_config.ampl_th))
    {
        float newfreq = lastFreq;
        ESP_LOGI(TAG, "lastFreq %f", lastFreq);

        for (int i = 1; i < 4; i++)
        {
            if ((lastFreq / float(i)) < (goalFreq * 1.5))
            {
                newfreq = lastFreq / float(i);
                break;
            }
        }
        ESP_LOGI(TAG, "newfreq %f", newfreq);

        if (newfreq < (goalFreq - base_config.freq_th2))
        {
            result = PitchStatus::FAR_LOWER;
        }
        else if (newfreq < (goalFreq - base_config.freq_th1))
        {
            result = PitchStatus::CLOSE_LOWER;
        }
        else if (newfreq > (goalFreq + base_config.freq_th2))
        {
            result = PitchStatus::FAR_HIGHER;
        }
        else if (newfreq > (goalFreq + base_config.freq_th1))
        {
            result = PitchStatus::CLOSE_HIGHER;
        }
        else
        {
            result = PitchStatus::TUNED;
        }
    }

    return result;
}
}
