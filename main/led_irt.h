#ifndef led_irt
#define led_irt

// Expose a C friendly interface for main functions.
#ifdef __cplusplus
extern "C" {
#endif

void led_task(void* arg);

void set_isr_task();
void set_isr_led_notify_handle(TaskHandle_t *isr_task_handle);
void set_isr_http_client_notify_handle(TaskHandle_t *isr_task_handle);

#ifdef __cplusplus
}
#endif

#endif