#include "i2s_sound_handler.h"


namespace AutoTuner
{
const char *I2SSoundHandler::TAG = "I2SSoundHandler";

I2SSoundHandler::I2SSoundHandler(const I2SSoundHandlerConfig &_config) :
    config{_config}
{
    ESP_LOGI(TAG, "Constructor");
}

void I2SSoundHandler::takeSample()
{
    size_t  r_bytes = 0;
    uint8_t *bytes  = (uint8_t *)samples;

    if (i2s_channel_read(rx_chan, samples, number_of_samples * sizeof(int32_t), &r_bytes, 1000) == ESP_OK)
    {
        for (int i = 0; i < r_bytes / 4; i++)
        {
            uint8_t  lsb = bytes[i * 4 + 1];
            uint8_t  mid = bytes[i * 4 + 2];
            uint8_t  msb = bytes[i * 4 + 3];

            uint32_t raw = (((uint32_t)msb) << 16) | ((uint32_t)mid << 8) | ((uint32_t)lsb);

            if (msb & 0x80) // lsb
            {
                raw |= 0xff800000;
            }
            // printf("msb: %02x, mid: %02x, lsb: %02x, raw: %08x %d\n",
            //        msb,
            //        mid,
            //        lsb,
            //        (unsigned int)raw,
            //        (int)raw);

            x1[i] = ((int32_t)raw);
        }
    }
    else
    {
        printf("Read Task: i2s read failed\n");
    }
}

void I2SSoundHandler::deviceInit(void)
{
    ESP_LOGI(TAG, "deviceInit");
    i2s_chan_config_t rx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&rx_chan_cfg, NULL, &rx_chan));

    i2s_std_config_t  rx_std_cfg  = {
        .clk_cfg                  = I2S_STD_CLK_DEFAULT_CONFIG(base_config.sampling_frequency),
        .slot_cfg                 = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg                 = {
            .mclk                 = I2S_GPIO_UNUSED, // some codecs may require mclk signal, this example doesn't need it
            .bclk                 = config.clock_pin,
            .ws                   = config.ws_pin,
            .dout                 = GPIO_NUM_NC,
            .din                  = config.in_pin,
            .invert_flags         = {
                .mclk_inv         = false,
                .bclk_inv         = false,
                .ws_inv           = false,
            },
        },
    };
    rx_std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_LEFT;
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_chan, &rx_std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_chan));
}
}
