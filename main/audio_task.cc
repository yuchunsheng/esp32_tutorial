#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "freertos/ringbuf.h"
// #include "freertos/stream_buffer.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2s_std.h"

#include <cstring>
#include "audio_task.h"
#include "config.h"
#include "global.h"

#define I2S_WS 38
#define I2S_SCK 37
#define I2S_SD 36


static const char *TAG = "I2S Task";

i2s_chan_handle_t rx_chan;
// size_t example_buffer_size = 16000 ;

void i2s_example_init_std_simplex(size_t sample_rate)
{
    // i2s_chan_config_t rx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    i2s_chan_config_t rx_chan_cfg = {
        .id = I2S_NUM_AUTO, 
        // .id = I2S_NUM_0, 
        .role = I2S_ROLE_MASTER, 
        .dma_desc_num = 8, 
        .dma_frame_num = 256, 
        .auto_clear = false,
    };

    ESP_ERROR_CHECK(i2s_new_channel(&rx_chan_cfg, NULL, &rx_chan));

    i2s_std_config_t rx_std_cfg = {
        // .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate),
        // .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000),
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate),
        // .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
        // .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg =
            {
                .mclk = I2S_GPIO_UNUSED,
                .bclk = GPIO_NUM_37,
                .ws = GPIO_NUM_38,
                .dout = I2S_GPIO_UNUSED,
                .din = GPIO_NUM_36,
                .invert_flags =
                    {
                        .mclk_inv = false,
                        .bclk_inv = false,
                        .ws_inv = false,
                    },
            },
    };
    rx_std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_LEFT; //I2S_STD_SLOT_RIGHT;
    rx_std_cfg.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256; //I2S_MCLK_MULTIPLE_384; 
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_chan, &rx_std_cfg));
    // ESP_ERROR_CHECK(i2s_channel_enable(rx_chan));

    /* Enable the RX channel */
    ESP_ERROR_CHECK(i2s_channel_enable(rx_chan));
    ESP_LOGI(TAG, "I2S Channel enabled.....");
}

void i2s_example_read_task(char *r_buf, size_t r_samples)
{
    size_t r_bytes = 0;

    /* Read i2s data */
    if (i2s_channel_read(rx_chan, r_buf, r_samples, &r_bytes, portMAX_DELAY) == ESP_OK)
    {
        ESP_LOGI(TAG, "Read %d Bytes\n", r_bytes);
    }
    else
    {
        ESP_LOGE(TAG, "Read Task: i2s read failed\n");
    }

    
}


void audio_input_task(void *pPar)
{
    ESP_LOGI(TAG, "start audio input task");
    TaskParameters *params = (TaskParameters *)pPar;
    StreamBufferHandle_t audio_stream_buffer = params->audio_stream_buffer;
    StreamBufferHandle_t feature_stream_buffer = params->feature_stream_buffer;

    i2s_example_init_std_simplex(SAMPLE_RATE);

    //read 10ms sound
    char * r_buf = (char *)calloc(AUDIO_INPUT_SIZE, sizeof(char *));
    size_t r_samples = AUDIO_INPUT_SIZE;
    assert(r_buf); // Check if r_buf allocation success

    while(1){

        size_t r_bytes = 0;

        /* Read i2s data */
        if (i2s_channel_read(rx_chan, r_buf, r_samples, &r_bytes, portMAX_DELAY) == ESP_OK)
        {
            // ESP_LOGI(TAG, "Read %d Bytes\n", r_bytes);
            if (audio_stream_buffer == NULL) {
                printf("Failed to create stream buffer\n");
            }else{
                // UBaseType_t res =  xRingbufferSend(audio_stream_buffer, r_buf, r_bytes, pdMS_TO_TICKS(10));
                size_t xBytesSent = xStreamBufferSend(audio_stream_buffer, r_buf, r_bytes, portMAX_DELAY);
                
                if (xBytesSent != r_bytes) {
                    printf("sent bytes: %d, received bytes %d", xBytesSent, r_bytes);
                    
                    printf("Failed to send item from the audio task\n");
                }

            }
            
        }
        else
        {
            ESP_LOGE(TAG, "Read Task: i2s read failed\n");
        }
        // ESP_LOGI(TAG, "read 10ms sound");
        vTaskDelay(pdMS_TO_TICKS(10)); //delay 10ms to read

    }

    free(r_buf);
    vTaskDelete(NULL);
    
}