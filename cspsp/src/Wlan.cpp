#include "Wlan.h"
#include <emscripten/websocket.h>
#include <queue>
#include <vector>

static EMSCRIPTEN_WEBSOCKET_T proxy_socket;
static std::queue<std::vector<char>> messageQueue;

void printBinaryBuffer(char* buf, int size)
{
    for(int i = 0; i < size; ++i)
        printf(" %02X", buf[i]);
    printf("\n");
}

EM_BOOL WebSocketOpen(int eventType, const EmscriptenWebSocketOpenEvent *e, void *userData)
{
	printf("open(eventType=%d, userData=%d)\n", eventType, (int)userData);
	return 0;
}

EM_BOOL WebSocketClose(int eventType, const EmscriptenWebSocketCloseEvent *e, void *userData)
{
	printf("close(eventType=%d, wasClean=%d, code=%d, reason=%s, userData=%d)\n", eventType, e->wasClean, e->code, e->reason, (int)userData);
    return 0;
}

EM_BOOL WebSocketError(int eventType, const EmscriptenWebSocketErrorEvent *e, void *userData)
{
	printf("error(eventType=%d, userData=%d)\n", eventType, (int)userData);
	return 0;
}

EM_BOOL WebSocketMessage(int eventType, const EmscriptenWebSocketMessageEvent *e, void *userData)
{
	// printf("message(eventType=%d, userData=%d, data=%p, numBytes=%d, isText=%d)\n", eventType, (int)userData, e->data, e->numBytes, e->isText);
	
	std::vector<char> buffer(e->data, e->data + e->numBytes);
	messageQueue.push(buffer);

	// printBinaryBuffer((char*)e->data, e->numBytes);
	return 0;
}


int WlanInit()
{
	if (!emscripten_websocket_is_supported())
	{
		printf("WebSockets are not supported, cannot continue!\n");
        return 1;
	}

	EmscriptenWebSocketCreateAttributes attr;
	emscripten_websocket_init_create_attributes(&attr);

	char url[64];
	sprintf(url, "ws://%s:%d", PROXY_IP, PROXY_PORT);
	attr.url = url;

	proxy_socket = emscripten_websocket_new(&attr);
	if (proxy_socket <= 0)
	{
		printf("WebSocket creation failed, error code %d!\n", (EMSCRIPTEN_RESULT)proxy_socket);
        return 1;
	}

	emscripten_websocket_set_onopen_callback(proxy_socket, (void*)42, WebSocketOpen);
	emscripten_websocket_set_onclose_callback(proxy_socket, (void*)43, WebSocketClose);
	emscripten_websocket_set_onerror_callback(proxy_socket, (void*)44, WebSocketError);
	emscripten_websocket_set_onmessage_callback(proxy_socket, (void*)45, WebSocketMessage);

    return 0;
}

int WlanTerm()
{
	emscripten_websocket_close(proxy_socket, 0, 0);
	emscripten_websocket_delete(proxy_socket);
    return 0;
}

/*
	socket parameter is not used as we are using the proxy connection.
*/
int SocketConnect(Socket* socket, char* host, int port) 
{
	char buffer[32];
	sprintf(buffer, "CONN %s %d", host, port);
	return emscripten_websocket_send_binary(proxy_socket, buffer, strlen(buffer));
	return 1;
}

int SocketConnectUdp(Socket* socket, char* host, int port)
{
    return SocketConnect(socket, host, port); 
}

/*
	socket parameter is not used as we are using the proxy connection.
*/
int SocketRecv(Socket* socket, char* buf, int size)
{
	if(messageQueue.size() > 0){
		std::vector<char> buffer = messageQueue.front();
		memcpy(buf, buffer.data(), buffer.size());
		messageQueue.pop();
		return buffer.size();
	}

	return 0;
}

/*
	socket parameter is not used as we are using the proxy connection.
*/
int SocketSend(Socket* socket, char* buf, int size)
{   
    return emscripten_websocket_send_binary(proxy_socket, buf, size);
}

int SocketSendUdp(Socket* socket, char* buf, int size)
{
    return SocketSend(socket, buf, size);
}

int SocketClose(Socket* socket)
{
	while(messageQueue.size()>0)
		messageQueue.pop();
    return 0;
}