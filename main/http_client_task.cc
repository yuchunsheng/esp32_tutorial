#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/ringbuf.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "esp_tls.h"

#include "http_client_task.h"
#include "wifi_function.h"
#include "config.h"

#include "audio_task.h"


#define MAX_HTTP_RECV_BUFFER 1024*2
#define MAX_HTTP_OUTPUT_BUFFER 1024*2

#define SAMPLE_RATE 16000

const char *HTTP_CLIENT_TAG = "http client";
const char *LOCAL_URL = "http://10.0.0.55:8888";


esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer; // Buffer to store response of http request from event handler
    static int output_len;      // Stores number of bytes read
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:{
        ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ERROR");
        break;

    }
        
    case HTTP_EVENT_ON_CONNECTED:{
        ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ON_CONNECTED");
        break;

    }
        
    case HTTP_EVENT_HEADER_SENT:{
        ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_HEADER_SENT");
        break;

    }
        
    case HTTP_EVENT_ON_HEADER:{
        ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;

    }
        
    case HTTP_EVENT_ON_DATA:{
        ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        /*
         *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
         *  However, event handler can also be used in case chunked encoding is used.
         */
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            // If user_data buffer is configured, copy the response into the buffer
            int copy_len = 0;
            if (evt->user_data)
            {
                copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                if (copy_len)
                {
                    memcpy(evt->user_data + output_len, evt->data, copy_len);
                }
            }
            else
            {
                const int buffer_len = esp_http_client_get_content_length(evt->client);
                if (output_buffer == NULL)
                {
                    output_buffer = (char *)malloc(buffer_len);
                    output_len = 0;
                    if (output_buffer == NULL)
                    {
                        ESP_LOGE(HTTP_CLIENT_TAG, "Failed to allocate memory for output buffer");
                        return ESP_FAIL;
                    }
                }
                copy_len = MIN(evt->data_len, (buffer_len - output_len));
                if (copy_len)
                {
                    memcpy(output_buffer + output_len, evt->data, copy_len);
                }
            }
            output_len += copy_len;
        }

        break;

    }
        
    case HTTP_EVENT_ON_FINISH:{
        ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ON_FINISH");
        if (output_buffer != NULL)
        {
            // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
            // ESP_LOG_BUFFER_HEX(HTTP_CLIENT_TAG, output_buffer, output_len);
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;

    }
        
    case HTTP_EVENT_DISCONNECTED:{
        ESP_LOGI(HTTP_CLIENT_TAG, "HTTP_EVENT_DISCONNECTED");
        int mbedtls_err = 0;
        esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
        if (err != 0)
        {
            ESP_LOGI(HTTP_CLIENT_TAG, "Last esp error code: 0x%x", err);
            ESP_LOGI(HTTP_CLIENT_TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
        }
        if (output_buffer != NULL)
        {
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;

    }
        
    case HTTP_EVENT_REDIRECT:{
        ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_REDIRECT");
        esp_http_client_set_header(evt->client, "From", "user@example.com");
        esp_http_client_set_header(evt->client, "Accept", "text/html");
        esp_http_client_set_redirection(evt->client);
        break;

    }
        
    }
    return ESP_OK;
}

int post_chunk_data(char *data, size_t buffer_size,  esp_http_client_handle_t http_client);
void get_http_response(char * local_response_buffer, esp_http_client_handle_t client);
void read_i2s();

void http_client_post_chunked(void *pPar)
{
    ESP_LOGI(HTTP_CLIENT_TAG, "in the http_client_post_chunked");
    // RingbufHandle_t *rb = (RingbufHandle_t*)pPar;
    
    // size_t feature_buffer_size = SAMPLE_RATE; //save 1 second data
    
    // char* data = (char *)calloc(feature_buffer_size, sizeof(char));
    
    // if (data == NULL) {
    //     ESP_LOGI(HTTP_CLIENT_TAG, "data pointer allocate is failed");
    //     ESP_LOGI(HTTP_CLIENT_TAG, "feature_buffer_size is %d", feature_buffer_size );
    // }else{
    //     ESP_LOGI(HTTP_CLIENT_TAG, "http_client_post_chunked buffer address is %p", (void *)&data);
    //     ESP_LOGI(HTTP_CLIENT_TAG, "after copy data value is: %c", data[0]);
    // }
   
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

    i2s_example_init_std_simplex(SAMPLE_RATE);
    
    while(1){

        esp_http_client_config_t config = {
            // .url = LOCAL_URL,
            .host = "10.0.0.55",
            .port = 8888,
            .path = "/upload",
            .disable_auto_redirect = true,
            .event_handler = _http_event_handler,
            .user_data = local_response_buffer, // Pass address of local buffer to get response
               
        };
        esp_http_client_handle_t client = esp_http_client_init(&config);
    
        esp_http_client_set_method(client, HTTP_METHOD_POST);

        esp_http_client_set_header(client, "Transfer-Encoding", "chunked");
        esp_http_client_set_header(client, "x-audio-sample-rates", "16000");
        esp_http_client_set_header(client, "x-audio-bits", "16");
        esp_http_client_set_header(client, "x-audio-channel", "1");
        
        uint32_t ulNotificationValue;
        xTaskNotifyWait(0, 0xffffffff, &ulNotificationValue, portMAX_DELAY);

        esp_err_t err = esp_http_client_open(client, -1); // write_len=-1 sets header "Transfer-Encoding: chunked" and method to POST
        ESP_LOGI(HTTP_CLIENT_TAG, "stat posting data, err code is %d", err);
        if (err == ESP_OK)
        {
            //read i2s sound
            // read_i2s();
            
            char * r_buf = (char *)calloc(2*SAMPLE_RATE, sizeof(uint16_t));
            size_t r_samples = 2*SAMPLE_RATE*sizeof(uint16_t);
            assert(r_buf); // Check if r_buf allocation success
            i2s_example_read_task(r_buf, r_samples);
            vTaskDelay(pdMS_TO_TICKS(1100));

            int data_length = post_chunk_data(r_buf, r_samples, client);
            
            ESP_LOGI(HTTP_CLIENT_TAG, "post_chunk is done. Send data: %d", data_length);
                    
            // get_http_response(local_response_buffer, client);
            free(r_buf);
            esp_http_client_close(client);
        }
        else
        {
            ESP_LOGE(HTTP_CLIENT_TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        }

        esp_http_client_cleanup(client);
        // vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int post_chunk_data(char *data, size_t buffer_size,  esp_http_client_handle_t http_client)
{
    int written_length = 0;
    int total_length = 0;
    char length_string[11] = "";
    ESP_LOGI(HTTP_CLIENT_TAG, "transmit_split_chunk_data data buffer address is %p", (void *)data);

    if (buffer_size > 0)
    {
        // bzero(length_string, sizeof(length_string));
        // snprintf(length_string, sizeof(length_string), "%X", buffer_size);
        ESP_LOGI(HTTP_CLIENT_TAG, "buffer_size=%d", buffer_size);
        itoa((int)buffer_size, length_string, 10);
        ESP_LOGI(HTTP_CLIENT_TAG, "data length = %s", length_string);
        ESP_LOGI(HTTP_CLIENT_TAG, "length_string = %d", strlen(length_string));

        written_length = esp_http_client_write(http_client, length_string, strlen(length_string));

        if (written_length > 0)
        {
            total_length += written_length;
        }
        else
        {
            return -1;
        }

        written_length = esp_http_client_write(http_client, "\r\n", 2);

        if (written_length > 0)
        {
            total_length += written_length;
        }
        else
        {
            return -1;
        }

        ESP_LOGI(HTTP_CLIENT_TAG, "chunk size is : %s", length_string);

        written_length = esp_http_client_write(http_client, data, (int)buffer_size);
        ESP_LOGI(HTTP_CLIENT_TAG, "data chunk size is : %d", buffer_size);


        if (written_length > 0)
        {
            total_length += written_length;
        }
        else
        {
            return -1;
        }
        
        written_length = esp_http_client_write(http_client, "\r\n", 2);

        if (written_length > 0)
        {
            total_length += written_length;
        }
        else
        {
            return -1;
        }

        // ESP_LOGD(HTTP_CLIENT_TAG, "Sent: %s", data);
    }
    
    written_length = esp_http_client_write(http_client, "0", 1); // end

    if (written_length > 0)
    {
        total_length += written_length;
    }
    else
    {
        return -1;
    }

    esp_http_client_write(http_client, "\r\n", 2);

    if (written_length > 0)
    {
        total_length += written_length;
    }
    else
    {
        return -1;
    }


    ESP_LOGI(HTTP_CLIENT_TAG, "Sent http end chunk");

    return total_length;
}

void get_http_response(char * local_response_buffer, esp_http_client_handle_t client){
    
    // After the POST is complete, you can examine the response as required using:
    int content_length = esp_http_client_fetch_headers(client);
    ESP_LOGI(HTTP_CLIENT_TAG, "esp_http_client_fetch_headers is done");
    int total_read_len = 0, read_len;
    if (total_read_len < content_length && content_length <= MAX_HTTP_RECV_BUFFER)
    {
        read_len = esp_http_client_read(client, local_response_buffer, content_length);
        if (read_len <= 0)
        {
            ESP_LOGE(HTTP_CLIENT_TAG, "Error read data");
        }
        local_response_buffer[read_len] = 0;
        ESP_LOGD(HTTP_CLIENT_TAG, "read_len = %d", read_len);
    }

    ESP_LOGI(HTTP_CLIENT_TAG, "HTTP POST Status = %d, content_length = %" PRIu64,
            esp_http_client_get_status_code(client),
            esp_http_client_get_content_length(client));
    ESP_LOGI(HTTP_CLIENT_TAG, "Response: %s", local_response_buffer);
}

void read_i2s(){
    ESP_LOGI(HTTP_CLIENT_TAG, "Reading INMP441");
}