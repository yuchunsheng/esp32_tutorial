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
    StreamBufferHandle_t audio_stream_buffer = params->audio_stream_buffer;
    StreamBufferHandle_t feature_stream_buffer = params->feature_stream_buffer;

    ESP_LOGI(TAG, "start feature extract task");
    char * received_data = (char *)calloc(AUDIO_INPUT_SIZE, sizeof(char *));;
    size_t input_samples ;

    while(1){
        
        // char *item = (char *)xRingbufferReceive(audio_ring_buffer, &input_samples, pdMS_TO_TICKS(10));
        input_samples = xStreamBufferReceive(audio_stream_buffer, received_data, AUDIO_INPUT_SIZE, portMAX_DELAY);

        //Check received item
        if (input_samples > 0) {
            //Print item
            // for (int i = 0; i < input_samples; i++) {
            //     printf("%c", item[i]);
            // }
            // ESP_LOGI(TAG, "input samples : %d", input_samples);
            size_t xBytesSent =  xStreamBufferSend(feature_stream_buffer, received_data, FEATURE_INPUT_SIZE, portMAX_DELAY);
            if (xBytesSent != FEATURE_INPUT_SIZE) {
                printf("sent bytes: %d, received bytes %d", xBytesSent, FEATURE_INPUT_SIZE);
                printf("Failed to send item from feature task \n");
            }

            // Return Item
            // vRingbufferReturnItem(audio_ring_buffer, (void *)item);
        } else {
            //Failed to receive item
            printf("Failed to receive item\n");
        }
    
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
    
}