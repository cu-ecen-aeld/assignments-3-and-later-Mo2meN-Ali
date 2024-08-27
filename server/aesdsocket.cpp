#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "server.h"
#include "queue.h"
#include "timestamp.h"
#include "../aesd-char-driver/aesd_ioctl.h"

#define USE_AESD_CHAR_DEVICE (1U) // A build switch for the 'aesdchar' device driver

constexpr unsigned int  SERVER_MSG_LEN = 100000;
constexpr unsigned char BUFFER_LEN     = 100;
constexpr unsigned char LF   = 0x0A;
constexpr unsigned char CR   = 0x0D;
constexpr unsigned char CRLF = 0xDA;
constexpr int ERROR = -1;

int sigTermFlag = false;
int sigIntFlag  = false;
int sigAlrmFlag = false;
int sigThreadTerminateFlag = false;

#if (1 == USE_AESD_CHAR_DEVICE)
    const char *ioc_seekto_str = "AESDCHAR_IOCSEEKTO:"; 
#endif

void *server_thread(void *args);
// Return the thread id in case of success and null otherwise.
pthread_t timestamp_thread_init(uint32_t strLen, pthread_mutex_t *fileMutex);
void *timestamp_thread(void *args);
static void signal_handler(int signalNo);
#if (1 == USE_AESD_CHAR_DEVICE)
    /** Extracts X, Y from the AESDChAR_IOCSEEKTO:X,Y command
     * @returns -1 on failure, 0 on sucess
    */
    int extract_ioctl_xy(char *str, uint32_t *x, uint32_t *y);
#endif


/*
 * struct sigaction {
        void     (*sa_handler)(int);
        void     (*sa_sigaction)(int, siginfo_t *, void *);
        sigset_t   sa_mask;
        int        sa_flags;
        void     (*sa_restorer)(void);
    };
*/

