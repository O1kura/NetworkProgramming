#include <stdio.h>
#include <winsock2.h>

#include <ws2tcpip.h>
#include <string>
using namespace std;

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)


char* replaceNull(char* array) {
    /*for (int i = 0; i < sizeof(array); i++)
    {
        if (array[i] == NULL) array[i] = ' ';

    }*/

    int cx = 0;
    char buffer[100];
    char* token = strtok(array, " ");

    while (token != NULL)
    {
        DWORD lpBytesPerSector = 0;
        DWORD lpSectorsPerCluster = 0;
        DWORD totalNumberOfClusters = 0;
        DWORD amountOfKBytesOfComputer = 0;
        GetDiskFreeSpaceA(token, &lpSectorsPerCluster, &lpBytesPerSector, NULL, &totalNumberOfClusters);
        amountOfKBytesOfComputer = lpBytesPerSector * lpSectorsPerCluster * (totalNumberOfClusters / 1024 / 1024);
        cx = snprintf(buffer, sizeof(buffer), " %s %lu MB\n", token, amountOfKBytesOfComputer);
        token = strtok(NULL, " ");
    }
    return buffer;
}


int main(int argc, char* argv[])
{
    // Lấy dữ liệu cần gửi
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD t = MAX_COMPUTERNAME_LENGTH + 1;
    if (!GetComputerNameA(computerName, &t))
        printf("Fail to get PC name");

    char lpBuffer[50];
    GetLogicalDriveStringsA(sizeof(lpBuffer), lpBuffer);
    char* diskInfo = replaceNull(lpBuffer); 
  
    char buff[256];
    snprintf(buff, sizeof(buff),
        "- Name of Computer: %s\n"
        "- List of disk:\n%s"
        , computerName, diskInfo);
 

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

    // Gui du lieu den server

    printf("Gui du lieu");
    send(client, buff, strlen(buff), 0);

    // Nhan du lieu tu server va in ra man hinh
    char buf[256];
    while (1)
    {
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;

        // Them ky tu ket thuc xau va in ra man hinh
        if (ret < sizeof(buf))
        {
            buf[ret] = 0;
            printf("Du lieu tu server:\n %s\n", buf);
        }
    }

    closesocket(client);
    WSACleanup();
}