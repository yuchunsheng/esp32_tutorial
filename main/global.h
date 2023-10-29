#ifndef global_h
#define global_h

#ifdef __cplusplus
extern "C" {
#endif

// #include "freertos/ringbuf.h"
#include "freertos/stream_buffer.h"

#define SAMPLE_RATE 16000
#define PERIOD 10  //10 ms sound
// #define FEATURE_PERIOD 30 // 30ms to generate features

#define AUDIO_INPUT_SIZE PERIOD*(sizeof(int16_t)*SAMPLE_RATE/1000) // 10ms 16 bytes length 
#define FEATURE_INPUT_SIZE PERIOD*(sizeof(int16_t)*SAMPLE_RATE/1000) // 10ms 16 bytes length 

#define AUDIO_STREAM_BUFFER_SIZE 2*3*AUDIO_INPUT_SIZE // 30ms 16 bytes length 

#define FEATURE_STREAM_BUFFER_SIZE 2*3*FEATURE_INPUT_SIZE  // now just save 30ms sound


// Create a custom structure to hold the ring buffer handle and other parameters
typedef struct {
    StreamBufferHandle_t audio_stream_buffer;
    StreamBufferHandle_t feature_stream_buffer;
} TaskParameters;

#ifdef __cplusplus
}
#endif

#endif