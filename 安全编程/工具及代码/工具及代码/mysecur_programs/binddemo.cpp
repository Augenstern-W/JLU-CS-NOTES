/*
  This application demonstrates a generic UDP-based server.
  It listens on port 8391. If you have something running there,
  change the port number and remember to change the client too.
*/
#include <iostream>
#include <winsock2.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char *argv[])
{
	WSADATA Ws;
    SOCKET sock,jacksock;
    sockaddr_in sin,local,from;
    int len, packets;
    bool hijack=false;
    bool nohijack=false;
	char buf[512];
    len=sizeof(sockaddr);
	ZeroMemory(buf, sizeof(buf));
	//Init Windows Socket
    if ( WSAStartup(MAKEWORD(2,2), &Ws) != 0 )
    {
        cout<<"Init Windows Socket Failed::"<<GetLastError()<<endl;
        return -1;
    }
	if(argc<2||argc>3)
    {
        printf("Usage is %s [addres to bind]\n",argv[0]);
        printf("Options are:\n\t-nohijack\n");
        return -1;
    }
	if(argc==3)
    {
        //check to see whether hijacking mode or 
        //no-hijack mode is enable.
        if(strcmp("-hijack",argv[2])==0) hijack=true;
        else if(strcmp("-nohijack",argv[2])==0) nohijack=true;
        else
        {
            printf("unrecognized argument %s\n", argv[2]);
            return -1;
        }
    }

    sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock==INVALID_SOCKET)
    {
        printf("Cannot create socket –err=%d\n",
             GetLastError());
        return -1;
    }
	
	jacksock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(jacksock==INVALID_SOCKET)
    {
        printf("Cannot create socket –err=%d\n",
             GetLastError());
        return -1;
    }
	ZeroMemory(&sin, len);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8391);            
	sin.sin_addr.S_un.S_addr = inet_addr(argv[1]);
	
	ZeroMemory(&local, len);
	local.sin_family = AF_INET;
	local.sin_port = htons(8391);            
	local.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");  	
	

    if(hijack) 
	{
        BOOL val=TRUE;

        if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(val))==SOCKET_ERROR)
		{
            printf("Cannot set SO_REUSEADDR – err=%d\n", GetLastError());
            closesocket(sock);
            return -1;
        }
		else
		{
            printf("SO_REUSEADDR enabled%d\n",SO_REUSEADDR);
			if(bind(sock,(sockaddr *)&sin, len)==SOCKET_ERROR)
			{
				printf("Curses! Our evil warez are foiled!\n");
				printf("Cannot bind socket-err=%d\n", GetLastError());
				closesocket(sock);
				return -1;
			}
			else
			{
				printf("Socket bound to %s\n",argv[1]);
			}
		}
    }
	else if(nohijack)
	{
        BOOL val=TRUE;
        if(setsockopt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char *)&val, 
			sizeof(val))==SOCKET_ERROR)
		{
            printf("Cannot set SO_EXCLUSIVEADDRUSE – err=%d\n", GetLastError());
            closesocket(sock);
            return -1;
        }
		else
		{
            printf("SO_EXCLUSIVEADDRUSE enabled%d\n",SO_EXCLUSIVEADDRUSE);
            printf("No hijackers allowed!\n");
			if(bind(sock,(sockaddr *)&sin, sizeof(sockaddr))==SOCKET_ERROR)
			{
				printf("Curses! Our evil warez are foiled!\n");
				printf("Cannot bind socket-err=%d\n", GetLastError());
				closesocket(sock);
				return -1;
			}
			else
			{
				printf("Socket bound to %s\n",argv[1]);
			}
        }
    }
	else
	{
			if(bind(sock,(sockaddr *)&sin, sizeof(sockaddr))==SOCKET_ERROR)
			{
				printf("Curses! Our evil warez are foiled!\n");
				printf("Cannot bind socket-err=%d\n", GetLastError());
				closesocket(sock);
				return -1;
			}
			else
			printf("Socket bound to %s\n",argv[1]);		
	}

	for(packets=0; packets<10; packets++)
    {
		int fromlen=sizeof(sockaddr);
		ZeroMemory(buf, sizeof(buf));
        if(recvfrom(sock, buf, 512, 0, (sockaddr *)&from, &fromlen)>0)
        {
            printf("Message from %s at port %d:\n%s\n", 
				inet_ntoa(from.sin_addr),ntohs(from.sin_port),buf);

			if(hijack)
			{
				ZeroMemory(buf, sizeof(buf));
				strcpy(buf, "you are hacked!");
				if(sendto(jacksock, buf, strlen(buf)+1, 0,
					(sockaddr *)&local, sizeof(sockaddr))<0)
					printf("Cannot send message to localhost %s:%d\n",
						inet_ntoa(local.sin_addr),GetLastError()); 
			}
		}
		else 
		{
			printf("Ghastly error %d\n", GetLastError());
			break; 
		}
	}
	return 0;	
}


