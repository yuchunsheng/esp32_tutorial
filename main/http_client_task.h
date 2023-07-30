#ifndef http_client_task
#define http_client_task

// Expose a C friendly interface for main functions.
#ifdef __cplusplus
extern "C" {
#endif

    void http_client_post_chunked(void * pPar);

#ifdef __cplusplus
}
#endif

#endif