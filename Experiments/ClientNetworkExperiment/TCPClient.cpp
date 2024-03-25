#define WIN32_LEAN_AND_MEAN
#define NOMINMAX  // To allow std::max() to work
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <numeric>

int simpleTCPClient() {
    WSADATA wsaData;
    int wsaErr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaErr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaErr != 0) {
        std::cout << "WSAStartup failed!" << std::endl;
        return 1;
    }
    std::cout << "WSAStartup successful!" << std::endl;
    std::cout << "Status: " << wsaData.szSystemStatus << std::endl;

    // Create socket for TCP
    SOCKET clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Failed to create socket. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();   // Free the DLL up
        return 1;
    }
    std::cout << "Socket creation successful!" << std::endl;

    // Connect client
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    InetPton(AF_INET, _T("127.0.0.1"), &clientService.sin_addr.s_addr);
    int port = 55555;
    clientService.sin_port = htons(port);
    // The binding of the port and IP to the socket happens automatically in
    // the connect function, so that step is "skipped" for client code.
    if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        std::cout << "Client failed to connect." << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "Client connected successfully!" << std::endl;
    std::cout << "Client can now start sending and receiving data!" << std::endl;

    char sendToServerBuffer[200];
    std::cout << "Please enter the message you want to send to the server, then hit enter:" << std::endl;
    std::cin.getline(sendToServerBuffer, 200);
    int bytesSent = send(clientSocket, sendToServerBuffer, 200, 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cout << "Client failed to send! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "Client sent " << bytesSent << " bytes to the server." << std::endl;

    char receiveFromServerBuffer[200];
    int bytesReceived = recv(clientSocket, receiveFromServerBuffer, 200, 0);
    if (bytesReceived < 0) {
        std::cout << "Error receiving from client! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "Received \"" << receiveFromServerBuffer << "\" from the server." << std::endl;

    WSACleanup();
    return 0;
}


SOCKET setupTCPClient(int portNumber, PCWSTR serverIPAddress) {
    WSADATA wsaData;
    int wsaErr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaErr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaErr != 0) {
        std::cout << "WSAStartup failed!" << std::endl;
        return INVALID_SOCKET;
    }
    std::cout << "WSAStartup successful!" << std::endl;
    std::cout << "Status: " << wsaData.szSystemStatus << std::endl;

    SOCKET clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Failed to create socket. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();   // Free the DLL up
        return INVALID_SOCKET;
    }
    std::cout << "Socket creation successful!" << std::endl;

    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    InetPton(AF_INET, serverIPAddress, &clientService.sin_addr.s_addr);
    clientService.sin_port = htons(portNumber);
    if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        std::cout << "Client failed to connect." << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }

    return clientSocket;
}


int simpleTCPChatClient() {
    SOCKET clientSocket;
    if ((clientSocket = setupTCPClient(55555, L"127.0.0.1")) == INVALID_SOCKET) {
        std::cout << "Failed to setup TCP client socket!" << std::endl;
        return 1;
    }

    const int bufferSize = 500;
    char sendToServerBuffer[bufferSize] = "";
    char receiveFromServerBuffer[bufferSize] = "";
    int bytesSent;
    int bytesReceived;

    while (true) {
        std::cout << "Please type the message you wish to send to the server: ";
        std::cin.getline(sendToServerBuffer, bufferSize);
        if (std::cin.fail()) {
            std::cin.clear();  // Clear the fail bit
            // Discard characters until a newline is encountered
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        bytesSent = send(clientSocket, sendToServerBuffer, bufferSize, 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cout << "Client failed to send! Error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 1;
        }

        bytesReceived = recv(clientSocket, receiveFromServerBuffer, bufferSize, 0);
        if (bytesReceived < 0) {
            std::cout << "Error receiving from client! Error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 1;
        }
        std::cout << "Server: \"" << receiveFromServerBuffer << "\"" << std::endl;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    std::cout << "The amount of arguments is " << argc << std::endl;
    std::cout << "The argument received is " << *(argv[1]) << std::endl;

    switch (*(argv[1])) {
    case '1':
        simpleTCPChatClient();
        break;
    }
    return 0;
}