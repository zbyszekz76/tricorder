#include "wifi_manager.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"

#include "nvs_flash.h"

#include "lwip/apps/sntp.h"

#include <string.h>

#include "../common/shared_data.h"

// =================================================
// ===================== CONFIG ====================
// =================================================

static const char* WIFI_SSID =
    "T-Mobile_Swiatlowod_2.4GHz_5CC0";

static const char* WIFI_PASS =
    "de2tujh4vgvfttjs";

// static const char* WIFI_SSID =
//     "sala12w";

// static const char* WIFI_PASS =
//     "zse7591@#";

// =================================================
// ===================== TAG =======================
// =================================================

static const char* TAG =
    "WIFI";

// =================================================
// ===================== STATE =====================
// =================================================

static bool wifi_connected =
    false;

static char ip_string[16] =
    "0.0.0.0";

// =================================================
// ===================== EVENT =====================
// =================================================

static void wifi_event_handler(
    void* arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void* event_data
)
{
    // STA START

    if (event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "Connecting...");

        esp_wifi_connect();
    }

    // DISCONNECTED

    else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGW(TAG, "Disconnected");

        wifi_connected = false;

        g_imu_data.wifi_connected = false;

        esp_wifi_connect();
    }

    // GOT IP

    else if (event_base == IP_EVENT &&
             event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event =
            (ip_event_got_ip_t*)event_data;

        sprintf(
            ip_string,
            IPSTR,
            IP2STR(&event->ip_info.ip)
        );

        ESP_LOGI(
            TAG,
            "IP: %s",
            ip_string
        );

        wifi_connected = true;

        strcpy(
            g_imu_data.ip_address,
            ip_string
        );

        g_imu_data.wifi_connected =
            true;

        // =============================================
        // ================= SNTP ======================
        // =============================================

        sntp_setoperatingmode(
            SNTP_OPMODE_POLL
        );

        sntp_setservername(
            0,
            "pool.ntp.org"
        );

        sntp_init();

        setenv(
            "TZ",
            "CET-1CEST,M3.5.0/2,M10.5.0/3",
            1
        );

        tzset();

        ESP_LOGI(
            TAG,
            "SNTP started"
        );

        time_t now;
struct tm timeinfo;

vTaskDelay(pdMS_TO_TICKS(3000));

time(&now);

localtime_r(
    &now,
    &timeinfo
);

ESP_LOGI(
    TAG,
    "TIME: %02d:%02d:%02d",
    timeinfo.tm_hour,
    timeinfo.tm_min,
    timeinfo.tm_sec
);
    }
}

// =================================================
// ===================== INIT ======================
// =================================================

void wifi_manager_init(void)
{
    ESP_LOGI(TAG, "WiFi init");

    // =================================================
    // ================= NVS ===========================
    // =================================================

    esp_err_t ret =
        nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(
            nvs_flash_erase()
        );

        ESP_ERROR_CHECK(
            nvs_flash_init()
        );
    }

    // =================================================
    // ================= NETIF =========================
    // =================================================

    ESP_ERROR_CHECK(
        esp_netif_init()
    );

    ESP_ERROR_CHECK(
        esp_event_loop_create_default()
    );

    esp_netif_create_default_wifi_sta();

    // =================================================
    // ================= WIFI INIT ====================
    // =================================================

    wifi_init_config_t cfg =
        WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(
        esp_wifi_init(&cfg)
    );

    // =================================================
    // ================= EVENTS =======================
    // =================================================

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL
        )
    );

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &wifi_event_handler,
            NULL
        )
    );

    // =================================================
    // ================= CONFIG =======================
    // =================================================

    wifi_config_t wifi_config = {};

    strcpy(
        (char*)wifi_config.sta.ssid,
        WIFI_SSID
    );

    strcpy(
        (char*)wifi_config.sta.password,
        WIFI_PASS
    );

    ESP_ERROR_CHECK(
        esp_wifi_set_mode(
            WIFI_MODE_STA
        )
    );

    ESP_ERROR_CHECK(
        esp_wifi_set_config(
            WIFI_IF_STA,
            &wifi_config
        )
    );

    // =================================================
    // ================= START ========================
    // =================================================

    ESP_ERROR_CHECK(
        esp_wifi_start()
    );

    ESP_LOGI(TAG, "WiFi started");
}

// =================================================
// ===================== API =======================
// =================================================

bool wifi_is_connected(void)
{
    return wifi_connected;
}

const char* wifi_get_ip(void)
{
    return ip_string;
}

int wifi_get_rssi(void)
{
    wifi_ap_record_t ap_info;

    if (esp_wifi_sta_get_ap_info(
            &ap_info
        ) == ESP_OK)
    {
        return ap_info.rssi;
    }

    return 0;
}

void wifi_update_time(void)
{
    time_t now;

    struct tm timeinfo;

    time(&now);

    localtime_r(
        &now,
        &timeinfo
    );

    strftime(
        g_imu_data.current_time,
        sizeof(g_imu_data.current_time),
        "%H:%M:%S",
        &timeinfo
    );

    strftime(
        g_imu_data.current_date,
        sizeof(g_imu_data.current_date),
        "%d-%m-%Y",
        &timeinfo
    );

    g_imu_data.wifi_rssi =
        wifi_get_rssi();
}