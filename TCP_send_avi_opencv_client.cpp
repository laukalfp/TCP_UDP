
// client

#include <WINSOCK2.H>  
#include <STDIO.H>  
#include "opencv2/opencv.hpp"
#pragma  comment(lib,"ws2_32.lib")  

using namespace cv;
typedef struct socket_info
{
	SOCKET sclient;
	sockaddr_in sin;
	int len;

}Socket_Udp_Client_Info;
typedef struct socket_info_tcp
{
	SOCKET sclient;

}Socket_Tcp_Client_Info;

Socket_Udp_Client_Info * initUdpClient()
{
	Socket_Udp_Client_Info * mSocketInfo = (Socket_Udp_Client_Info *)malloc(sizeof(Socket_Udp_Client_Info));

	WORD socketVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		return 0;
	}
	mSocketInfo->sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	mSocketInfo->sin.sin_family = AF_INET;
	mSocketInfo->sin.sin_port = htons(8888);
	mSocketInfo->sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	mSocketInfo->len = sizeof(mSocketInfo->sin);

	return mSocketInfo;
}
void unitUdpClient(Socket_Udp_Client_Info * mSocketInfo)
{
	closesocket(mSocketInfo->sclient);
	WSACleanup();
}
Socket_Tcp_Client_Info * initTcpClient()
{
	Socket_Tcp_Client_Info * mSocketInfo = (Socket_Tcp_Client_Info *)malloc(sizeof(Socket_Tcp_Client_Info));

	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}

	mSocketInfo->sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocketInfo->sclient == INVALID_SOCKET)
	{
		printf("invalid socket !");
		return 0;
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(8888);
	serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//serAddr.sin_addr.S_un.S_addr = inet_addr("192.168.0.211");
	if (connect(mSocketInfo->sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("connect error !");
		closesocket(mSocketInfo->sclient);
		return 0;
	}
	return mSocketInfo;
}
void unitTcpClient(Socket_Tcp_Client_Info * mSocketInfo)
{
	closesocket(mSocketInfo->sclient);
	WSACleanup();
}

int main()
{

	Socket_Udp_Client_Info * mSocketUdpClientInfo = initUdpClient();
	Socket_Tcp_Client_Info * mSocketTcpClientInfo = initTcpClient();

	//cv::VideoCapture capture(0); // 打开摄像头
	cv::VideoCapture capture("G:/test.avi"); // 打开摄像头
	cv::Mat imageTemp;
	capture >> imageTemp;
	int info[3];
	int ret = 0;
	int flag = 0;
	info[0] = imageTemp.cols;
	info[1] = imageTemp.rows;
	info[2] = imageTemp.channels();
	// 先把宽高和通道数传过去
	ret = send(mSocketTcpClientInfo->sclient, (char *)info, sizeof(int)* 3, 0);
	if (ret == sizeof(int)* 3)
	{
		printf("client send width height channel succeed \n");
	}
	recv(mSocketTcpClientInfo->sclient, (char *)(&flag), sizeof(int), 0);
	if (flag == 1)
	{
		printf("client dui fang succeed recevied \n");
	}
	printf("Image size: [%d %d]\n", imageTemp.cols, imageTemp.rows);
	int m = 0;
	double t = (double)cvGetTickCount();
	while (1)
	{
		capture >> imageTemp;
		if (imageTemp.empty())
		{
			break;
		}
		// 发送的包的大小不能超过1500个字节
		//ret = sendto(mSocketUdpClientInfo->sclient, (char *)(imageTemp.data), imageTemp.cols, 0, (sockaddr *)&mSocketUdpClientInfo->sin, mSocketUdpClientInfo->len);
		ret = send(mSocketTcpClientInfo->sclient, (char *)(imageTemp.data), imageTemp.cols * imageTemp.rows * imageTemp.channels(), 0);
		//printf("Client ret = %d \n", ret);
		imshow("send", imageTemp);
		cv::waitKey(10);
		printf("%d\n", m);
		m++;
	}
	t = ((double)cvGetTickCount() - t) / (cvGetTickFrequency() * 1000);
	printf("时间：%d s", t);
	getchar();
	return 0;
}