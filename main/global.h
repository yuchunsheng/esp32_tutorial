#ifndef global_h
#define global_h

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/ringbuf.h"

#define SAMPLE_RATE 16000
#define PERIOD 10  //10 ms sound
#define FEATURE_PERIOD 30 // 30ms to generate features

#define AUDIO_RING_BUFFER_SIZE 2*PERIOD*(SAMPLE_RATE/1000) // 10ms bytes 
#define FEATURE_RING_BUFFER_SIZE 2*FEATURE_PERIOD*(SAMPLE_RATE/1000)  // now just save 30ms sound


// Create a custom structure to hold the ring buffer handle and other parameters
typedef struct {
    RingbufHandle_t input_ring_buffer;
    RingbufHandle_t output_ring_buffer;
} TaskParameters;

#ifdef __cplusplus
}
#endif

#endif