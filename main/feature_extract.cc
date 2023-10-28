#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "feature_extract.h"
#include "audio_task.h"
#include "global.h"

#define TAG "Feature Extract Task"
// #define SAMPLE_RATE 16000
// #define PERIOD 10  //10 ms sound

void feature_extract_task(void *pPar)
{
    TaskParameters *params = (TaskParameters *)pPar;
    RingbufHandle_t audio_ring_buffer = params->audio_ring_buffer;
    RingbufHandle_t feature_ring_buffer = params->feature_ring_buffer;

    ESP_LOGI(TAG, "start feature extract task");
    
    while(1){
        size_t input_samples ;
        char *item = (char *)xRingbufferReceive(audio_ring_buffer, &input_samples, pdMS_TO_TICKS(10));
        //Check received item
        if (item != NULL) {
            //Print item
            // for (int i = 0; i < input_samples; i++) {
            //     printf("%c", item[i]);
            // }
            // ESP_LOGI(TAG, "input samples : %d", input_samples);
            UBaseType_t res =  xRingbufferSend(feature_ring_buffer, item, input_samples, pdMS_TO_TICKS(10));
            if (res != pdTRUE) {
                printf("Failed to send item from feature task \n");
            }

            // Return Item
            vRingbufferReturnItem(audio_ring_buffer, (void *)item);
        } else {
            //Failed to receive item
            printf("Failed to receive item\n");
        }
    
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
    
}