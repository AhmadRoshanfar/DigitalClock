#include <stdio.h>
#include <string.h>
#include "ssd1306.h"
#include "connect_wifi.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include <time.h>
#include <sys/time.h>
#include "esp_sntp.h"
#include "esp_sleep.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define I2C_MASTER_SCL_IO 22      /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 21      /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_1  /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000 /*!< I2C master clock frequency */