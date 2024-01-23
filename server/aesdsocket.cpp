#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "server.h"
#include "queue.h"
#include "timestamp.h"

constexpr unsigned int SERVER_MSG_LEN  = 100000;
constexpr unsigned char BUFFER_LEN     = 100;
constexpr unsigned char IP_ADDR_LEN    = 20;
constexpr unsigned char LF   = 0x0A;
constexpr unsigned char CR   = 0x0D;
constexpr unsigned char CRLF = 0xDA;
constexpr int ERROR = -1;

int sigTermFlag = false;
int sigIntFlag  = false;
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
    char syslogBuffer[BUFFER_LEN] = {'\0'};
    unsigned char i   = 1;
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
    portList[0] = timestamp_thread_init(BUFFER_LEN, &fileMutex, pFile);
    if (NULL != portList[0]) {
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
                strcpy(syslogBuffer, "Accepted connection from ");
                ipServer.getIpAddr(syslogBuffer + strlen(syslogBuffer));
                syslog(LOG_DEBUG, syslogBuffer);
            }
        }
    }
    for (int j = 0; j < i; ++j) {
        prog_status = pthread_join(portList[j], NULL);
        if (0 != prog_status) {
            errno = prog_status;
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
}

pthread_t timestamp_thread_init(uint32_t strLen, 
                                pthread_mutex_t *fileMutex, FILE *pFile)
{
// Init args data structure
    struct timestamp_args *pTimestamp_args = 
        static_cast<struct timestamp_args *>(malloc(sizeof(struct timestamp_args)));
    pTimestamp_args->sTimeStamp_RFC2822 = 
        static_cast<char *>(malloc(strLen * sizeof(char)));     
    pTimestamp_args->fileMutex = fileMutex;
    pTimestamp_args->pFile     = pFile;
    pTimestamp_args->timestamp_len = strLen;
// Creat the actual thread
    pthread_create(&(pTimestamp_args->threadID), NULL, 
                   timestamp_thread, pTimestamp_args);
    return (pTimestamp_args->threadID);
}

void *server_thread(void *args)
{
    int numOfBytes = 0;
    char *fileBuffer = 
        static_cast<char *>(malloc(sizeof(char) * SERVER_MSG_LEN));
    unsigned int i, j;
    struct server::thread_args *pThreadArgs =
        static_cast<struct server::thread_args *>(args);

    memset(pThreadArgs->msg, 0, pThreadArgs->msgLen);
    memset(fileBuffer, 0, SERVER_MSG_LEN);
    do {
        if (true == sigThreadTerminateFlag) {
            break;
        }
        numOfBytes =
            recv(pThreadArgs->socketfd, pThreadArgs->msg, pThreadArgs->msgLen, 0);
        if (-1 == numOfBytes) {
            perror("recv():");
        // /* Since I am not doing anything in case a thread exited with a failure 
        //     I do not provide an actual argument. */
            pthread_exit(nullptr);  
        }
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
                auto mutexRes = pthread_mutex_lock(pThreadArgs->fileMutex);
                if (0 != mutexRes) {
                    if (EBUSY != mutexRes) {
                        puts("Mutex locking Error");
                        pthread_exit(nullptr);
                    } else {
                        do {
                            auto nsleep = usleep(1000);
                            printf("lock not ready, thread ID: %lu, not slept: %d\n", 
                                pthread_self(), nsleep);
                            mutexRes = pthread_mutex_lock(pThreadArgs->fileMutex);
                        } while(0 != mutexRes);
                    }
                } else {
                    printf("Mutex locked, thread ID: %lu\n", pthread_self());
                }
                fseek(pThreadArgs->pFile, 0, SEEK_END);
                fwrite(fileBuffer, strlen(fileBuffer), 1, pThreadArgs->pFile);
                fflush(pThreadArgs->pFile);
                memset(fileBuffer, '\0', SERVER_MSG_LEN);
                rewind(pThreadArgs->pFile);
                fread(fileBuffer, SERVER_MSG_LEN, 1, pThreadArgs->pFile);
                fflush(pThreadArgs->pFile);
                mutexRes = pthread_mutex_unlock(pThreadArgs->fileMutex);
                if (0 != mutexRes) {
                    if (EBUSY != mutexRes) {
                        puts("Mutex unlocking Error");
                        pthread_exit(nullptr);
                    } else {
                        do {
                            auto nsleep = usleep(1000);
                            printf("lock not ready, thread ID: %lu, not slept: %d\n", 
                                pthread_self(), nsleep);
                            mutexRes = pthread_mutex_unlock(pThreadArgs->fileMutex);
                        } while(0 != mutexRes);
                    }
                } else {
                    printf("Mutex unlocked, thread ID: %lu\n", pthread_self());
                }
                numOfBytes = send(pThreadArgs->socketfd, fileBuffer,
                                strlen(fileBuffer), 0);
                if (-1 == numOfBytes) {
                    perror("send():");
                    // /* Since I am not doing anything in case a thread exited with a failure 
                    //     I do not provide an actual argument. */
                    pthread_exit(nullptr); 
                }
                memset(fileBuffer, '\0', SERVER_MSG_LEN);
            }
        }
        memset(pThreadArgs->msg, 0, pThreadArgs->msgLen);
    } while (1);
    free(fileBuffer);
    pThreadArgs->isComplete = true;

    return EXIT_SUCCESS;
}

