#include <stdio.h>
#include <winsock2.h>
#include <string>
#include<iostream>
#include <fstream>
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

    // Doc file chao
    fstream infile;
    
    char ch[100];
    infile.open(argv[2], ios::in);
    if (infile.is_open()) {
        infile >> ch;
        send(client, ch, strlen(ch), 0);
        infile.close();
        
    }
    else {
        char msg[] = "Error";
        send(client, msg, strlen(msg), 0);
    }
  
    // Nhan du lieu tu client
    char buf[256];
    fstream outfile;
    outfile.open(argv[3], ios::app);
    if (outfile.is_open()) {
        while (1) {
            int ret = recv(client, buf, sizeof(buf), 0);
            if (ret <= 0)
                break;

            // Them ky tu ket thuc xau va in ra man hinh
            if (ret < sizeof(buf))
            {
                buf[ret] = 0;
                printf("Du lieu tu cilent: %s", buf);
                outfile << buf;

            }
        }

    }
    outfile.close();
    // Dong ket noi va giai phong thu vien
    closesocket(client);
    closesocket(listener);
    WSACleanup();
}