#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "server.h"

constexpr unsigned int SERVER_MSG_LEN  = 100000;
constexpr unsigned char BUFFER_LEN     = 100;
constexpr unsigned char IP_ADDR_LEN    = 20;
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
    char syslogBuffer[BUFFER_LEN] = {'\0'};
    unsigned char i   = 0;
    int listenPort    = -1;
    struct sigaction signalsAct;
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
                ipServer.initServerThread(listenPort, server_thread, SERVER_MSG_LEN, pFile);
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

void *server_thread(void *args)
{
    int numOfBytes = 0;
    char *fileBuffer = 
        static_cast<char *>(malloc(sizeof(char) * SERVER_MSG_LEN));
    unsigned int i, j, k = 0;
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
        }
        if (0 != numOfBytes) {
            i = j = k = 0;
            // printf("pThreadArgs->msgLen = %d, numOfBytes received = %d\n\n",
            //     pThreadArgs->msgLen, numOfBytes);
            while ((i < pThreadArgs->msgLen) &&
                   ('\0' != pThreadArgs->msg[i])) {
                while ((LF  != pThreadArgs->msg[i]) &&
                       ('\0' != pThreadArgs->msg[i])) {
                    fileBuffer[j++] = pThreadArgs->msg[i++];
                }
                fileBuffer[j++] = LF;
                j = 0;
                ++i;
                fseek(pThreadArgs->pFile, 0, SEEK_END);
                // printf("fileBuffer = %s, pThreadsArgs->msg = %s\n\n",
                //     fileBuffer, pThreadArgs->msg);
                fprintf(pThreadArgs->pFile, "%s", fileBuffer);
                fflush(pThreadArgs->pFile);
                memset(fileBuffer, '\0', SERVER_MSG_LEN);
                rewind(pThreadArgs->pFile);
                while (j < SERVER_MSG_LEN) {
                    fileBuffer[j] = fgetc(pThreadArgs->pFile);
                    if (EOF == fileBuffer[j]) {
                        break;
                    }
                    ++j;
                }
                fflush(pThreadArgs->pFile);
                fileBuffer[j] = '\0';
                numOfBytes = send(pThreadArgs->socketfd, fileBuffer,
                                strlen(fileBuffer), 0);
                if (-1 == numOfBytes) {
                    perror("send():");
                }
                // printf("numOfBytes sent: %d, strlen(fileBuffer) = %lu\n",
                //     numOfBytes, strlen(fileBuffer));
                j = 0;
                memset(fileBuffer, '\0', SERVER_MSG_LEN);
            }
        }
        memset(pThreadArgs->msg, 0, pThreadArgs->msgLen);
    } while (1);
    free(fileBuffer);
    pThreadArgs->isComplete = true;

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
