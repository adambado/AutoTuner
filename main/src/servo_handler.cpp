#include "servo_handler.h"

#include "esp_err.h"
#include "esp_log.h"

#include "driver/mcpwm_prelude.h"

namespace AutoTuner
{
const char *ServoHandler::TAG = "ServoHandler";

ServoHandler::ServoHandler(const ServoConfig &_config) :
    config(_config),
    comparator(NULL),
    last_rotation(0)
{
    ESP_LOGI(TAG, "Constructor");
}

void ServoHandler::init(void)
{
    ESP_LOGI(TAG, "init");

    mcpwm_timer_handle_t    timer               = NULL;
    mcpwm_timer_config_t    timer_config        =
    {
        .group_id                             = 0,
        .clk_src                              = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz                        = config.timebase_resolution,
        .count_mode                           = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks                         = config.timebase_period,
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

    mcpwm_oper_handle_t     oper                = NULL;
    mcpwm_operator_config_t operator_config     =
    {
        .group_id                             = 0,       // operator must be in the same group to the timer
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &oper));

    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper, timer));

    mcpwm_comparator_config_t comparator_config = {0};
    comparator_config.flags.update_cmp_on_tez = true;

    ESP_ERROR_CHECK(mcpwm_new_comparator(oper, &comparator_config, &comparator));

    mcpwm_gen_handle_t       generator          = NULL;
    mcpwm_generator_config_t generator_config   =
    {
        .gen_gpio_num                         = config.servo_gpio
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(oper, &generator_config, &generator));

    ESP_LOGI(TAG, "MOTOR: Set generator action on timer and compare event");
    // go high on counter empty
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generator,
                                                              MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                                                                           MCPWM_TIMER_EVENT_EMPTY,
                                                                                           MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(generator,
                                                                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                                                                               comparator,
                                                                                               MCPWM_GEN_ACTION_LOW)));

    ESP_LOGI(TAG, "MOTOR: Enable and start timer");
    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
}

void ServoHandler::rotate(int rotation)
{
    ESP_LOGI(TAG, "rotate: %d", rotation);

    if (last_rotation != rotation)
    {
        last_rotation = rotation;
        if (rotation < -100)
        {
            rotation = -100;
        }
        else if (rotation > 100)
        {
            rotation = 100;
        }

        if (rotation == 0)
        {
            rotation = config.default_pulse_width;
        }
        else if (rotation < 0)
        {
            rotation = config.default_pulse_width + ((config.default_pulse_width - config.min_pulse_width) * rotation / 100.0);
        }
        else
        {
            rotation = config.default_pulse_width + ((config.max_pulse_width - config.default_pulse_width) * rotation / 100.0);
        }

        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, rotation));
    }
}
} // namespace AutoTuner
