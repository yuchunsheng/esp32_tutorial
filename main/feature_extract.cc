#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "feature_extract.h"
#include "audio_task.h"

#define TAG "Feature Extract Task"

void feature_extract_task(void *pPar)
{
    ESP_LOGI(TAG, "start feature extract task");
    vTaskDelete(NULL);
}