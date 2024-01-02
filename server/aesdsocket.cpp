#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "server.h"

constexpr unsigned short FILE_BUFFER_LEN = 100;
constexpr unsigned char MSG_LEN     = 100;
constexpr unsigned char IP_ADDR_LEN = 20;
constexpr char LF   = 0x0A;
constexpr int ERROR = -1;

int sigTermFlag = false;
int sigIntFlag  = false;
int sigThreadTerminateFlag = false;

void *server_thread(void *args);
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
    char syslogBuffer[MSG_LEN]     = {'\0'};
    unsigned char i   = 0;
    int listenPort    = -1;
    FILE *pFile       = NULL;
    struct sigaction signalsAct;
    // server defServer; 
    server ipServer("9000");
    struct sockaddr ipSockAddr;
    socklen_t len = sizeof(ipSockAddr);

    printf("ipServer socketfd: %d\n", ipServer.create(20));
    
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
    // pFile = fopen("/var/tmp/aesdsocketdata.txt", "a");
    // if (NULL == pFile)  {
    //     puts("fopen(): failed");
    //     exit(EXIT_FAILURE);
    // }
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
                (i < THREAD_MAX)) {
                ipServer.initServerThread(listenPort, server_thread, MSG_LEN);
                portList[i] = ipServer.getThreadId(i);
                printf("Created new thread %lu\n\n", ipServer.getThreadId(i));
                ++i;
                strcpy(syslogBuffer, "Accepted connection from ");
                ipServer.getIpAddr(syslogBuffer + strlen(syslogBuffer));
                syslog(LOG_DEBUG, syslogBuffer);
            }
        } 
    }
    for (int j = 0; j < i; ++j) {
        // printf("Searching for thread %lu with j = %d\n", portList[j], j);
        // printf("Thread join: %lu, completion status: %d\n\n", 
        //     portList[j], ipServer.threadCompletionStatus(portList[j]));
        prog_status = pthread_join(portList[j], NULL);
        if (0 != prog_status) {
            errno = prog_status;
            perror("pthread_join():");
        }
    }
    sigThreadTerminateFlag = false;
    // fclose(pFile);
    return (prog_status);
}

void *server_thread(void *args)
{
    int numOfReceivedBytes = 0;
    char fileBuffer[FILE_BUFFER_LEN] = {'\0'};
    int i, j;
    struct server::thread_args *pThreadArgs = 
        static_cast<struct server::thread_args *>(args);

    FILE *pFile = NULL;
    pFile = pFile; // To avoid the warning
    pFile = fopen("/var/tmp/aesdsocketdata.txt", "w");
    if (NULL == pFile)  {
        puts("fopen(): failed");
        exit(EXIT_FAILURE);
    }
    memset(pThreadArgs->msg, 0, pThreadArgs->msgLen);
    do { 
        if (true == sigThreadTerminateFlag) {
            break;
        }
        numOfReceivedBytes = 
            recv(pThreadArgs->socketfd, pThreadArgs->msg, pThreadArgs->msgLen, 0);
        if (0 != numOfReceivedBytes) {
            i = j = 0;
            // printf("pThreadArgs->msgLen = %d, numOfReceivedBytes = %d\n\n", 
            //     pThreadArgs->msgLen, numOfReceivedBytes);
            while ((i < pThreadArgs->msgLen) && 
                   ('\0' != pThreadArgs->msg[i])) {
                while ((LF  != pThreadArgs->msg[i]) && 
                       ('\0' != pThreadArgs->msg[i])) {
                    fileBuffer[j] = pThreadArgs->msg[i];
                    ++i;
                    ++j;
                }
                fileBuffer[j] = ' ';
                j = 0;
                ++i;
                // printf("fileBuffer = %s, pThreadsArgs->msg = %s\n\n", 
                //     fileBuffer, pThreadArgs->msg);
                fprintf(pFile, "%s", fileBuffer);
                fflush(pFile);
                memset(fileBuffer, 0, FILE_BUFFER_LEN);
            }
        }
        memset(pThreadArgs->msg, 0, pThreadArgs->msgLen);
    } while (1);
    pThreadArgs->isComplete = true;
    fclose(pFile);
    system("rm /var/tmp/aesdsocketdata.txt");
    return EXIT_SUCCESS;
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
