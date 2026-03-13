#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std:: cout << "bind failed\n";
        return 1;
    }
    if (listen(server, 5) == SOCKET_ERROR) {
        std::cout << "listen failed\n";
        return 1;
    }

    std::cout << "server listening on port 8080\n";

    while (true) {
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);
        SOCKET client = accept(server, (sockaddr*)&clientAddr, &clientSize);
        if (client == INVALID_SOCKET) {
            std::cout << "accept failed: " << WSAGetLastError() << "\n";
            continue;
        }

        char buffer[4096];
        int bytesReceived = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "recv failed or client disconnected\n";
            closesocket(client);
            continue;
        }
        buffer[bytesReceived] = '\0';
        std::cout << buffer << std::endl;

        const char* response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<h1>Hello from my cpp server</h1>";

        send(client, response, strlen(response), 0);
        closesocket(client);
    }

    closesocket(server);
    WSACleanup();
}