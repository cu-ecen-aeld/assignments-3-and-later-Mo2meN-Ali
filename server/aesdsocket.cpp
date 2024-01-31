#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "server.h"
#include "queue.h"
#include "timestamp.h"

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

void *server_thread(void *args);
// Return the thread id in case of success and null otherwise.
pthread_t timestamp_thread_init(uint32_t strLen, 
                                pthread_mutex_t *fileMutex, FILE *pFile);
void *timestamp_thread(void *args);
static void signal_handler(int signalNo);

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
    pthread_t portList[THREAD_MAX];
    unsigned char i   =  1;
    int listenPort    = -1;
    struct sigaction signalsAct;
    pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;
    // server defServer;
    server ipServer("9000");
    struct sockaddr ipSockAddr;
    socklen_t len = sizeof(ipSockAddr);
    FILE *pFile = NULL;

    pFile = fopen("/var/tmp/aesdsocketdata.txt", "w+");
    if (NULL == pFile)  {
        perror("fopen():");
        exit(EXIT_FAILURE);
    }

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
    sigaction(SIGALRM, &signalsAct, NULL);
    if (0 != prog_status) {
        perror("sigaction(SIGALARM):");
        exit(EXIT_FAILURE);
    }
    portList[0] = timestamp_thread_init(BUFFER_LEN, &fileMutex, pFile);
    if (0 < portList[0]) {
        printf("portlist[0] = %lu\n", portList[0]);
    }
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
                                          SERVER_MSG_LEN, pFile, &fileMutex);
                portList[i] = ipServer.getThreadId(i - 1);
                printf("Created new thread %lu\n\n", portList[i]);
                ++i;
                syslog(LOG_DEBUG, "Accepted connection from %s", ipServer.getIpAddr());
            }
        }
    }
    for (int j = 0; j < i; ++j) {
        prog_status = pthread_join(portList[j], NULL);
        if (0 != prog_status) {
            errno = prog_status;
            printf("Thread %d, ", i);
            perror("pthread_join():");
        }
    }
    sigThreadTerminateFlag = false;
    fclose(pFile);
    system("rm /var/tmp/aesdsocketdata.txt");

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
    if (SIGALRM == signalNo) {
        sigAlrmFlag = true;
    }
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
        // printf("pThreadArgs->msgLen = %d, numOfBytes received = %d\n\n",
        //         pThreadArgs->msgLen, numOfBytes);
        if (0 != numOfBytes) {
            i = j = 0;
            while ((i < pThreadArgs->msgLen) &&
                   ('\0' != pThreadArgs->msg[i])) {
                while ((LF != pThreadArgs->msg[i])   &&
                       (CR != pThreadArgs->msg[i])   &&
                       (CRLF != pThreadArgs->msg[i]) &&
                       ('\0' != pThreadArgs->msg[i])) {
                    fileBuffer[j++] = pThreadArgs->msg[i++];
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
                j = 0;
                ++i;
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
                    fseek(pThreadArgs->pFile, 0, SEEK_END);
                    fwrite(fileBuffer, strlen(fileBuffer), 1, pThreadArgs->pFile);
                    memset(fileBuffer, '\0', SERVER_MSG_LEN);
                    rewind(pThreadArgs->pFile);
                    fread(fileBuffer, SERVER_MSG_LEN, 1, pThreadArgs->pFile);
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
        }
        memset(pThreadArgs->msg, 0, pThreadArgs->msgLen);
        usleep(500000); // sleep for 0.5 second
    } while (1);
    free(fileBuffer);
    pThreadArgs->isComplete = true;

    return EXIT_SUCCESS;
}

pthread_t timestamp_thread_init(uint32_t strLen, 
                                pthread_mutex_t *fileMutex, FILE *pFile)
{
    struct itimerval itv;
// Init args data structure
    struct timestamp_args *pTimestamp_args = 
        static_cast<struct timestamp_args *>(malloc(sizeof(struct timestamp_args)));
    pTimestamp_args->sTimeStamp_RFC2822 = 
        static_cast<char *>(malloc(strLen * sizeof(char)));     
    pTimestamp_args->fileMutex = fileMutex;
    pTimestamp_args->pFile     = pFile;
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
                fwrite(fileBuffer, strlen(fileBuffer), 1, pTimestamp_args->pFile);  // Log to the file
                fflush(pTimestamp_args->pFile);
                pthread_mutex_unlock(pTimestamp_args->fileMutex);
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
