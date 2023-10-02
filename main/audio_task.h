#ifndef audio_task_h
#define audio_task_h

// Expose a C friendly interface for main functions.
#ifdef __cplusplus
extern "C" {
#endif
    void i2s_example_init_std_simplex(size_t sample_rate);
    void i2s_example_read_task(char *r_buf, size_t r_samples);
    void audio_input_task(void *pPar);


#ifdef __cplusplus
}
#endif

#endif