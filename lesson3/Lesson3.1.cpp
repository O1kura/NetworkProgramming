// Lesson3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32")

SOCKET clients[64];
int numClients;
DWORD WINAPI ClientThread(LPVOID lpParam)
{
    SOCKET client = *(SOCKET*)lpParam;
    int ret;
    char buf[256];
    char buf2[10] = "Enter ID\n";
    char id[40];
    snprintf(id,sizeof(id),"client_id:%d\n",client);
    printf(id);
    while (1) {
        send(client, buf2, strlen(buf2), 0);
        ret = recv(client, buf, sizeof(buf), 0);
        buf[ret] = 0;
        if (strcmp(buf, id) == 0) {
            send(client, "Da ket noi\n", 11, 0);
            break;
        }
    }
    
    // Nhan du lieu tu client va in ra man hinh
    while (1)
    {
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        buf[ret] = 0;
        printf("Du lieu nhan duoc tu %d: %s\n", client, buf);
        char cha[256];
        snprintf(cha, sizeof(cha), "%d: %s", client, buf);
      
        for (int i = 0; i < numClients; i++) {
            
            if (clients[i] != client)
                send(clients[i], cha, strlen(cha), 0);

        }

    }
    closesocket(client);
}
int main()
{
    // Khoi tao thu vien
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    // Tao socket
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Khai bao dia chi server
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);
    // Gan cau truc dia chi voi socket
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    // Chuyen sang trang thai cho ket noi
    listen(listener, 5);
    while (1)
    {
        SOCKET client = accept(listener, NULL, NULL);

        clients[numClients] = client;
        numClients++;

        printf("Client moi ket noi: %d\n", client);
        CreateThread(0, 0, ClientThread, &client, 0, 0);
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