int main(int argc, char *argv[])
{
    int prog_status = EXIT_SUCCESS;
    pthread_t thread_id_list[THREAD_MAX];
    unsigned char i   =  1;
    int listenPort    = -1;
    struct sigaction signalsAct;
    pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;
    server ipServer("9000");
    struct sockaddr ipSockAddr;
    socklen_t len = sizeof(ipSockAddr);

    printf("ipServer socketfd: %d\n", ipServer.create(20));
    if (NULL != argv[1]) {
        if (0 == strcmp(argv[1], "-d")) {
            prog_status = daemon(0, 0);
            if (0 != prog_status) {
                perror("daemon():");
                exit(EXIT_FAILURE);
            }
        }
    }
    memset(&signalsAct, 0, sizeof(signalsAct));
    signalsAct.sa_handler = signal_handler;

    sigaction(SIGINT, &signalsAct, NULL);
    if (0 != prog_status) {
        perror("sigaction(SIGINT):");
        exit(EXIT_FAILURE);
    }
    sigaction(SIGTERM, &signalsAct, NULL);
    if (0 != prog_status) {
        perror("sigaction(SIGTERM):");
        exit(EXIT_FAILURE);
    }
/**
 * In case of \c USE_AESD_CHAR_DEVICE==1 the \c thread_id_list[0] will be empty,
 *  but that is fine since the array is already static and does not take extra space for it anyway.
 */
#if (0 == USE_AESD_CHAR_DEVICE)
    sigaction(SIGALRM, &signalsAct, NULL);
    if (0 != prog_status) {
        perror("sigaction(SIGALARM):");
        exit(EXIT_FAILURE);
    }
    thread_id_list[0] = timestamp_thread_init(BUFFER_LEN, &fileMutex);
    if (thread_id_list[0]) {
        printf("thread_id_list[0] = %lu\n", thread_id_list[0]);
    }
#endif
    while (1) {
        if (true  == sigTermFlag) {
            syslog(LOG_DEBUG, "Caught signal, exiting");
            sigIntFlag = false;
            sigThreadTerminateFlag = true;
            break;
        } else if (true == sigIntFlag) {
            syslog(LOG_DEBUG, "Caught signal, exiting");
            sigTermFlag = false;
            sigThreadTerminateFlag = true;
            break;
        }
        if (i < 100) {
            listenPort  = ipServer.newClientSocket(&ipSockAddr, &len);
            if ((listenPort > 0) &&
                (i < (THREAD_MAX + 1))) {
                ipServer.initServerThread(listenPort, server_thread,
                                          SERVER_MSG_LEN, &fileMutex);
                thread_id_list[i] = ipServer.getThreadId(i - 1);
                printf("Created new thread %lu\n\n", thread_id_list[i]);
                ++i;
                syslog(LOG_DEBUG, "Accepted connection from %s", ipServer.getIpAddr());
            }
        }
    }
#if (0 == USE_AESD_CHAR_DEVICE)
    for (int j = 0; j < i; ++j) {
#else // thread_id_list[0] is used for the timer thread which does not exist in this configuration
    for (int j = 1; j < i; ++j) {
#endif
        prog_status = pthread_join(thread_id_list[j], NULL);
        if (0 != prog_status) {
            errno = prog_status;
            printf("Thread %d, ", i);
            perror("pthread_join():");
        }
    }
    sigThreadTerminateFlag = false;
#if (0 == USE_AESD_CHAR_DEVICE) // No deletion needed for a device driver
    system("rm /var/tmp/aesdsocketdata.txt");
#endif
    return (prog_status);
}

static void signal_handler(int signalNo)
{
    if (SIGTERM == signalNo) {
        sigTermFlag = true;
    }
    if (SIGINT == signalNo) {
        sigIntFlag = true;
    }
#if (0 == USE_AESD_CHAR_DEVICE)
    if (SIGALRM == signalNo) {
        sigAlrmFlag = true;
    }
#endif
}

int extract_ioctl_xy(char *str, uint32_t *x, uint32_t *y)
{
    size_t i;
    char tmp[6] = {'\0'};

// Check the str starts with ioc_seekto_str and not just includes it
    for (i = 0; i < strlen(ioc_seekto_str); ++i) 
        if (str[i] != ioc_seekto_str[i])  
            return -1;
    
    *x = strtoul(str + i, NULL, 10);
// Here we check that the user didn't overflow the X argument 
    sprintf(tmp, "%d", *x);
    printf("tmp = %s, strlen(tmp) = %ld\n", tmp, strlen(tmp));
    if (strlen(tmp) >= 6) // X can't be 6-digits number
        return -1;

    *y = strtoul(str + i + strlen(tmp) + 1, NULL, 10); // AESDCHAR_IOCSEEKTO: + X + ','
    printf("str = %s, ioc_seekto_str = %s, x = %d, y = %d\n", 
        str, ioc_seekto_str, *x, *y);
    return 0;
}

void *server_thread(void *args)
{
    int numOfBytes = 0;
    char *fileBuffer =
        static_cast<char *>(malloc(sizeof(char) * SERVER_MSG_LEN));
    unsigned int i, j;
    unsigned char k = 0, noOfRetries = 10;
    int mutexRes = -1;
    struct server::thread_args *pThreadArgs =
        static_cast<struct server::thread_args *>(args);
    FILE *pFile = NULL;

#if (USE_AESD_CHAR_DEVICE == 1)
    uint32_t write_cmd, write_cmd_offset;
    write_cmd = write_cmd_offset = 0;
#endif

    memset(pThreadArgs->msg, 0, pThreadArgs->msgLen);
    memset(fileBuffer, 0, SERVER_MSG_LEN);
    do {
        if (true == sigThreadTerminateFlag) {
            pthread_mutex_unlock(pThreadArgs->fileMutex);
            break;
        }
        numOfBytes =
            recv(pThreadArgs->socketfd, pThreadArgs->msg, pThreadArgs->msgLen, 0);
        if (-1 == numOfBytes) {
            perror("recv():");
        }
        if (0 != numOfBytes) {
            i = j = 0;
            while ((i < pThreadArgs->msgLen) &&
                   ('\0' != pThreadArgs->msg[i])) {
                if ((LF != pThreadArgs->msg[i])   &&
                    (CR != pThreadArgs->msg[i])   &&
                    (CRLF != pThreadArgs->msg[i]) &&
                    ('\0' != pThreadArgs->msg[i])) {
                    fileBuffer[j++] = pThreadArgs->msg[i++];
                } else {
                    break;
                }
            }
            if (LF == pThreadArgs->msg[i]) {
                fileBuffer[j] = LF;
            } else if ((CR == pThreadArgs->msg[i++]) &&
                        (LF == pThreadArgs->msg[i])) {
                fileBuffer[j++] = CR;
                fileBuffer[j] = LF;
            } else if (CR  == pThreadArgs->msg[i]) {
                fileBuffer[j] = CR;
            } else if (CRLF == pThreadArgs->msg[i]) {
                fileBuffer[j] = CRLF;
            } else {
                fileBuffer[j] = LF;
            }
            mutexRes = pthread_mutex_lock(pThreadArgs->fileMutex);
            if (0 != mutexRes) {
                puts("Mutex locking Error");
                do {
                    usleep(10000);
                    printf("lock not ready, thread ID: %lu\n",
                        pthread_self());
                    mutexRes = pthread_mutex_lock(pThreadArgs->fileMutex);
                } while((0 != mutexRes) && (k++ < noOfRetries));
            } else {
                printf("Mutex locked, thread ID: %lu\n", pthread_self());
            }
            if (0 == mutexRes) {
                k = 0;  // restart the retries counter;
#if (1 == USE_AESD_CHAR_DEVICE)
                pFile = fopen("/dev/aesdchar", "r+");
                if (NULL == pFile)  {
                    perror("fopen():");
                    exit(EXIT_FAILURE);
                }
                // Check if it is ioctl command
                if (0 == extract_ioctl_xy(fileBuffer, &write_cmd, &write_cmd_offset)) {
                    syslog(LOG_DEBUG, "IOCTL");
                    struct aesd_seekto seekto = {write_cmd, write_cmd_offset};
                    printf("ioctl: %d\n", ioctl(fileno(pFile), AESDCHAR_IOCSEEKTO, &seekto));
                    
                } else { // If not; just read and write normally
                    fseek(pFile, 0, SEEK_END); // Point to the end of the file
                    fwrite(fileBuffer, strlen(fileBuffer), sizeof(char), pFile);
                    fseek(pFile, 0, SEEK_SET); // Point to the beginning of the file
                }
                memset(fileBuffer, '\0', SERVER_MSG_LEN);
                fread(fileBuffer, SERVER_MSG_LEN, sizeof(char), pFile);
                fclose(pFile);
#else
                pFile = fopen("/var/tmp/aesdsocketdata.txt", "a+");
                if (NULL == pFile)  {
                    perror("fopen():");
                    exit(EXIT_FAILURE);
                }
                fseek(pFile, 0, SEEK_END); // Point to the end of the file
                fwrite(fileBuffer, strlen(fileBuffer), sizeof(char), pFile);
                memset(fileBuffer, '\0', SERVER_MSG_LEN);
                fseek(pFile, 0, SEEK_SET); // Point to the beginning of the file
                fread(fileBuffer, SERVER_MSG_LEN, sizeof(char), pFile);
                fclose(pFile);
#endif
                mutexRes = pthread_mutex_unlock(pThreadArgs->fileMutex);
                if (0 != mutexRes) {
                    puts("Mutex unlocking Error");
                    do {
                        usleep(10000);
                        printf("unlock not ready, thread ID: %lu\n",
                            pthread_self());
                        mutexRes = pthread_mutex_unlock(pThreadArgs->fileMutex);
                    } while((0 != mutexRes) && (k++ < noOfRetries));
                } else {
                    printf("Mutex unlocked, thread ID: %lu\n", pthread_self());
                }
            }
            numOfBytes = send(pThreadArgs->socketfd, fileBuffer,
                            strlen(fileBuffer), 0);
            if (-1 == numOfBytes) {
                perror("send():");
            }
            memset(fileBuffer, '\0', SERVER_MSG_LEN);
        }
        memset(pThreadArgs->msg, 0, pThreadArgs->msgLen);
        usleep(500000); // sleep for 0.5 second
    } while (1);
    free(fileBuffer);
    pThreadArgs->isComplete = true;

    return EXIT_SUCCESS;
}

#if (0 == USE_AESD_CHAR_DEVICE)
pthread_t timestamp_thread_init(uint32_t strLen,
                                pthread_mutex_t *fileMutex)
{
    struct itimerval itv;
// Init args data structure
    struct timestamp_args *pTimestamp_args =
        static_cast<struct timestamp_args *>(malloc(sizeof(struct timestamp_args)));
    pTimestamp_args->sTimeStamp_RFC2822 =
        static_cast<char *>(malloc(strLen * sizeof(char)));
    pTimestamp_args->fileMutex = fileMutex;
    pTimestamp_args->timestamp_len = strLen;
// Create the actual thread
    pthread_create(&(pTimestamp_args->threadID), NULL,
                   timestamp_thread, static_cast<void *>(pTimestamp_args));
// Init timer
    itv.it_value.tv_sec     = 0;
    itv.it_value.tv_usec    = 10;
    itv.it_interval.tv_sec  = 10;
    itv.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &itv, NULL);

    return (pTimestamp_args->threadID);
}

