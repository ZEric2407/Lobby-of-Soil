#include <stdio.h>
#include <WinSock2.h>
#include <sys/types.h>
#include <WS2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

struct clientNode{
    int socketFD;
    int userID;
    char name[32];
    struct clientNode* next;
    HANDLE threadHandle;
} ADMIN;

struct clientNode* HEAD;
int numClients = 0;

struct clientNode* addNode(int sockFD, int uID, char name[]){
    struct clientNode* newNode = (struct clientNode*) malloc(sizeof(struct clientNode));
    if (newNode == NULL){
        fprintf(stderr, "Unable to create new client node.");
        return NULL;
    }

    strcpy(newNode->name, name);
    newNode->userID = uID;
    newNode->socketFD = sockFD;
    
    if (HEAD == NULL) newNode->next = NULL;
    else newNode->next = HEAD;

    HEAD = newNode;
    numClients++;
    puts("New Node Created");
    return newNode;
}

int removeNode(int uid){
    struct clientNode* currNode = HEAD;
    
    if (HEAD->userID == uid){
        currNode = HEAD->next;
        free(HEAD);
        HEAD = currNode;
        numClients--;
        return 0;
    }

    for (int i = 0; i < numClients; i++){
        if (currNode->next->userID == uid) {
            struct clientNode* tmpNode = currNode->next;
            currNode->next = currNode->next->next;
            free(tmpNode);
            tmpNode = NULL;
            numClients--;
        }
    }

    fprintf(stderr, "UserID not found");
    return -1;
}

int sendMessage(char *message, int uID){
    struct clientNode* currNode = HEAD;
    puts("Sending Message");
    for (int i = 0; i < numClients; i++){
        int sendStatus = send(currNode->socketFD, message, strlen(message), 0);
        if (sendStatus == -1) fprintf(stderr, "Error while sending message.\n");
        printf("Message sent to %s, UID = %d\n", currNode->name, currNode->userID);
        currNode = currNode->next;
    }
    return 0;
}

DWORD WINAPI handleClient(void *arg){
    int leaveIndicator = 0;
    char buffer[256];
    puts("Thread in action");

    char greetings[256];
    strcpy(greetings, ((struct clientNode*) arg)->name);
    strcat(greetings, " has joined the lobby.");
    sendMessage(greetings, ADMIN.userID);

    while(1){
        puts("Entered thread loop");
        if (leaveIndicator) break;
        int receiveStatus = recv(((struct clientNode*) arg)->socketFD, buffer, sizeof(buffer), 0);
        if (receiveStatus > 0 && strlen(buffer) > 0) {
            puts("Message Received");
            sendMessage(buffer, ((struct clientNode*) arg)->userID);
        } else {
            fprintf(stderr, "Error while receiving data: %d\n", receiveStatus);
            break;
        }
        strcpy(buffer, "");
    }

    puts("Broken out of thread loop");

    char farewell[256];
    strcpy(((struct clientNode*) arg)->name, farewell);
    strcat(farewell, " has left the lobby.");
    sendMessage(farewell, ADMIN.userID);

    closesocket(((struct clientNode*) arg)->socketFD);
    CloseHandle(((struct clientNode*) arg)->threadHandle);
    removeNode(((struct clientNode*) arg)->userID);
    return 0;
}

int main(){
    ADMIN.userID = -1;
    strcpy(ADMIN.name, "SERVER");

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

    DWORD threadID;
    while (1) {
        int client = accept(socketFD, NULL, NULL);
        printf("Connection Found\n");
        char buffer[256];

        recv(client, buffer, sizeof(buffer), 0);
        struct clientNode* node = addNode(client, numClients, buffer);
        node->threadHandle = CreateThread(NULL, 0, handleClient, (void *)node, 0 ,&threadID);
        if (node->threadHandle == NULL){
            fprintf(stderr, "Error while creating thread");
            exit(-1);
        }

        Sleep(1);
    }

    closesocket(socketFD);
    WSACleanup();
    return 0;
}