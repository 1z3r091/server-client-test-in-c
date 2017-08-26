#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	SOCKADDR_IN servAddr;
	int iResult;

	char message[30] = {0,};
	int strLen;

	if(argc!=3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		ErrorHandling("WSAStartup() errer!");
	}

	ConnectSocket=socket(AF_INET, SOCK_STREAM, 0);
	if(ConnectSocket==INVALID_SOCKET)
		ErrorHandling("ConnectSocketet() error!");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=inet_addr(argv[1]);
	servAddr.sin_port=htons(atoi(argv[2]));

    // connect to server
	iResult = connect(ConnectSocket, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (iResult == SOCKET_ERROR) {
		ErrorHandling("connect() error!");
    }

    // Receive until the peer closes the connection
	do {
        printf("Client: ");
        gets(message);

        if (strcmp(message,"exit") == 0) break;

         // Send an initial buffer
        iResult = send( ConnectSocket, message, (int)strlen(message), 0 );
        if (iResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

        printf("Bytes Sent: %ld\n", iResult);

        /*
        // shutdown the connection since no more data will be sent
        iResult = shutdown(ConnectSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }
        */

        iResult = recv(ConnectSocket, message, sizeof(message)-1, 0);
        if(iResult == -1)
            ErrorHandling("read() error!");
        message[iResult] = '\0';
        printf("Message from server: %s \n", message);
	} while(1);


    // cleanup
	printf("Disconnected");
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
