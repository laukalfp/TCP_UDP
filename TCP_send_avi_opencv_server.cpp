// server

#include <stdio.h>  
#include <winsock2.h>  
#pragma comment(lib,"ws2_32.lib")  
#include "opencv2/opencv.hpp"
using namespace cv;

typedef struct socket_info
{
	SOCKET serSocket;
	int numClient;
	sockaddr_in remoteAddr[10];
	int nAddrLen[10];

}Socket_Udp_Server_Info;
typedef struct socket_info_tcp
{
	SOCKET slisten;
	int numClient;
	SOCKET sClient[10]; // 记录客户端的套接字
	sockaddr_in remoteAddr[10];

}Socket_Tcp_Server_Info;
Socket_Udp_Server_Info * initUdpServer()
{
	int i = 0;
	Socket_Udp_Server_Info * mSocketInfo = (Socket_Udp_Server_Info *)malloc(sizeof(Socket_Udp_Server_Info));

	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	mSocketInfo->serSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mSocketInfo->serSocket == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(8888);
	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(mSocketInfo->serSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("bind error !");
		closesocket(mSocketInfo->serSocket);
		return 0;
	}
	else
	{
		printf("bind succeed  ! \n");
	}
	mSocketInfo->nAddrLen[0] = sizeof(sockaddr_in);

	return mSocketInfo;
}

// 结束时候调用
void unitUdpServer(Socket_Udp_Server_Info * mSocketInfo)
{
	closesocket(mSocketInfo->serSocket);

	WSACleanup(); // 结束ws2_32.lib
}
Socket_Tcp_Server_Info * initTcpServer()
{
	int i = 0;
	Socket_Tcp_Server_Info * mSocketInfo = (Socket_Tcp_Server_Info *)malloc(sizeof(Socket_Tcp_Server_Info));
	mSocketInfo->numClient = 1; // 只连接一个客户端
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	//创建套接字  
	mSocketInfo->slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocketInfo->slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	//绑定IP和端口  
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(mSocketInfo->slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error !");
	}

	//开始监听  
	if (listen(mSocketInfo->slisten, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}
	int nAddrlen = sizeof(mSocketInfo->remoteAddr[0]);
	for (i = 0; i < 1; i++) // 如果要接收很多客户端就要循环多次，这里只有一个客户端，如果多个客户端，要多线程处理
	{
		mSocketInfo->sClient[i] = accept(mSocketInfo->slisten, (SOCKADDR *)&mSocketInfo->remoteAddr[i], &nAddrlen);
		if (mSocketInfo->sClient[i] == INVALID_SOCKET)
		{
			printf("accept error !");
			continue;
		}
		else
		{
			printf("accept success  接收到一个连接 ! \n");
		}
	}

	return mSocketInfo;
}

// 结束时候调用
void unitTcpServer(Socket_Tcp_Server_Info * mSocketInfo)
{
	closesocket(mSocketInfo->slisten);
	for (int i = 0; i < mSocketInfo->numClient; i++)
	{
		closesocket(mSocketInfo->sClient[i]);
	}
	WSACleanup(); // 结束ws2_32.lib
}

int main()
{

	Socket_Udp_Server_Info * mSocketUdpServerInfo = initUdpServer();
	Socket_Tcp_Server_Info * mSocketTcpServerInfo = initTcpServer();

	int info[3];
	int flag = 0;
	int ret = 0;

	recv(mSocketTcpServerInfo->sClient[0], (char *)info, sizeof(int)* 3, 0);
	int width = info[0];
	int height = info[1];
	int channel = info[2];
	int bufferSize = width * height * channel;
	flag = 1;
	ret = send(mSocketTcpServerInfo->sClient[0], (char *)(&flag), sizeof(int), 0);
	if (ret == sizeof(int))
	{
		printf("flag re succeed \n");
	}
	
	Mat reviMat = Mat(height, width, CV_8UC3);

	int m = 0;
	double t = (double)cvGetTickCount();
	while (1)
	{
		char * revBuffer = (char *)malloc(sizeof(char)* bufferSize);
		ret = 0;
		//ret = recvfrom(mSocketUdpServerInfo->serSocket, revBuffer, bufferSize, 0, (sockaddr *)&mSocketUdpServerInfo->remoteAddr[0], &mSocketUdpServerInfo->nAddrLen[0]);
		ret = recv(mSocketTcpServerInfo->sClient[0], revBuffer, bufferSize, 0);  //当client传送完了之后它也会一直在等，因为它不知道是否发送完。用后面的break；结束，要预先知道帧数。
	//	printf("server ret = %d\n", ret);
		
		if (ret == bufferSize)
		{
			memcpy(reviMat.data, revBuffer, bufferSize);
			imshow("revice", reviMat);
			waitKey(10);
			printf("%d\n", m);
			m++;
		}
		if (m==38)
		{
			break;
		}
		
	}

	t = ((double)cvGetTickCount() - t) / (cvGetTickFrequency() * 1000);
	printf("时间：%d s", t);
	getchar();
	return 0;
}