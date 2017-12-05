#include <stdio.h>
#include <Winsock2.h>

#pragma comment( lib, "ws2_32.lib" ) 

void main()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return;
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		return;
	}
	SOCKET sockSrv = socket(AF_INET, SOCK_DGRAM, 0);

	int len = sizeof(SOCKADDR);

	SOCKADDR_IN from;
	SOCKADDR_IN local;
	local.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	local.sin_family = AF_INET;
	local.sin_port = htons(27015);

	int a = bind(sockSrv, (SOCKADDR*)&local, len);



	while (1)
	{
		char recvBuf[50];
		recvfrom(sockSrv, recvBuf, 50, 0, (SOCKADDR*)&from, &len);//from收到客户端的IP信息
		printf("%s\n", recvBuf);
		printf("%s\n", inet_ntoa(local.sin_addr));
		char sendBuf[50];
		sprintf(sendBuf, "Welcome %s to here!", inet_ntoa(from.sin_addr));
		sendto(sockSrv, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR*)&from, len);

		Sleep(2000);
	}
	closesocket(sockSrv);
	WSACleanup();
}