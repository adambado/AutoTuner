#include "controller.h"

namespace AutoTuner
{
const char *AutoTuner::Controller::TAG = "AutoTuner";

void Controller::init()
{
    ESP_LOGI(TAG, "init");

    led.init();
    servo.init();

    servo.rotate(100);
    sound.init();
    servo.rotate(0);

    button.init();
    if (!button.getButtonState())
    {
        network.init();
        network.setTuningHandler(&tuning);
    }
}

void Controller::test()
{
    // ledtest
    led.blink(3, 300, LedDuty{1, 0, 0});
    led.blink(3, 300, LedDuty{0, 1, 0});
    led.blink(3, 300, LedDuty{0, 0, 1});

    // buttontest
    ButtonStates state = button.waitForButton();
    ESP_LOGI(Controller::TAG, "Button state: %d", (int)state);

    // soundtest
    for (int i = 0; i < 1000; i++)
    {
        sound.takeSample();
        sound.process();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    // servotest
    servo.init();
    servo.rotate(100);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    servo.rotate(-100);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    servo.rotate(0);
}

void Controller::loop()
{
    ESP_LOGI(TAG, "Start of loop");
    updateLedMachineState();

    while (true)
    {
        switch (state)
        {
            case TunerStates::IDLE:
            {
                handleButtonState(button.waitForButton());
                tuning.setSelection(network.getInstrument(), network.getTuning());
                break;
            }
            case TunerStates::TUNING:
            {
                tune();
                ESP_LOGI(TAG, "TUNING");
                break;
            }
            case TunerStates::TUNED:
            {
                ESP_LOGI(TAG, "TUNED");
                handleButtonState(button.waitForButton());
                break;
            }
            default:
            {
                ESP_LOGI(TAG, "UNKNOWN STATE");
                break;
            }
        }
    }

    ESP_LOGI(AutoTuner::Controller::TAG, "END OF LOOP");
}

void Controller::handleButtonState(ButtonStates state)
{
    switch (state)
    {
        case ButtonStates::PRESSED:
        {
            changeState(TunerStates::TUNING);
            break;
        }
        case ButtonStates::PRESSED_LONG:
        {
            changeState(TunerStates::IDLE);
            break;
        }
        default:
        {
            break;
        }
    }
}

void Controller::changeState(TunerStates state)
{
    this->state = state;
    updateLedMachineState();
}

void Controller::updateLedMachineState()
{
    ESP_LOGI(TAG, "updateLedMachineState");

    switch (state)
    {
        case TunerStates::IDLE:
        {
            led.blink(3, 300, LedDuty{0, 0, 1});
            break;
        }
        case TunerStates::TUNING:
        {
            led.blink(3, 300, LedDuty{1, 0, 0});
            break;
        }
        case TunerStates::TUNED:
        {
            led.setDuty(LedDuty{0, 1, 0});
            break;
        }
        default:
        {
            break;
        }
    }
}

void Controller::setLedTuneState(PitchStatus status)
{
    switch (status)
    {
        case PitchStatus::FAR_LOWER:
        case PitchStatus::FAR_HIGHER:
        {
            led.setDuty(LedDuty{1, 0, 0});
            break;
        }
        case PitchStatus::CLOSE_LOWER:
        case PitchStatus::CLOSE_HIGHER:
        {
            led.setDuty(LedDuty{1, 1, 0});
            break;
        }
        case PitchStatus::TUNED:
        {
            led.setDuty(LedDuty{0, 1, 0});
            break;
        }
        default:
        {
            break;
        }
    }
}

void Controller::handleOutput(PitchStatus status, float freqDiff)
{
    ESP_LOGI(TAG, "handleOutput %d", (int)status);

    // rotation = (freqDiff < 0 ? fmax(freqDiff, -20.0) : fmin(freqDiff, 20.0)) / 20.0 * 100.0;

    if (status != PitchStatus::UNKNOWN)
    {
        tuning.updateFrequency(sound.getFrequency());
    }

    switch (status)
    {
        case PitchStatus::FAR_LOWER:
        {
            rotation = 100;
            servo.rotate(1 * rotation);
            break;
        }
        case PitchStatus::CLOSE_LOWER:
        {
            rotation = 50;
            servo.rotate(1 * rotation);
            break;
        }
        case PitchStatus::FAR_HIGHER:
        {
            rotation = 100;
            servo.rotate(-1 * rotation);
            break;
        }
        case PitchStatus::CLOSE_HIGHER:
        {
            rotation = 50;
            servo.rotate(-1 * rotation);
            break;
        }
        default:
        {
            servo.rotate(0);
            break;
        }
    }

    setLedTuneState(status);
}

void Controller::tune()
{
    ESP_LOGI(TAG, "tune");

    size_t inst_id = tuning.current_selection.instrument;
    size_t tune_id = tuning.current_selection.tuning;

    for (int i = 0; i < tuning.instruments[tuning.current_selection.instrument].number_of_strings; i++)
    {
        ESP_LOGI(TAG, "Tuning string %d to %f Hz", i + 1, tuning.instruments[inst_id].tunings[tune_id].freq[i]);
        tuning.setCurrentString(i);

        if (state == TunerStates::IDLE)
        {
            break;
        }

        while (true)
        {
            sound.takeSample();
            sound.process();
            handleOutput(sound.getPitchStatus(tuning.instruments[inst_id].tunings[tune_id].freq[i]),
                         tuning.instruments[inst_id].tunings[tune_id].freq[i] - sound.getFrequency());

            if (button.getButtonState())
            {
                handleButtonState(button.waitForButton());
                break;
            }
        }
    }
    tuning.setCurrentString(0);
}
}
