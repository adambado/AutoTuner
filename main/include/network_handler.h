#ifndef WIFI_HANDLING_H
#define WIFI_HANDLING_H

#include "tuning_handler.h"

#include <stdint.h>

#include "driver/ledc.h"
#include "esp_timer.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"


#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include <stdio.h>
#include <string.h>

#include "nvs_flash.h"
#include "esp_event.h"

#include "protocol_examples_common.h"
#include "protocol_examples_utils.h"
#include "esp_wifi.h"
#include "driver/i2s.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_check.h"
#include "esp_now.h"

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"

#include <esp_http_server.h>
#include "esp_tls.h"
#include "sdkconfig.h"

// TODO: use mdns
// TODO: add next string button to html
// TODO: wifi connection issue in box
// TODO: refactor networkHandler
// TODO: new undefined value check for frequency
// TODO: selected string color change

namespace AutoTuner
{
#define WIFI_CONNECTED_BIT    BIT0
#define WIFI_FAIL_BIT         BIT1

class NetworkHandler
{
    private:
        static const int32_t      query_size = 1024;
        static int                retry_num;
        static const char         *TAG;
        static const char         *ssid;
        static const char         *password;
        static EventGroupHandle_t s_wifi_event_group;
        static bool               is_initialized;
        static const int32_t      max_try;
        static const httpd_uri_t  root;
        static httpd_handle_t     server;
        static char               query_string[query_size];
        static const char         *html;

    public:
        static void init();
        void setTuningHandler(TuningHandler *tuning);

        int getInstrument();
        int getTuning();

    private:
        static void eventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
        static void httpEvent(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
        static httpd_handle_t startServer(void);
        static esp_err_t stopServer(httpd_handle_t server);
        static esp_err_t root_get_handler(httpd_req_t *req);

        static int           instrumentId;
        static int           tuningId;
        static TuningHandler *tuning;
};
}

#endif // WIFI_HANDLING_H
