#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include "sdkconfig.h"

#include "led_irt.h"
#include "wifi_function.h"
#include "http_client_task.h"

static const char *TAG = "Main Function";


extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Start ....");
    // Create the task handle for the interrupt task
    TaskHandle_t r_irs_http_client_handle = NULL;
    TaskHandle_t r_irs_led_handle = NULL;

    esp_err_t ret = connect_to_wifi();
    // esp_err_t ret = ESP_OK;
    if (ret == ESP_OK){
        set_isr_task();
        
        xTaskCreatePinnedToCore(led_task, "led_task", 1024*4, NULL, tskIDLE_PRIORITY, &r_irs_led_handle,1);
        set_isr_led_notify_handle(&r_irs_led_handle);
        //delay 40 ms to initiate the GPIO
        vTaskDelay(pdMS_TO_TICKS(40));

        xTaskCreatePinnedToCore(http_client_post_sound, "http client", 1024*10, NULL, 5, &r_irs_http_client_handle, 1);
        ESP_LOGI(TAG, "create http_client_post_chunked task");
        set_isr_http_client_notify_handle(&r_irs_http_client_handle);
        //delay 40 ms to initiate the GPIO
        vTaskDelay(pdMS_TO_TICKS(40));
       

    }

    vTaskDelete(NULL);

}
