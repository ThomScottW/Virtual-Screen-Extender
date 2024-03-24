#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <tchar.h>
#include <iostream>

int main(int argc, char* argv[]) {
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
    if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        std::cout << "Client failed to connect." << std::endl;
        WSACleanup();
        return 1;
    }
    std::cout << "Client connected successfully!" << std::endl;
    std::cout << "Client can now start sending and receiving data!" << std::endl;

    WSACleanup();
	return 0;
}