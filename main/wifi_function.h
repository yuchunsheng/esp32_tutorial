#ifndef wifi_function
#define wifi_function

// Expose a C friendly interface for main functions.
#ifdef __cplusplus
extern "C" {
#endif
// void event_handler(void* arg, esp_event_base_t event_base,
//                                 int32_t event_id, void* event_data);

// void wifi_init_sta(void);

// void wifi_task(void *pvParameter);
esp_err_t connect_to_wifi();

#ifdef __cplusplus
}
#endif

#endif