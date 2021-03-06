#include <iostream>
#include "httpd.h"
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "HandleTCPClient.h"
#include "DieWithError.h"
#include <pthread.h>

#define MAXPENDING 5    /* Maximum outstanding connection requests */


using namespace std;

void *ThreadMain(void *arg);

struct ThreadArgs {
  int clntSock;
  string doc_root;
};

void start_httpd(unsigned short port, string doc_root)
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int clntLen;            /* Length of client address data structure */

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
      
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(port);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, 
                               &clntLen)) < 0)
            DieWithError("accept() failed");

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

        struct timeval timeout;      
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        if (setsockopt (clntSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
           DieWithError("setsockopt failed\n");

        if (setsockopt (clntSock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
           DieWithError("setsockopt failed\n");

        /* clntSock is connected to a client! */


        struct ThreadArgs *threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs));
        if (threadArgs == NULL)
          DieWithError("malloc() failed");
        threadArgs->clntSock = clntSock;
        threadArgs->doc_root = doc_root;

	pthread_t threadID;
        int returnValue = pthread_create(&threadID, NULL, ThreadMain, threadArgs);
        if (returnValue != 0)
          DieWithError("pthread_create() failed");
    }
    /* NOT REACHED */
}

void *ThreadMain(void *threadArgs) {
  pthread_detach(pthread_self());

  int clntSock = ((struct ThreadArgs *) threadArgs)->clntSock;
  string doc_root = ((struct ThreadArgs *) threadArgs)->doc_root;
  free(threadArgs);

  HandleTCPClient(clntSock, doc_root);

  return (NULL);
}
