#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "led_irt.h"


#define ESP_INTR_FLAG_DEFAULT 0

#define BUTTON_PIN GPIO_NUM_4
#define LED_PIN GPIO_NUM_19

static TaskHandle_t xLedTaskHandle = NULL;
static TaskHandle_t xHttpClientTaskHandle = NULL;

static const char *LED_TAG = "LED Task";


void IRAM_ATTR button_isr_handler(void *arg)
{
    
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    // Notify the LED task to toggle the LED
    if(xLedTaskHandle != NULL){
        xTaskNotifyFromISR(xLedTaskHandle, 0, eNoAction, &xHigherPriorityTaskWoken);
    }
    if(xHttpClientTaskHandle != NULL){
        xTaskNotifyFromISR(xHttpClientTaskHandle, 0, eNoAction, &xHigherPriorityTaskWoken);

    }
    
}

void led_task(void* arg)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pin_bit_mask = (1ULL<<GPIO_NUM_19);
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pin_bit_mask = (1ULL<<GPIO_NUM_3);
    gpio_config(&io_conf);

    
    uint32_t ulNotificationValue;
    
    for (;;) {
        ESP_LOGI(LED_TAG, "in the led task");
        
        // Wait for the notification from the ISR
        xTaskNotifyWait(0, 0xffffffff, &ulNotificationValue, portMAX_DELAY);

        // Toggle the LED
        gpio_set_level(LED_PIN, !gpio_get_level(LED_PIN));
        gpio_set_level(GPIO_NUM_3, !gpio_get_level(GPIO_NUM_3));

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


void set_isr_task(){
    
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_POSEDGE; //GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pin_bit_mask = (1ULL<<GPIO_NUM_4);
    gpio_config(&io_conf);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(GPIO_NUM_4, button_isr_handler, NULL);

}

void set_isr_led_notify_handle(TaskHandle_t *isr_task_handle){
    vTaskDelay(pdMS_TO_TICKS(100));
    xLedTaskHandle = *isr_task_handle;
}

void set_isr_http_client_notify_handle(TaskHandle_t *isr_task_handle){
    vTaskDelay(pdMS_TO_TICKS(100));
    xHttpClientTaskHandle = *isr_task_handle;
}