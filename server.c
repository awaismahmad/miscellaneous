#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#pragma comment(lib, "Ws2_32.lib")

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 50000
#define SZ 512

int main()
{
	SOCKET serv_sock = INVALID_SOCKET;
	SOCKET cli_sock = INVALID_SOCKET;
	//
	struct sockaddr_in sin;
	struct hostent* hent;
	//
	WSADATA wsaData;
	//
	int res;
	char* ip;
	char buff[SZ];
	int opt_len = sizeof(BOOL);
	BOOL opt_val = FALSE;
	//
	ZeroMemory(&wsaData, sizeof(wsaData));
	res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != 0)
	{
		printf("Winsock initialisation failed: %d\n", res);
		return -1;
	}
	printf("Winsock initialised!\n");
	//
	//ZeroMemory(hent, sizeof(hent));
	hent = gethostbyname("localhost");
	if (hent == NULL)
	{
		printf("Could not find host: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	printf("Host obtained!\n");
	//
	ip = inet_ntoa(*((struct in_addr*)hent->h_addr_list[0]));
	printf("Host IP: %s\n", ip);
	//
	ZeroMemory(&sin, sizeof(sin));
	sin.sin_addr.S_un.S_addr = inet_addr(ip);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	//
	serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serv_sock == INVALID_SOCKET)
	{
		printf("Could not connect server socket: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	printf("Server socket connected!\n");
	//
	res = bind(serv_sock, (struct sockaddr*)&sin, sizeof(sin));
	if (res == SOCKET_ERROR)
	{
		printf("Could not bind server socket: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	printf("Server socket bound!\n");
	//
	opt_val = TRUE;
	res = setsockopt(serv_sock, SOL_SOCKET, SO_KEEPALIVE, (char*)&opt_val, opt_len);
	if (res != 0)
	{
		printf("Could not set socket options: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	printf("Socket options set!\n");
	//
	printf("Listening...\n");
	res = listen(serv_sock, 3);
	if (res != 0 || res == SOCKET_ERROR)
	{
		printf("Error occured while listening: %d\n", WSAGetLastError());
		WSACleanup();
		closesocket(serv_sock);
		return -1;
	}
	//
	cli_sock = accept(serv_sock, NULL, NULL);
	if (cli_sock == INVALID_SOCKET)
	{
		printf("Error accepting connection: %d\n", WSAGetLastError());
		WSACleanup();
		closesocket(serv_sock);
		return -1;
	}
	printf("Client socket accepted!\n");
	while (!_kbhit())
	{
		//
		ZeroMemory(buff, SZ);
		res = recv(cli_sock, buff, SZ - 1, 0);
		if (res <= 0)
		{
			printf("Could not receive client data: %d\n", WSAGetLastError());
			WSACleanup();
			closesocket(serv_sock);
			closesocket(cli_sock);
			return -1;
		}
		if (res == 0)
		{
			printf("Peer closed connection!\n");
			WSACleanup();
			closesocket(serv_sock);
			closesocket(cli_sock);
			return -1;
		}
		printf("Received: %d byte(s)\n", res);
		printf("%s\n", buff);
		//
		//strcpy(buff, "Server says hello!\n");
		res = send(cli_sock, buff, sizeof(buff), 0);
		if (res <= 0)
		{
			printf("Could not sent data to client: %d\n", WSAGetLastError());
			WSACleanup();
			closesocket(serv_sock);
			closesocket(cli_sock);
			return -1;
		}
		printf("Data sent to client. Byte(s) sent: %d\n", res);
	}
	//
	printf("A key was pressed. Exiting...\n");
	//
	res = shutdown(serv_sock, SD_BOTH);
	if (res != 0)
	{
		printf("Server socket shut-down failed: %d\n", WSAGetLastError());
		WSACleanup();
		closesocket(serv_sock);
		return -1;
	}
	printf("Server socket shut-down!\n");
	res = shutdown(cli_sock, SD_BOTH);
	if (res != 0)
	{
		printf("Client socket shut-down failed: %d\n", WSAGetLastError());
		WSACleanup();
		closesocket(serv_sock);
		return -1;
	}
	printf("Client socket shut-down\n");
	//
	res = closesocket(cli_sock);
	if (res != 0)
	{
		printf("Client socket did not close: %d\n", WSAGetLastError());
		WSACleanup();
		closesocket(serv_sock);
		return -1;
	}
	printf("Client socket closed!\n");
	res = closesocket(serv_sock);
	if (res != 0)
	{
		printf("Server socket did not close: %d\n", WSAGetLastError());
		WSACleanup();
		closesocket(serv_sock);
		return -1;
	}
	printf("Server socket closed!\n");
	//
	return 0;
}