void *timestamp_thread(void *args)
{
    struct timestamp_args *pTimestamp_args =
        static_cast<struct timestamp_args *>(args);
    uint16_t fileBufferSize =
        strlen("timestamp:") + (BUFFER_LEN * sizeof(char));
    char *fileBuffer =
        static_cast<char *>(malloc(fileBufferSize));
    pTimestamp_args->sTimeStamp_RFC2822[0] = {'\0'};
    fileBuffer[0] = {'\0'};
    unsigned char k = 0, noOfRetries = 20;
    time_t t;
    struct tm *timestamp;
    int mutexRes = -1;
    FILE *pFile = NULL;

    while (1) {
        if (true == sigThreadTerminateFlag) {
            printf("~Free timestamp thread\n\n");
            free(fileBuffer);
            free(pTimestamp_args->sTimeStamp_RFC2822);
            free(pTimestamp_args);
            break;
        }
        if (true == sigAlrmFlag) {
            sigAlrmFlag = false;
            t = time(NULL); // Time in seconds since the epoch
            timestamp = localtime(&t); // Converts t to tm format (min, hour, seconds...etc)
            if (NULL == timestamp) {
                perror("localtime:");
                free(fileBuffer);
                free(pTimestamp_args->sTimeStamp_RFC2822);
                free(pTimestamp_args);
                pthread_exit(NULL);
            }
            strftime(pTimestamp_args->sTimeStamp_RFC2822,
                    pTimestamp_args->timestamp_len, "%a, %d %b %Y %T %z", timestamp);
            memset(fileBuffer, 0, fileBufferSize);
            strcat(fileBuffer, "timestamp:");
            strcat(fileBuffer, pTimestamp_args->sTimeStamp_RFC2822);
            strcat(fileBuffer, "\n");
            printf("pTimestamp_args->sTimeStamp_RFC2822: %s\n",
                pTimestamp_args->sTimeStamp_RFC2822);
            printf("fileBuffer: %s\n", fileBuffer);
            mutexRes = pthread_mutex_lock(pTimestamp_args->fileMutex);
            if (0 != mutexRes) {
                puts("Mutex locking Error");
                do {
                    usleep(10000);
                    printf("lock not ready, thread ID: %lu\n",
                        pthread_self());
                    mutexRes = pthread_mutex_lock(pTimestamp_args->fileMutex);
                } while((0 != mutexRes) && (k++ < noOfRetries));
            } else {
                printf("Mutex locked, thread ID: %lu\n", pthread_self());
            }
            if (0 == mutexRes) {
                k = 0;  // restart the retries counter;
                pFile = fopen("/var/tmp/aesdsocketdata.txt", "a+");
                if (NULL == pFile)  {
                    perror("fopen():");
                    exit(EXIT_FAILURE);
                }
                fwrite(fileBuffer, strlen(fileBuffer), 1, pFile);  // Log to the file
                fflush(pFile);
                fclose(pFile);
                mutexRes = pthread_mutex_unlock(pTimestamp_args->fileMutex);
                if (0 != mutexRes) {
                    puts("Mutex unlocking Error");
                    do {
                        usleep(10000);
                        printf("unlock not ready, thread ID: %lu\n",
                            pthread_self());
                        mutexRes = pthread_mutex_unlock(pTimestamp_args->fileMutex);
                    } while((0 != mutexRes) && (k++ < noOfRetries));
                } else {
                    printf("Mutex unlocked, thread ID: %lu\n", pthread_self());
                }
            }
        }
        sleep(5);
    }
    return(EXIT_SUCCESS);
}
#endif
