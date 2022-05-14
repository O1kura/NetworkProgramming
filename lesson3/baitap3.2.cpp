// baitap3.2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <WinSock2.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>
using namespace std;
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

int main() {
	//Tao server ket noi
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(listener, (SOCKADDR*)&addr, sizeof(addr));

	listen(listener, 5);

	SOCKET client = accept(listener, NULL, NULL);

	char buf[256];
	int ret;

	//Doc file username password
	fstream inFile;
	inFile.open("C:\\test_server\\unpwd.txt",ios::in);
	char uname[21], passw[20];
	//Kiem tra uname voi password lien tuc

	while (1) {
		send(client, "nhap username va pass\n", 22, 0);
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			break;
		buf[ret] = 0;
		printf("Du lieu nhan duoc: %s\n", buf);
		char checkun[30],checkpw[30];
		int j;
		for (int i = 0; i < strlen(buf); i++) {
			if (buf[i] == ' ') {
				checkun[i] = 0;
				break;
			}
			checkun[i] = buf[i];
		}
		j = strlen(checkun)+1;
		for (int k = 0; k < strlen(buf) - j-1; k++) {
			checkpw[k] = buf[j + k];
		}
		checkpw[strlen(buf) - j-1] = 0;
		
		bool check = FALSE;

		while (!inFile.eof()) {
			inFile >> uname >> passw;
			if (strcmp(uname, checkun)==0) {
				printf("dung uname\n");
				if(strcmp(passw, checkpw)==0) {
					check = TRUE;
					printf("dung pw\n");
					inFile.close();
					break; 
				}
			}
		}
		if (inFile.is_open()) {
			inFile.clear();
			inFile.seekg(0, ios::beg);
		}
		if (check)
			break;
	
	}
	//Nhan du lieu lien tuc
	char command[50];
	char out[] = " > C:\\test_server\\out.txt";
	fstream cmdOutFile;
	cmdOutFile.open("C:\\test_server\\out.txt",ios::in);
	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			break;

		// Them ky tu ket thuc xau va in ra man hinh
		buf[ret-1] = 0;
		printf("Du lieu tu client: %s\n", buf);
		strcpy(command, buf);
		strcat(command, out);
		system(command);

		int i = 0;
		char ch;
		/*char buffer[1000];
		while (!cmdOutFile.eof())
		{
			cmdOutFile.get(ch);
			buffer[i] = ch;
			i++;
		}
		send(client, buffer, i, 0);*/
		while (!cmdOutFile.eof()) {
			cmdOutFile.getline(buf, sizeof(buf));
			strcat(buf, "\n");
			send(client, buf, strlen(buf), 0);
		}
	}
	cmdOutFile.close();
	//Dong ket noi va giai phong thu vien
	closesocket(client);
	closesocket(listener);
	WSACleanup();

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
