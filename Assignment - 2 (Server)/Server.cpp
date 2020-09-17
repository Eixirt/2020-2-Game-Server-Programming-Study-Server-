#pragma comment(lib, "Ws2_32.lib")
#define MAX_BUFFER 1024
#define SERVER_PORT 3500

#include <iostream>
#include <WS2tcpip.h>

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
        return 1;

    SOCKET listenSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
    SOCKADDR_IN serverAddr;
    memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    ::bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(listenSocket, 5);

    while (true) {
        SOCKADDR_IN client_addr;
        int addr_size = sizeof(client_addr);
        SOCKET client_socket = accept(listenSocket, (sockaddr*)&client_addr, &addr_size);
        while (true) {
            char messageBuffer[MAX_BUFFER + 1];
            int receiveBytes = recv(client_socket, messageBuffer, MAX_BUFFER, 0);
            if (receiveBytes > 0) {
                messageBuffer[receiveBytes] = 0;
                std::cout << "TRACE - Receive message : " << messageBuffer << "(" << receiveBytes << " bytes)" << std::endl;
            }
            else break;

            int sendBytes = send(client_socket, messageBuffer, receiveBytes, 0);
            if (sendBytes > 0) std::cout << "TRACE _ Send message : " << messageBuffer << "( " << sendBytes << "bytes)" << std::endl;
        }
        closesocket(client_socket);
    }
    closesocket(listenSocket);
    WSACleanup();
}

