#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "feature_extract.h"
#include "audio_task.h"

#define TAG "Feature Extract Task"
#define SAMPLE_RATE 16000
#define PERIOD 10  //10 ms sound

void feature_extract_task(void *pPar)
{
    ESP_LOGI(TAG, "start feature extract task");
    i2s_example_init_std_simplex(SAMPLE_RATE);

    //read 10ms sound
    char * r_buf = (char *)calloc(2*PERIOD, sizeof(uint16_t));
    size_t r_samples = 2*PERIOD*sizeof(uint16_t);
    assert(r_buf); // Check if r_buf allocation success
    //store 30ms sound to do FFT
    char * feature_buf = (char *)calloc(2*3*PERIOD, sizeof(uint16_t));
    size_t feature_samples = 2*3*PERIOD*sizeof(uint16_t);
    assert(feature_buf); // Check if r_buf allocation success

    while(1){

        i2s_example_read_task(r_buf, r_samples);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    free(r_buf);
    vTaskDelete(NULL);
    
}