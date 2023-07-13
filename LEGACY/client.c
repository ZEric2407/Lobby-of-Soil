#include <stdio.h>
#include <WinSock2.h>
#include <sys/types.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main(){
    WORD wVersReq;
    WSADATA wsaData;

    wVersReq = MAKEWORD(2, 2);

    int err = WSAStartup(wVersReq, &wsaData);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
        exit(1);
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        printf("Winsock.dll not found or version unsupported\n");
        WSACleanup();
        exit(1);
    }
    else
        printf("Winsock 2.2 dll successfully located\n");
  
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1) {
        fprintf(stderr, "Socket failed to initialize.");
        exit(-1);
    }
    
    struct sockaddr_in address;
    address.sin_port = htons(8080);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");

    int connectErr = connect(socketFD, (struct sockaddr *) &address, sizeof(address));
    if (connectErr != -1) printf("Connection successful at %s\n", inet_ntoa(address.sin_addr));
    else{
        printf("Connection failed with code: %d\n", connectErr);
        exit(-1);
    }
    
    char buffer[500];
    recv(socketFD, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);

    fgets(buffer, 500, stdin);
    send(socketFD, buffer, sizeof(buffer), 0);

    closesocket(socketFD);
    WSACleanup();
    return 0;
}