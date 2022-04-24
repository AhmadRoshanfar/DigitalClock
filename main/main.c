#include "main.h"

static ssd1306_handle_t ssd1306_dev = NULL;

/* Variable holding number of times ESP32 restarted since first boot.
 * It is placed into RTC memory using RTC_DATA_ATTR and
 * maintains its value when ESP32 wakes from deep sleep.
 */
RTC_DATA_ATTR static int boot_count = 0;

static const char *TAG = "Main";

void nvs_init()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}
void oled_init()
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);

    ssd1306_dev = ssd1306_create(I2C_MASTER_NUM, SSD1306_I2C_ADDRESS);
    ssd1306_refresh_gram(ssd1306_dev);
    ssd1306_clear_screen(ssd1306_dev, 0x00);
}

void hw_init()
{
    nvs_init();
    oled_init();
}
void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

static void init_sntp()
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
}

static void show_time(void *arg)
{

    // localtime_r(&now, &timeinfo);
    // strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    // ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);

    time_t now = 0;
    struct tm timeinfo;
    memset((void *)&timeinfo, 0, sizeof(timeinfo));
    char buf[64];

    setenv("TZ", "IRST-4:30", 1);
    tzset();

    ssd1306_clear_screen(ssd1306_dev, 0x00);

    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
        // ssd1306_refresh_gram(ssd1306_dev);
        strftime(buf, sizeof(buf), "%H:%M:%S", &timeinfo);
        ssd1306_draw_string(ssd1306_dev, 32, 16, (const uint8_t *)buf, 16, 1);
        strftime(buf, sizeof(buf), "%A", &timeinfo);
        ssd1306_draw_string(ssd1306_dev, 32, 0, (const uint8_t *)buf, 16, 1);
        strftime(buf, sizeof(buf), "%x", &timeinfo);
        ssd1306_draw_string(ssd1306_dev, 32, 32, (const uint8_t *)buf, 16, 1);
        ssd1306_refresh_gram(ssd1306_dev);

        /* Deep Sleep for 10 Second */
        // const int deep_sleep_sec = 10;
        // ESP_LOGI(TAG, "Entering deep sleep for %d seconds", deep_sleep_sec);
        // esp_deep_sleep(1000000LL * deep_sleep_sec);
    }
}

static void sync_time(void *arg)
{
    char buffer[20] = {0};
    int retry = 0;
    const int retry_count = 10;

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
    {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    if (retry == retry_count)
    {
        ssd1306_clear_screen(ssd1306_dev, 0x00);
        sprintf(buffer, "Sync Failed");
        ssd1306_draw_string(ssd1306_dev, 0, 32, (const uint8_t *)buffer, 16, 1);
        vTaskDelete(NULL);
        return;
    }

    xTaskCreate(show_time, "show_time", 8192, NULL, 5, NULL);
    vTaskDelete(NULL);
}

void wifi_conn_cb(void)
{
    char data_str[20] = {0};
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    sprintf(data_str, "Wifi connected");
    ssd1306_draw_string(ssd1306_dev, 2, 32, (const uint8_t *)data_str, 16, 1);
    ssd1306_refresh_gram(ssd1306_dev);
    init_sntp();
    xTaskCreate(sync_time, "sync", 8192, NULL, 5, NULL);
}

void wifi_failed_cb(void)
{
    ESP_LOGE(TAG, "wifi failed");
}

void wifi_init()
{

    connect_wifi_params_t p = {
        .on_connected = wifi_conn_cb,
        .on_failed = wifi_failed_cb};
    connect_wifi(p);
}

void app_main(void)
{
    ++boot_count;
    ESP_LOGI(TAG, "Boot count: %d", boot_count);

    hw_init();
    wifi_init();
    char data_str[20] = {0};
    sprintf(data_str, "Hello Bright");
    ssd1306_draw_string(ssd1306_dev, 10, 0, (const uint8_t *)data_str, 16, 1);
    ssd1306_refresh_gram(ssd1306_dev);
}
