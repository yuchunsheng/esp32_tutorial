#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include "sdkconfig.h"

#include "led_irt.h"
#include "wifi_function.h"
#include "http_client_task.h"
#include "feature_extract.h"
#include "http_feature_post.h"
#include "audio_task.h"
#include "global.h"

static const char *TAG = "Main Function";

// Create a ring buffer with the specified size
RingbufHandle_t audio_ring_buffer = xRingbufferCreate(AUDIO_RING_BUFFER_SIZE, RINGBUF_TYPE_NOSPLIT);
RingbufHandle_t feature_ring_buffer = xRingbufferCreate(FEATURE_RING_BUFFER_SIZE, RINGBUF_TYPE_NOSPLIT);

// Create producer and consumer tasks with different parameters
TaskParameters feature_extract_task_params = {audio_ring_buffer, feature_ring_buffer};
TaskParameters http_feature_post_params = {audio_ring_buffer, feature_ring_buffer};

// Expose a C friendly interface for main functions.
#ifdef __cplusplus
extern "C" {
#endif

void app_main(void)
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

        // xTaskCreatePinnedToCore(http_client_post_chunked, "http client", 1024*10, NULL, 5, &r_irs_http_client_handle, 1);
        // ESP_LOGI(TAG, "create http_client_post_chunked task");
        // set_isr_http_client_notify_handle(&r_irs_http_client_handle);
        // //delay 40 ms to initiate the GPIO
        // vTaskDelay(pdMS_TO_TICKS(40));

        xTaskCreatePinnedToCore(audio_input_task, "audio input", 1024*2, &feature_extract_task_params, 5, NULL, 1);
        ESP_LOGI(TAG, "create freature extract task");
        //delay 20 ms to initiate the GPIO
        vTaskDelay(pdMS_TO_TICKS(20));

        xTaskCreatePinnedToCore(feature_extract_task, "feature extract", 1024*2, &feature_extract_task_params, 5, NULL, 1);
        ESP_LOGI(TAG, "create freature extract task");

        //delay 30 ms to initiate the GPIO
        vTaskDelay(pdMS_TO_TICKS(30));

        xTaskCreatePinnedToCore(http_feature_post, "http feature post", 1024*40, &http_feature_post_params, 5, &r_irs_http_client_handle, 1);
        ESP_LOGI(TAG, "create http_client_post_chunked task");
        set_isr_http_client_notify_handle(&r_irs_http_client_handle);
        //delay 40 ms to initiate the GPIO
        // vTaskDelay(pdMS_TO_TICKS(40));

        

        

    }

    vTaskDelete(NULL);

}


#ifdef __cplusplus
}
#endif