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
    RingbufHandle_t input_ring_buffer = params->input_ring_buffer;
    RingbufHandle_t output_ring_buffer = params->output_ring_buffer;

    ESP_LOGI(TAG, "start feature extract task");
    
    
    while(1){
        size_t input_samples ;
        char *item = (char *)xRingbufferReceive(input_ring_buffer, &input_samples, pdMS_TO_TICKS(1000));
        //Check received item
        if (item != NULL) {
            //Print item
            for (int i = 0; i < input_samples; i++) {
                printf("%c", item[i]);
            }
            printf("\n");
            // Return Item
            vRingbufferReturnItem(input_ring_buffer, (void *)item);
        } else {
            //Failed to receive item
            printf("Failed to receive item\n");
        }

    
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    vTaskDelete(NULL);
    
}