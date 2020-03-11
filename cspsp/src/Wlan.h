#ifndef _WLAN_H_
#define _WLAN_H_

#include "JGE.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <vector>

#define PROXY_IP "frankinfotech.it"
#define PROXY_PORT 2900

struct ConnectionConfig {
	char name[64];
	int index;
};

class Socket
{
public:
	int sock;
};


int WlanInit();
int WlanTerm();
int SocketConnect(Socket* socket, char* host, int port);
int SocketConnectUdp(Socket* socket, char* host, int port);
int SocketRecv(Socket* socket, char* buf, int size);
int SocketSend(Socket* socket, char* buf, int size);
int SocketSendUdp(Socket* socket, char* buf, int size);
int SocketClose(Socket* socket);

#endif
