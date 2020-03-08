#include "Wlan.h"

#include <errno.h> /* EINPROGRESS, errno */
#include <sys/types.h> /* timeval */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>


int WlanInit()
{
	// WSADATA wsaData;	// Windows socket

	// // Initialize Winsock
	// if (WSAStartup(MAKEWORD(2,2), &wsaData) == SOCKET_ERROR) {
	// }

	// wlanInitialized = true;
	return 0;
}

int WlanTerm()
{
	// if (!wlanInitialized) return 0;
	// WSACleanup();
	// wlanInitialized = false;
	return 0;
}

int SetSockNoBlock(int s, u32 val)
{ 
	// unsigned long nonblocking = 1;
	// ioctlsocket(s, FIONBIO, &nonblocking);

    return 1;
}

int SocketConnect(Socket* socket, char* host, int port)
{
    printf("SocketConnect %s:%d \n", host, port);
    socket->sock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socket->sock == -1) {
        printf("Socket::connect(): failed to create socket \n");
        return -1;
    }
    fcntl(socket->sock, F_SETFL, O_NONBLOCK);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if(inet_pton(AF_INET, host, &addr.sin_addr) != 1) {
        printf("Socket::connect(): inet_pton failed \n");
        return -1;
    }

    const int res = ::connect(socket->sock, (struct sockaddr *)&addr, sizeof(addr));
    if(res == -1) {
        if(errno == EINPROGRESS) {
            printf("Socket::connect(): Connection in progress for fd: %d \n",socket->sock);

            /* Wait for connection to complete */
            fd_set sockets;
            FD_ZERO(&sockets);
            FD_SET(socket->sock, &sockets);

            /* You should probably do other work instead of busy waiting on this...
               or set a timeout or something */
            while(select(socket->sock + 1, nullptr, &sockets, nullptr, nullptr) <= 0) {}

        } else {
            printf("Socket::connect(): connection failed \n");
            return -1;
        }
    } 
	return 1;

	// socket1->sock = socket(AF_INET, SOCK_STREAM, 0);
	// if (socket1->sock < 0) ;//error("socket");

	// struct hostent *hp;
	// hp = gethostbyname(host);

	// socket1->addrTo.sin_family = AF_INET;
	// socket1->addrTo.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)hp->h_addr));
	// socket1->addrTo.sin_port = htons(port);

	// SetSockNoBlock(socket1->sock, 1);

	// connect(socket1->sock, (struct sockaddr *)&socket1->addrTo, sizeof socket1->addrTo);

	// return 1;
}

int SocketConnectUdp(Socket* socket, char* host, int port)
{	
	return SocketConnect(socket, host, port);

	// socket1->sock = socket(AF_INET, SOCK_DGRAM, 0);
	// if (socket1->sock < 0) ;//error("socket");

	// socket1->addrTo.sin_family = AF_INET;

	// struct hostent *hp;
	// hp = gethostbyname(host);

	// bcopy((char *)hp->h_addr, (char *)&socket1->addrTo.sin_addr, hp->h_length);

	// socket1->addrTo.sin_port = htons(port);

	// SetSockNoBlock(socket1->sock, 1);

	// return 1;
}

int SocketRecv(Socket* socket, char* buf, int size)
{
    /* Wait timeout milliseconds to receive data */
    fd_set sockets;
    FD_ZERO(&sockets);
    FD_SET(socket->sock, &sockets);

    int ret = select(socket->sock + 1, &sockets, nullptr, nullptr, nullptr); //todo set timeout
    if(ret == 0) {
        /* Timeout */
		return -1;
    } else if(ret < 0) {
    	printf("Socket::receive(): select failed \n");
		return -1;
    }

    ret = recv(socket->sock, buf, size, 0);
    if(ret < 0) {
        printf("Socket::receive(): recv failed \n");
        return -1;
    }

    return ret;


	// struct sockaddr_in addrFrom;
	// unsigned int len = sizeof (addrFrom); 

	// int count = recv(socket->sock, buf, size, 0);
	// return count;
}

int SocketSend(Socket* socket, char* buf, int size)
{
    const int ret = ::send(socket->sock, buf, size, 0);
    if(ret == -1) {
        printf("Socket::send(): send failed \n");
		SocketClose(socket);
        return -1;
    }

	return ret;

	// int n = send(socket->sock,buf,size,0);
	// return n;
}


int SocketSendUdp(Socket* socket, char* buf, int size)
{
	return SocketSend(socket, buf, size);

    // int total = 0;        // how many bytes we've sent
    // int bytesleft = size; // how many we have left to send
    // int n;

    // while(total < size) {
    //     n = sendto(socket->sock, buf+total, bytesleft, 0, (sockaddr*)&(socket->addrTo),sizeof(socket->addrTo));
    //     if (n == -1) { break; }
    //     total += n;
    //     bytesleft -= n;
    // }

	// return 1;
}

int SocketClose(Socket* socket)
{
	return ::close(socket->sock);
}