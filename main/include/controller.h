#ifndef AUTOTUNER_CONTROLLER_H
#define AUTOTUNER_CONTROLLER_H

#include "button_handler.h"
#include "led_handler.h"
#include "servo_handler.h"
#include "analog_sound_handler.h"
#include "i2s_sound_handler.h"
#include "network_handler.h"
#include "tuning_handler.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// TODO: PID instead of current logix
// TODO: handle tuning from higher to lower then higher again example: dropd
// TODO: test do not tune during recording

namespace AutoTuner
{
enum class TunerStates
{
    IDLE = 0,
    TUNING,
    TUNED
};

class Controller
{
    public:
        void init();
        void loop();
        void test();

    private:
        void handleButtonState(ButtonStates state);
        void tune();
        void changeState(TunerStates state);
        void handleOutput(PitchStatus status, float freqDiff);
        void setLedTuneState(PitchStatus status);
        void updateLedMachineState();

    public:
        static const char *TAG;

    private:
        TunerStates       state{TunerStates::IDLE};
        int               rotation = 100;

        LedHandler        led;
        ServoHandler      servo;
        // AnalogSoundHandler sound;
        I2SSoundHandler   sound;
        TuningHandler     tuning;
        ButtonHandler     button;
        NetworkHandler    network;
};
}

#endif // AUTOTUNER_CONTROLLER_H
