#include "led_handler.h"

#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// --------------------------------------------------------------------------------------------

namespace AutoTuner
{
const char *LedHandler::TAG = "LedDuty";

void LedDuty::normalize()
{
    double sum = r + g + b;

    if (sum > 1.0)
    {
        r /= sum;
        g /= sum;
        b /= sum;
    }
}

LedHandler::LedHandler(const LedConfig &_config, LedDuty _duty) : config(_config), duty(_duty)
{
    ESP_LOGI(TAG, "Constructor");
}

void LedHandler::init(void)
{
    ESP_LOGI(TAG, "init");
    // ledc_init();

    ledc_timer_config_t   timer    =
    {
        .speed_mode      = config.ledc_mode,
        .duty_resolution = config.duty_resolution,
        .timer_num       = config.ledc_timer,
        .freq_hz         = config.frequency,                   // Set output frequency at 4 kHz
        .clk_cfg         = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer));

    ledc_channel_config_t r_config =
    {
        .gpio_num        = config.red_gpio,
        .speed_mode      = config.ledc_mode,
        .channel         = config.red_channel,
        .intr_type       = LEDC_INTR_DISABLE,
        .timer_sel       = config.ledc_timer,
        .duty            = 0,
        .hpoint          = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&r_config));

    ledc_channel_config_t g_config =
    {
        .gpio_num        = config.green_gpio,
        .speed_mode      = config.ledc_mode,
        .channel         = config.green_channel,
        .intr_type       = LEDC_INTR_DISABLE,
        .timer_sel       = config.ledc_timer,
        .duty            = 0,
        .hpoint          = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&g_config));

    ledc_channel_config_t b_config =
    {
        .gpio_num        = config.blue_gpio,
        .speed_mode      = config.ledc_mode,
        .channel         = config.blue_channel,
        .intr_type       = LEDC_INTR_DISABLE,
        .timer_sel       = config.ledc_timer,
        .duty            = 0,
        .hpoint          = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&b_config));
}

void LedHandler::setDuty(const LedDuty &_duty)
{
    duty = _duty;
    duty.normalize();
    // ESP_LOGI(TAG, "setDuty %f %f %f", _duty.r, _duty.g, _duty.b);
    // ESP_LOGI(TAG, "ledc_duty %lu", config.ledc_duty);

    ESP_ERROR_CHECK(ledc_set_duty(config.ledc_mode, config.red_channel, config.ledc_duty * duty.r));
    ESP_ERROR_CHECK(ledc_set_duty(config.ledc_mode, config.green_channel, config.ledc_duty * duty.g));
    ESP_ERROR_CHECK(ledc_set_duty(config.ledc_mode, config.blue_channel, config.ledc_duty * duty.b));

    ESP_ERROR_CHECK(ledc_update_duty(config.ledc_mode, config.red_channel));
    ESP_ERROR_CHECK(ledc_update_duty(config.ledc_mode, config.green_channel));
    ESP_ERROR_CHECK(ledc_update_duty(config.ledc_mode, config.blue_channel));
}

void LedHandler::blink(uint32_t times, uint32_t delay, const LedDuty &_duty)
{
    for (uint32_t i = 0; i < times; i++)
    {
        setDuty(_duty);
        vTaskDelay(delay / portTICK_PERIOD_MS);
        setDuty(LedDuty{0, 0, 0});
        vTaskDelay(delay / portTICK_PERIOD_MS);
    }
}
}
