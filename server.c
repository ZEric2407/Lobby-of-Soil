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
    else printf("Winsock 2.2 dll successfully located\n");

    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1) {
        fprintf(stderr, "Socket failed to initialize.");
        exit(-1);
    }

    struct sockaddr_in address;
    address.sin_port = htons(8080);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt));
    
    int bindStatus = bind(socketFD, (struct sockaddr *) &address, sizeof(address));
    if (bindStatus == -1) {
        fprintf(stderr, "Failed to bind socket.");
        exit(-1);
    } else {
        printf("Socket binded at %s\n", inet_ntoa(address.sin_addr));
    }

    listen(socketFD, 4);
    int client = accept(socketFD, NULL, NULL);
    printf("Connection Found\n");

    char message[20] = "Hua Sheng\0";
    send(client, message, sizeof(message), 0);

    char buffer[500];
    recv(client, buffer, sizeof(buffer), 0);
    printf("Client: %s", buffer);

    closesocket(socketFD);
    WSACleanup();
    return 0;
}