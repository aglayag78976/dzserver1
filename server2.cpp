#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <ws2tcpip.h>
#include <cstdlib>  // для atoi() і itoa()
using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int main() {
    setlocale(0, "");
    system("title SERVER SIDE");
    cout << "Сервер запущено!\n";

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "Помилка WSAStartup: " << iResult << "\n";
        return 1;
    }

    addrinfo hints{}, * result = NULL;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        cout << "Помилка getaddrinfo: " << iResult << "\n";
        WSACleanup();
        return 2;
    }

    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Помилка socket: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        WSACleanup();
        return 3;
    }

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);
    if (iResult == SOCKET_ERROR) {
        cout << "Помилка bind: " << WSAGetLastError() << "\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 4;
    }

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        cout << "Помилка listen: " << WSAGetLastError() << "\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 5;
    }

    cout << "Очікування підключення клієнта...\n";

    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        cout << "Помилка accept: " << WSAGetLastError() << "\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 6;
    }

    closesocket(ListenSocket);

    char recvBuffer[DEFAULT_BUFLEN];
    int recvResult;

    do {
        recvResult = recv(ClientSocket, recvBuffer, DEFAULT_BUFLEN, 0);
        if (recvResult > 0) {
            recvBuffer[recvResult] = '\0';
            cout << "Отримано число від клієнта: " << recvBuffer << "\n";

            int receivedNumber = atoi(recvBuffer);
            receivedNumber++;

            char sendBuffer[DEFAULT_BUFLEN];
            _itoa_s(receivedNumber, sendBuffer, 10);

            send(ClientSocket, sendBuffer, strlen(sendBuffer), 0);
            cout << "Відправлено число клієнту: " << sendBuffer << "\n";
        }
    } while (recvResult > 0);

    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}

