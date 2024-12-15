#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include "driver/gpio.h"

#include <stdint.h>
#include <strings.h>

namespace AutoTuner
{
enum class ButtonStates
{
    PRESSED = 0,
    PRESSED_LONG,
};

class ButtonHandler
{
    public:
        ButtonHandler(uint32_t _long_presslimit=1000, gpio_num_t _button_input_pin=GPIO_NUM_13);
        void init(void);
        ButtonStates waitForButton(void);
        bool getButtonState(void);

    public:
        static const char *TAG;

        uint32_t          long_press_limit;

    private:
        gpio_num_t        button_input_pin;
};
}

#endif // BUTTON_HANDLER_H
