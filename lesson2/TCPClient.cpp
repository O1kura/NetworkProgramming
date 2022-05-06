#include<stdio.h>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include <string> 
using namespace std;
#pragma comment(lib,"ws2_32")
#pragma warning(disable:4996)

int main(int argc, char* argv[]) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    addrinfo* info;
    int ret = getaddrinfo(argv[1], "http", NULL, &info);
    if (ret != 0) {
        printf("Phan giai ten mien that bai.");
    }

    addrinfo* temp = info->ai_next;
    while (temp->ai_next != NULL) {
        if (temp->ai_family == 2)
            break;
        temp = temp->ai_next;
    }

    int portNumber = stoi(argv[2]);
    SOCKADDR_IN addr;
    memcpy(&addr, temp->ai_addr, temp->ai_addrlen);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(portNumber);

    system("pause");

    SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    ret = connect(client, (SOCKADDR*)&addr, sizeof(addr));
    if (ret == SOCKET_ERROR)
    {
        ret = WSAGetLastError();
        printf("Ket noi khong thanh cong - %d\n", ret);
        return 1;
    }


    char buf[256];
    // Nhan du lieu tu server
    int ret2 = recv(client, buf, sizeof(buf), 0);
    buf[ret2] = 0;
    printf("Du lieu tu server: %s\n", buf);
    // Lien tuc doc du lieu tu ban phim va gui sang client
    while (1)
    {
        printf("Nhap xau ky tu: ");
        fgets(buf, sizeof(buf), stdin);

        if (strncmp(buf, "exit", 4) == 0)
            break;

        send(client, buf, strlen(buf), 0);
    }

    closesocket(client);
    WSACleanup();
}

