#include <stdio.h>
#include <winsock2.h>
#include <string>
using namespace std;

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)
int main(int arg, char* argv[])
{
    // Khoi tao thu vien
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Tao socket
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Khai bao dia chi server
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int val = stoi(argv[1]);
    addr.sin_port = htons(val);

    // Gan cau truc dia chi voi socket
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));

    // Chuyen sang trang thai cho ket noi
    listen(listener, 5);

    // Cho ket noi moi
    SOCKET client = accept(listener, NULL, NULL);

    // Nhan du lieu tu client
    char buf[256];
    int ret = recv(client, buf, sizeof(buf), 0);

    // Them ky tu ket thuc xau va in ra man hinh
    buf[ret] = 0;
    printf("Du lieu tu client:\n %s\n", buf);

    // Lien tuc doc du lieu tu ban phim va gui sang client
    while (1)
    {
        printf("Nhap xau ky tu: ");
        fgets(buf, sizeof(buf), stdin);

        if (strncmp(buf, "exit", 4) == 0)
            break;

        send(client, buf, strlen(buf), 0);
    }

    // Dong ket noi va giai phong thu vien
    closesocket(client);
    closesocket(listener);
    WSACleanup();
}
