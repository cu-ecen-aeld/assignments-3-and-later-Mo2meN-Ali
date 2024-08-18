#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

#ifdef __cplusplus

#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

extern "C" {
struct timestamp_args {
    char *sTimeStamp_RFC2822;    // time stamp compliant format to strftime RFC2822
    unsigned char timestamp_len; // Length of the timestamp string
    pthread_mutex_t *fileMutex;  // A pointer to the file mutex 
    pthread_t threadID;         
};
} /* extern "C" */
#endif /* __cplusplus */
#endif /* TIMESTAMP_H_ */