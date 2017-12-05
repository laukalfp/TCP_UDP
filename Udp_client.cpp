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


	for (int index = 0;; index++)
	{
		SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);

		int len = sizeof(SOCKADDR);

		SOCKADDR_IN local;
		local.sin_addr.S_un.S_addr = inet_addr("192.168.200.35");
		local.sin_family = AF_INET;
		local.sin_port = htons(27015);

		char sendBuf[30];
		sprintf(sendBuf, "%3d,", index);
		strcat(sendBuf, "server node of: yaopeng");
		sendto(sockClient, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR*)&local, len);

		char recvBuf[50];
		recvfrom(sockClient, recvBuf, 50, 0, (SOCKADDR*)&local, &len);
		printf("my reply is : %s\n", recvBuf);
		printf("%s\n", inet_ntoa(local.sin_addr));

		closesocket(sockClient);
		Sleep(2000);
		WSACleanup();
	}
}