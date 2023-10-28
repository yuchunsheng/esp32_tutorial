#ifndef global_h
#define global_h

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/ringbuf.h"

#define SAMPLE_RATE 16000
#define PERIOD 10  //10 ms sound
// #define FEATURE_PERIOD 30 // 30ms to generate features

#define AUDIO_INPUT_SIZE PERIOD*(sizeof(int16_t)*SAMPLE_RATE/1000) // 10ms 16 bytes length 

#define AUDIO_RING_BUFFER_SIZE 2*3*AUDIO_INPUT_SIZE // 30ms 16 bytes length 

#define FEATURE_RING_BUFFER_SIZE 2*3*AUDIO_INPUT_SIZE  // now just save 30ms sound


// Create a custom structure to hold the ring buffer handle and other parameters
typedef struct {
    RingbufHandle_t audio_ring_buffer;
    RingbufHandle_t feature_ring_buffer;
} TaskParameters;

#ifdef __cplusplus
}
#endif

#endif