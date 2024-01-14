#ifndef SERVER_H_
#define SERVER_H_

#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstdio>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {

constexpr int THREAD_MAX = 20;

/*
 * struct addrinfo {
        int              ai_flags;
        int              ai_family;
        int              ai_socktype;
        int              ai_protocol;
        socklen_t        ai_addrlen;
        struct sockaddr *ai_addr;
        char            *ai_canonname;
        struct addrinfo *ai_next;
    };
*/

class server {
public:
    struct thread_args {
        bool isComplete;      // Thread completion status flag
        int socketfd;         // The socket representing this user
        unsigned int  msgLen; // Size of the buffer
        char *msg;            // The buffer to hold messages
        pthread_t threadId;
        FILE *pFile;
    };

    server():
        service(), hints(), resInfo(nullptr), socketfd(-1), numOfThreads(0)
    { 
        sprintf(service, "%d", (rand() % 65335) + 1025); // Random port
        hints.ai_family   = AF_UNSPEC;   // IPv4 or IPv6 
        hints.ai_flags    = AI_PASSIVE;  // Fill my IP for me
        hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    }

    server(const char *port): 
        hints(), resInfo(nullptr), socketfd(-1), numOfThreads(0)
    {    
        strncpy(service, port, sizeof(service));
        hints.ai_family   = AF_UNSPEC;   // IPv4 or IPv6 
        hints.ai_flags    = AI_PASSIVE;  // Fill my IP for me
        hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    }

    server(char *n, char *s, 
           struct addrinfo h, struct addrinfo *r):
        hints(h), resInfo(r), socketfd(-1), numOfThreads(0)
    { 
        strncpy(service, s, sizeof(service)); 
    } 

    ~server() 
    {
        if (-1 != socketfd) { // Close the server socket
            // shutdown(socketfd, SHUT_RDWR);
            close(socketfd);
        }
        if (true == isAllocated) { // Clean getaddrinfo() struct
            freeaddrinfo(resInfo);
        }
        for (int i = 0; i < numOfThreads; ++i) { // Clean memory after each thread
            printf("~Free thread %d\n\n", i);
            free(pThreadArgs[i]->msg);
            free(pThreadArgs[i]);
        }
    }

// Returns the result of getaddrinfo() and potentially the *res pointer    
    virtual int create(int backlog)
    {
        int status;

        status = getaddrinfo(NULL, service, &hints, &resInfo);
        if (0 != status) {
            fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
            return -1;
        }
        isAllocated = true;
        socketfd = socket(resInfo->ai_family, resInfo->ai_socktype, resInfo->ai_protocol);
        if (-1 == socketfd) {    
            perror("socket():");
            return socketfd;
        }
        status = bind(socketfd, resInfo->ai_addr, resInfo->ai_addrlen);
        if (0 != status) {
            perror("bind():");
            return status;
        }
        status = listen(socketfd, backlog);
        if (0 != status) {
            perror("listen():");
            return status;
        }
        return socketfd;
    }

    int newClientSocket(struct sockaddr *listenSockAddr, socklen_t *listenSockLen)
    {
        int commSocket = -1; // The socket that is going to be used by send annd receive
        // fcntl(socketfd, F_SETFL, O_NONBLOCK); 
        commSocket = accept(socketfd, listenSockAddr, listenSockLen);
        /* Debugging! */
        printf("Accept(), commSocket = %d\n", commSocket);
        if (-1 == commSocket) {
            perror("accept():");
            return commSocket;
        }

        return commSocket;
    }

    int sendMsg(int commSocket, const char *msg)
    {
        return send(commSocket, static_cast<const void *>(msg), strlen(msg), 0);
    }

    int recvMsg(int commSocket, char *msg, int msgLen)
    {
        return recv(commSocket, msg, msgLen, 0);
    }

    int getIpAddr(char *ipAddr)
    {
        if (AF_INET6 == resInfo->ai_family) {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)resInfo->ai_addr;
            inet_ntop(resInfo->ai_family, &(ipv6->sin6_addr), ipAddr, sizeof(ipAddr));
        } else if (AF_INET == resInfo->ai_family) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)resInfo->ai_addr;
            inet_ntop(resInfo->ai_family, &(ipv4->sin_addr), ipAddr, sizeof(ipAddr));
        } else {
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    int initServerThread(int commSocket, void *server_thread(void *threadArgs), 
                         int msgLen, FILE *userFile)
    {
        int status = -1;

        if (numOfThreads >= THREAD_MAX) {
            return status;
        }
        pThreadArgs[numOfThreads] = static_cast<struct thread_args *>(malloc(sizeof(struct thread_args)));
        if (NULL != pThreadArgs) {
            pThreadArgs[numOfThreads]->isComplete = false;
            pThreadArgs[numOfThreads]->socketfd   = commSocket;
            pThreadArgs[numOfThreads]->msgLen     = msgLen;
            pThreadArgs[numOfThreads]->msg        = static_cast<char *>(malloc(sizeof(char) * msgLen));
            memset(pThreadArgs[numOfThreads]->msg, 0, msgLen);
            if (NULL == pThreadArgs[numOfThreads]->msg) { // If malloc failed.
                return status;
            }
        } else {    // If malloc failed.
            return status;
        }
        pThreadArgs[numOfThreads]->pFile = userFile;
        status = pthread_create(&pThreadArgs[numOfThreads]->threadId, NULL, 
                    server_thread, static_cast<void *>(pThreadArgs[numOfThreads]));
        if (0 != status) {
            errno = status;
            perror("pthread_create():");
            return -1;
        }
        ++numOfThreads;
        printf("Number of thread(s) equal %d\n\n", numOfThreads);
        return status;
    }

    pthread_t getThreadId(int threadNum)
    {
        return pThreadArgs[threadNum]->threadId;
    }

    bool threadCompletionStatus(pthread_t threadID) 
    {
        for (auto i = 0; i < THREAD_MAX; ++i) {
            if (0 != pthread_equal(threadID, pThreadArgs[i]->threadId)) {
                return (pThreadArgs[i])->isComplete;
            }
        }
        return false;
    }

private:
    char service[5];
    struct addrinfo hints;
    struct addrinfo *resInfo;
    int socketfd;
    bool isAllocated = false;
    struct thread_args *pThreadArgs[THREAD_MAX];
    int numOfThreads;
};

} 
#endif /* __cplusplus */
#endif /* SERVER_H_ */