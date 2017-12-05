//#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") 
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
int main(int argc, char* argv[])
{
	WORD socketVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		return 0;
	}
	SOCKET sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int len = sizeof(sin);

	/*char * sendData = "来自客户端的数据包.\n";
	sendto(sclient, sendData, strlen(sendData), 0, (sockaddr *)&sin, len);*/
//	Mat image = imread("face.png", 1);

	//cv::VideoCapture capture("G:/视频/straight_H264.avi"); // 打开摄像头
	cv::VideoCapture capture("G:/VideoTest8.avi"); // 打开摄像头
	cv::Mat image;
	capture >> image;

	int info[3];
	info[0] = image.cols;
	info[1] = image.rows;
	info[2] = image.channels();
	int ret = 0;
	ret = sendto(sclient, (char*)info, sizeof(int)* 3, 0, (sockaddr*)&sin, len);
	if (ret == sizeof(int)* 3)
	{
		printf("client send width height channnel succeed \n");
	}
	int m = 0;
	double t = (double)cvGetTickCount();
	while (1)
	{
		capture >> image;
		if (image.empty())
		{
			break;
		}
		ret = sendto(sclient, (char*)image.data, image.cols*image.rows*image.channels(), 0, (sockaddr*)&sin, len);
		//printf("Client ret:=%d\n", ret);
		imshow("send", image);
		cout << m << endl;
		m++;
		waitKey(10);
	}

	t = ((double)cvGetTickCount() - t) / (cvGetTickFrequency() * 1000);
	printf("时间：%d s", t);
	/*char recvData[255];
	ret = recvfrom(sclient, recvData, 255, 0, (sockaddr *)&sin, &len);
	if (ret > 0)
	{
		recvData[ret] = 0x00;
		printf(recvData);
	}
	waitKey(0);*/
	closesocket(sclient);
	WSACleanup();
	getchar();
	return 0;
}