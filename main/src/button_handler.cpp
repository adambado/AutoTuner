#include "button_handler.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdint.h>
#include <strings.h>

namespace AutoTuner
{
const char *ButtonHandler::TAG = "ButtonHandler";

ButtonHandler::ButtonHandler(uint32_t _long_presslimit, gpio_num_t _button_input_pin) :
    long_press_limit(_long_presslimit),
    button_input_pin(_button_input_pin)
{
    ESP_LOGI(TAG, "Constructor");
}

void ButtonHandler::init(void)
{
    ESP_LOGI(TAG, "init");

    gpio_set_direction(button_input_pin, GPIO_MODE_INPUT);
    gpio_set_pull_mode(button_input_pin, GPIO_PULLUP_ONLY);
}

ButtonStates ButtonHandler::waitForButton(void)
{
    const uint8_t td             = 50;
    const uint8_t longPressLimit = long_press_limit / td;
    uint8_t       longPressCount = 0;

    while (true)
    {
        if (getButtonState())
        {
            longPressCount++;
        }
        else if (longPressCount > 0)
        {
            break;
        }

        vTaskDelay(td / portTICK_PERIOD_MS);
    }

    return longPressCount >= longPressLimit ? ButtonStates::PRESSED_LONG : ButtonStates::PRESSED;
}

bool ButtonHandler::getButtonState(void)
{
    return !gpio_get_level(button_input_pin);
}
}
