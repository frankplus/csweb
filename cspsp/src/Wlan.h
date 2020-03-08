#ifndef _WLAN_H_
#define _WLAN_H_

#include "JGE.h"
#include <stdio.h> // For perror() call
#include <stdlib.h> // For exit() call
// #include <windows.h>
// #include <winsock.h> // Include Winsock Library
// #include <iostream>

#define bzero(p, l) memset(p, 0, l)
#define bcopy(s, t, l) memmove(t, s, l)

#include <vector>

#define MAX_PICK 100

struct ConnectionConfig {
	char name[64];
	int index;
};

class Socket
{
public:
	int sock;
	// struct sockaddr_in addrTo; // ONLY WORKING ON WINDOWS
	// struct sockaddr_in addr;
	bool serverSocket;
	//Socket():  {};
};


static const char* wlanNotInitialized = "WLAN not initialized.";
static bool wlanInitialized = false;
static char resolverBuffer[1024];
static int resolverId;

int WlanInit();
int WlanTerm();
char* GetIPAddress();
int SetSockNoBlock(int s, u32 val);
int SocketConnect(Socket* socket, char* host, int port);
int SocketConnectUdp(Socket* socket, char* host, int port);
int SocketRecv(Socket* socket, char* buf, int size);
//sockaddr SocketRecvfrom(Socket* socket, char* buf, int size);
int SocketSend(Socket* socket, char* buf, int size);
int SocketSendUdp(Socket* socket, char* buf, int size);
int SocketClose(Socket* socket);

#endif
