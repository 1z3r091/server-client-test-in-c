#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <sys/types.h>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

void error_handling(char *message);

int main(int argc, char **argv)
{
    WSADATA wsaData;
    int iResult;

    SOCKET ClientSocket;
    SOCKET ListenSocket;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    int clnt_addr_size;
    char message[30];//"Hello World!\n";

    if(argc!=2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wprintf(L"WSAStartup failed: %d\n", iResult);
        return 1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    // Create a SOCKET for connecting to server
    ListenSocket=socket(AF_INET, SOCK_STREAM, 0);
    if(ListenSocket == INVALID_SOCKET) {
        printf("%d", WSAGetLastError());
        error_handling("socket() error");
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    if (iResult == SOCKET_ERROR) {
        error_handling("bind() error");
        WSACleanup();
        return 1;
    }

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        error_handling("listen() error");
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    clnt_addr_size=sizeof(clnt_addr);
    ClientSocket = accept(ListenSocket, (struct sockaddr*)&clnt_addr,&clnt_addr_size);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    // closesocket(ListenSocket);

    // Receive until the peer shuts down the connection
    do {
        iResult = recv(ClientSocket, message, sizeof(message)-1, 0);
        message[iResult] = '\0';
        printf("Message from Client : %s \n", message);
        if (iResult > 0) {
            printf("Server: ");
            gets(message);
            printf("Bytes received: %d\n", iResult);
            iSendResult = send( ClientSocket, message, (int)strlen(message), 0 );
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes Sent: %ld\n", iResult);
        }  else if (iResult == 0) {
            // printf("Connection closing...\n");
            strcpy(message,"");
        }  else  {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while (1);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    printf("Disconnected");
    close(ClientSocket); /* 연결 종료 */
    WSACleanup();

    return 0;
}



    void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);

    }
