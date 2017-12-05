//#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include <opencv2/opencv.hpp>
#pragma comment(lib, "ws2_32.lib") 
#include <iostream>
using namespace cv;
using namespace std;
int main(int argc, char* argv[])
{
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	SOCKET serSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serSocket == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(8888);
	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(serSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("bind error !");
		closesocket(serSocket);
		return 0;
	}

	sockaddr_in remoteAddr;
	int nAddrLen = sizeof(remoteAddr);

	printf("等待连接...");
	int info[3];
	int flag = 0;
	int ret = 0;
	ret = recvfrom(serSocket, (char*)info, sizeof(int)* 3, 0, (sockaddr*)&remoteAddr, &nAddrLen);
	printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
	int Width = info[0];
	int Height = info[1];
	int channel = info[2];
	int buffSize = Width*Height*channel;
	char*recvBuffer = (char*)malloc(sizeof(char)*buffSize);
	Mat reviMat = Mat(Height, Width, CV_8UC3);
	int m = 0;
	double t = (double)cvGetTickCount();
	while (1)
	{
		ret = recvfrom(serSocket, recvBuffer, buffSize, flag, (sockaddr*)&(remoteAddr), &nAddrLen);
		   printf("Server ret:=%d\n", ret);
	    	if (ret == buffSize){
			memcpy(reviMat.data, recvBuffer, buffSize);
			imshow("Recive", reviMat);
			cout << m << endl;
			m++;
			waitKey(10);
		}
		/*if (m==500)
		{
			break;
		}*/
	}
	
	t = ((double)cvGetTickCount() - t) / (cvGetTickFrequency() * 1000);
	printf("时间：%d s", t);
	//char * sendData = "一个来自服务端的UDP数据包\n";
	//sendto(serSocket, sendData, strlen(sendData), 0, (sockaddr *)&remoteAddr, nAddrLen);
	//waitKey(0);
	/*while (true)
	{
	char recvData[255];
	int ret = recvfrom(serSocket, recvData, 255, 0, (sockaddr *)&remoteAddr, &nAddrLen);
	if (ret > 0)
	{
	recvData[ret] = 0x00;
	printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
	printf(recvData);
	}

	char * sendData = "一个来自服务端的UDP数据包\n";
	sendto(serSocket, sendData, strlen(sendData), 0, (sockaddr *)&remoteAddr, nAddrLen);

	}*/
	closesocket(serSocket);
	WSACleanup();
	getchar();
	return 0;
}