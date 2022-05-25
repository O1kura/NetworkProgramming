// Lesson4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <WinSock2.h>
#include <iostream>
#include <string.h>

#pragma comment(lib,"ws2_32")
#pragma warning(disable:4996)

using namespace std; 
typedef struct {
    SOCKET client;
    char* id;
}user;
user clients[64];
int numClients = 0;
//Gui tin cho tat ca user tru nguoi su dung
void SendOtherClients(SOCKET client,char* msg) {
    for (int i = 0; i < numClients; i++) {
        if (client != clients[i].client)
            send(clients[i].client, msg, strlen(msg), 0);
    }
}
//Xoa nguoi dung
void RemoveClient(SOCKET client)
{
    // Tim vi tri cua client trong mang
    int i = 0;
    for (; i < numClients; i++)
        if (clients[i].client == client) break;
    // Xoa client khoi mang
    if (i < numClients - 1)
        clients[i] = clients[numClients - 1];
    numClients--;
}

//
bool checkClient(char* clientID) {
    for (int i = 0; i < numClients; i++) {
        if (strcmp(clientID, clients[i].id) == 0) {
            return i+1;
        }
    }
    return 0;
}

DWORD WINAPI ClientThread(LPVOID lpParam) {
    SOCKET client = *(SOCKET*)lpParam;
    int ret;
    char buf[256];
    char sbuf[256];
    char clientID[32];
    char cmd[32], id[32], tmp[32], mess[200];

    // Xu ly dang nhap
    while (1)
    {
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            return 0;

        buf[ret] = 0;
        printf("Du lieu nhan duoc: %s", buf);
        ret = sscanf(buf, "%s %s %s\n", cmd, clientID, tmp);
        if (ret != 2)
        {
            const char* msg = "[CONNECT] ERROR error_message\n";
            send(client, msg, strlen(msg), 0);
        }
        else
        {
            if (strcmp(cmd, "[CONNECT]") == 0 && !checkClient(clientID))
            {
                const char* msg = "[CONNECT] OK\n";
                send(client, msg, strlen(msg), 0);
                // Them vao mang
                clients[numClients].client = client;
                clients[numClients].id = clientID;
                numClients++;
                snprintf(sbuf, sizeof(sbuf), "[USER_CONNECT] %s\n", clientID);
                SendOtherClients(client, sbuf);
                break;
            }
            else
            {
                const char* msg = "[CONNECT] ERROR error_message\n";
                send(client, msg, strlen(msg), 0);
            }
        }
    }
    // Chuyen tiep tin nhan
    while (1)
    {
        ret = recv(client, buf, sizeof(buf), 0);
        bool flag= false;//Thuc hien command thanh cong hay khong
        if (ret <= 0)
        {
            RemoveClient(client);
            return 0;
        }
        buf[ret] = 0;
        printf("Du lieu nhan duoc tu %d: %s", client, buf);

        ret = sscanf(buf, "%s %s %s", cmd, id, mess);
        if (ret == 1) {
            //Dang xuat
            if (strcmp(cmd, "[DISCONNECT]") == 0) {
                snprintf(sbuf, sizeof(sbuf), "[USER_DISCONNECT] %s\n", clientID);
                SendOtherClients(client, sbuf);
                RemoveClient(client);
                return 0;
            }
            //Danh sach
            else if (strcmp(cmd, "[LIST]") == 0) {
                sbuf[0] = 0;
                strcat(sbuf, "[LIST] OK\n");
                for (int i = 0; i < numClients; i++) {
                    strcat(sbuf, clients[i].id);
                    strcat(sbuf, "\n");
                }
                send(client, sbuf, strlen(sbuf), 0);
                flag = true;
                
            }
      
        }
        else if (ret == 3) {
            //chuyen toan bo phan sau cua buf thanh tin nhan gui di
            strcpy(mess, &buf[strlen(cmd) + strlen(id) + 2]);
           
            //Xu ly viec in tin nhan
            
            if (strcmp(cmd,"[SEND]")==0) {
                flag = true;
                //Gui tin nhan den toan bo nguoi dung 
                if (strcmp(id,"ALL")==0) {
                    snprintf(sbuf, sizeof(sbuf), "[MESSAGE_ALL] %s: %s", clientID, mess);
                    SendOtherClients(client, sbuf);
                }
                //Gui tin den nguoi cu the
                else if(int j = checkClient(id)) {
                    snprintf(sbuf, sizeof(sbuf), "[MESSAGE] %s: %s", clientID, mess);
                    send(clients[j].client, sbuf, strlen(sbuf), 0);
                }
                else { 
                    const char* msg = "[SEND] ERROR error_message\n";
                    send(client, msg, strlen(msg), 0);
                }
            }
        }
        //Lenh sai cu phap
        if(!flag) {
            const char* msg = "[ERROR] error_message\n";
            send(client, msg, strlen(msg), 0);
        }

    }
    closesocket(client);
}

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listener, (SOCKADDR*)&addr, sizeof(addr));

    listen(listener, 5);

    while (1) {
        SOCKET client = accept(listener, NULL, NULL);

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
