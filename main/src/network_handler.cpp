#include "network_handler.h"

#include <string.h>

namespace AutoTuner
{
#define WIFI_CONNECTED_BIT    BIT0
#define WIFI_FAIL_BIT         BIT1

char NetworkHandler::query_string[1024] = {0};

const char         *NetworkHandler::TAG           = "NetworkHandler";
const char         *NetworkHandler::ssid          = "";
const char         *NetworkHandler::password      = "";
int                NetworkHandler::retry_num      = {0};
bool               NetworkHandler::is_initialized = {false};
EventGroupHandle_t NetworkHandler::s_wifi_event_group;
const int32_t      NetworkHandler::max_try        = 16;               // 256
const httpd_uri_t  NetworkHandler::root           = {
    .uri                                = "/",
    .method                             = HTTP_GET,
    .handler                            = NetworkHandler::root_get_handler
};
httpd_handle_t     NetworkHandler::server         = NULL;
int                NetworkHandler::instrumentId   = 0;
int                NetworkHandler::tuningId       = 0;
TuningHandler      *NetworkHandler::tuning        = NULL;

void NetworkHandler::init()
{
    if (!is_initialized)
    {
        esp_event_handler_instance_t instance_any_id;
        esp_event_handler_instance_t instance_got_ip;
        esp_event_handler_instance_t instance_http;

        is_initialized = true;


        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);

        ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

        s_wifi_event_group = xEventGroupCreate();

        ESP_ERROR_CHECK(esp_netif_init());

        ESP_ERROR_CHECK(esp_event_loop_create_default());
        esp_netif_create_default_wifi_sta();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                            ESP_EVENT_ANY_ID,
                                                            &NetworkHandler::eventHandler,
                                                            NULL,
                                                            &instance_any_id));
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                            IP_EVENT_STA_GOT_IP,
                                                            &NetworkHandler::eventHandler,
                                                            NULL,
                                                            &instance_got_ip));

        ESP_ERROR_CHECK(esp_event_handler_instance_register(ESP_HTTP_SERVER_EVENT,
                                                            ESP_EVENT_ANY_ID,
                                                            &NetworkHandler::httpEvent,
                                                            NULL,
                                                            &instance_http));

        wifi_config_t wifi_config = {
            .sta                           = {
                .sae_pwe_h2e               = WPA3_SAE_PWE_BOTH,
                .sae_h2e_identifier        = "",
            },
        };
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK,

        strcpy((char *)wifi_config.sta.ssid, ssid);
        strcpy((char *)wifi_config.sta.password, password);

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());
        ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

        EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                               WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                               pdFALSE,
                                               pdFALSE,
                                               portMAX_DELAY);

        if (bits & WIFI_CONNECTED_BIT)
        {
            ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                     ssid, password);
        }
        else if (bits & WIFI_FAIL_BIT)
        {
            ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                     ssid, password);
        }
        else
        {
            ESP_LOGE(TAG, "UNEXPECTED EVENT");
        }
    }
}

int NetworkHandler::getInstrument()
{
    return instrumentId;
}

int NetworkHandler::getTuning()
{
    return tuningId;
}

void NetworkHandler::setTuningHandler(TuningHandler *tuning)
{
    NetworkHandler::tuning = tuning;
}

void NetworkHandler::eventHandler(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (retry_num == 0)
        {
            if (server)
            {
                if (stopServer(server) == ESP_OK)
                {
                    server = NULL;
                }
                else
                {
                    ESP_LOGE(TAG, "Failed to stop https server");
                }
            }
        }
        if (retry_num < max_try)
        {
            esp_wifi_connect();
            retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

        if (server == NULL)
        {
            server = startServer();
        }
    }
}

void NetworkHandler::httpEvent(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == ESP_HTTP_SERVER_EVENT)
    {
        if (event_id == HTTP_SERVER_EVENT_ERROR)
        {
            esp_tls_last_error_t *last_error = (esp_tls_last_error_t *)event_data;
            ESP_LOGI(TAG,
                     "Error event triggered: last_error = %s, last_tls_err = %d, tls_flag = %d",
                     esp_err_to_name(last_error->last_error),
                     last_error->esp_tls_error_code,
                     last_error->esp_tls_flags);
        }
    }
}

esp_err_t NetworkHandler::root_get_handler(httpd_req_t *req)
{
    httpd_req_get_url_query_str(req, query_string, query_size);

    if (query_string[0] != '\0')
    {
        ESP_LOGI(TAG, "URL query = %s", query_string);
        if (strstr(query_string, "updateTable") != NULL)
        {
            httpd_resp_set_type(req, "text/csv");
            httpd_resp_send(req, tuning->getTableCsv(), HTTPD_RESP_USE_STRLEN);
        }
        else if (strstr(query_string, "updateChoices") != NULL)
        {
            httpd_resp_set_type(req, "text/csv");
            httpd_resp_send(req, tuning->getChoicesCsv(), HTTPD_RESP_USE_STRLEN);
        }
        else if (strstr(query_string, "instrument") != NULL)
        {
            if (2 == sscanf(query_string,
                            "%*[^0123456789]%d%*[^0123456789]%d",
                            &instrumentId,
                            &tuningId))
            {
                ESP_LOGI(TAG, "instrument = %d, tuning = %d", instrumentId, tuningId);
            }
            httpd_resp_set_status(req, "204 No Content");
            httpd_resp_send(req, NULL, 0);
        }

        query_string[0] = '\0';
    }
    else
    {
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    }

    return ESP_OK;
}

httpd_handle_t NetworkHandler::startServer(void)
{
    httpd_handle_t server = NULL;

    ESP_LOGI(TAG, "Starting server");

    httpd_config_t conf   = HTTPD_DEFAULT_CONFIG();
    conf.lru_purge_enable = true;

    esp_err_t      ret    = httpd_start(&server, &conf);
    if (ESP_OK != ret)
    {
        ESP_LOGI(TAG, "Error starting server!");

        return NULL;
    }

    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &root);

    return server;
}

esp_err_t NetworkHandler::stopServer(httpd_handle_t server)
{
    return httpd_stop(server);
}
}