void *timestamp_thread(void *args)
{
    struct timestamp_args *pTimestamp_args = 
        static_cast<struct timestamp_args *>(args);
    uint16_t fileBufferSize = 
        strlen("timestamp:") + (pTimestamp_args->timestamp_len * sizeof(char));
    char *fileBuffer = 
        static_cast<char *>(malloc(fileBufferSize));
    pTimestamp_args->sTimeStamp_RFC2822[0] = {'\0'};
    fileBuffer[0] = {'\0'};
    time_t t;
    struct tm *timestamp;

    t = time(NULL); // Time in seconds since the epoch
    timestamp = localtime(&t); // Converts t to tm format (min, hour, seconds...etc)
    if (NULL == timestamp) {
        perror("localtime:");
        exit(EXIT_FAILURE);
    }
    strftime(pTimestamp_args->sTimeStamp_RFC2822, 
             pTimestamp_args->timestamp_len, "%a, %d %b %Y %T %z", timestamp);
    strcat(fileBuffer, "timestamp:");
    strcat(fileBuffer, pTimestamp_args->sTimeStamp_RFC2822);
    strcat(fileBuffer, "\n");
    printf("pTimestamp_args->sTimeStamp_RFC2822: %s\n", 
        pTimestamp_args->sTimeStamp_RFC2822);
    printf("fileBuffer: %s\n", fileBuffer);

    auto mutexRes = pthread_mutex_lock(pTimestamp_args->fileMutex);
    if (0 != mutexRes) {
        if (EBUSY != mutexRes) {
            puts("Mutex locking Error");
            pthread_exit(nullptr);
        } else {
            do {
                auto nsleep = usleep(1000);
                printf("lock not ready, thread ID: %lu, not slept: %d\n", 
                    pthread_self(), nsleep);
                mutexRes = pthread_mutex_lock(pTimestamp_args->fileMutex);
            } while(0 != mutexRes);
        }
    } else {
        printf("Mutex locked, thread ID: %lu\n", pthread_self());
    }
    fwrite(fileBuffer, strlen(fileBuffer), 1, pTimestamp_args->pFile);  // Log to the file
    // fputc('\n', pTimestamp_args->pFile);
    // fprintf(pTimestamp_args->pFile, "timestamp:");
    fflush(pTimestamp_args->pFile);
    mutexRes = pthread_mutex_unlock(pTimestamp_args->fileMutex);
    if (0 != mutexRes) {
        if (EBUSY != mutexRes) {
            puts("Mutex unlocking Error");
            pthread_exit(nullptr);
        } else {
            do {
                auto nsleep = usleep(1000);
                printf("lock not ready, thread ID: %lu, not slept: %d\n", 
                    pthread_self(), nsleep);
                mutexRes = pthread_mutex_unlock(pTimestamp_args->fileMutex);
            } while(0 != mutexRes);
        }
    } else {
        printf("Mutex unlocked, thread ID: %lu\n", pthread_self());
    }     

    return(EXIT_SUCCESS);     
}
