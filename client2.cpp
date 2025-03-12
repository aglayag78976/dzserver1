#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <ws2tcpip.h>
#include <string>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int main() {
    setlocale(0, "Ukrainian");
    system("title CLIENT SIDE");
    cout << "Клієнт запущено!\n";

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "Помилка WSAStartup: " << iResult << "\n";
        return 1;
    }

    addrinfo hints{}, * result = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    const char* ip = "localhost";
    iResult = getaddrinfo(ip, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        cout << "Помилка getaddrinfo: " << iResult << "\n";
        WSACleanup();
        return 2;
    }

    SOCKET ConnectSocket = INVALID_SOCKET;
    for (addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) continue;

        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Неможливо підключитися до сервера!\n";
        WSACleanup();
        return 3;
    }

    char recvBuffer[DEFAULT_BUFLEN];
    string userInput;

    do {
        cout << "Введіть число (або 'exit' для виходу): ";
        cin >> userInput;

        if (userInput == "exit") break;

        send(ConnectSocket, userInput.c_str(), (int)userInput.length(), 0);

        iResult = recv(ConnectSocket, recvBuffer, DEFAULT_BUFLEN, 0);
        if (iResult > 0) {
            recvBuffer[iResult] = '\0';
            cout << "Сервер повернув: " << recvBuffer << "\n";
        }
    } while (true);

    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}
