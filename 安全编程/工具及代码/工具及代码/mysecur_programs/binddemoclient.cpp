//#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
 
#pragma comment(lib, "ws2_32.lib") 
 
int main(int argc, char* argv[])
{
	WORD socketVersion = MAKEWORD(2,2);
	WSADATA wsaData; 
	if(WSAStartup(socketVersion, &wsaData) != 0)
	{
		return 0;
	}
	SOCKET sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8391);
	sin.sin_addr.S_un.S_addr = inet_addr(argv[1]);
	int len = sizeof(sin);
	
	char sendData[] = "I'm Client!";
	sendto(sclient, sendData, strlen(sendData), 0, (sockaddr *)&sin, len);

	closesocket(sclient);
	WSACleanup();
	return 0;
}
