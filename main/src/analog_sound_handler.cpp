#include "analog_sound_handler.h"

#include "esp_dsp.h"
#include "esp_timer.h"
#include "driver/adc.h"
#include "esp_err.h"

#include "fastmath.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdint.h>

namespace AutoTuner
{
const char *AnalogSoundHandler::TAG = "AnalogSoundHandler";

AnalogSoundHandler::AnalogSoundHandler(const AnalogSoundHandlerConfig &_config) :
    SoundHandlerBase(),
    config{_config}
{
    ESP_LOGI(TAG, "Constructor");
}

void AnalogSoundHandler::takeSample()
{
    int saved_time = 0;

    for (int i = 0; i < number_of_samples_copy; i++)
    {
        x1[i]      = adc1_get_raw(config.adc_channel);
        saved_time = esp_timer_get_time();

        while (esp_timer_get_time() - saved_time < 1000000 / config.sampling_rate)
        {
        }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void AnalogSoundHandler::deviceInit(void)
{
    ESP_LOGI(TAG, "deviceInit");

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(config.adc_channel, ADC_ATTEN_DB_11);
}
}
