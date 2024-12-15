#include "controller.h"

static AutoTuner::Controller mainController;
// AutoTuner::I2SSoundHandler   sound;
// AutoTuner::TuningHandler     tuning;
// AutoTuner::NetworkHandler    network;


extern "C" void app_main()
{
    ESP_LOGI(AutoTuner::Controller::TAG, "app_main");
    // network.setTuningHandler(&tuning);
    // network.init();

    mainController.init();
    // mainController.test();
    mainController.loop();

    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
