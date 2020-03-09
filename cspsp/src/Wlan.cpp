#include "Wlan.h"
#include <emscripten/websocket.h>

#define BUFFER_SIZE 4096

static EMSCRIPTEN_WEBSOCKET_T proxy_socket;
static char message_buffer[BUFFER_SIZE];
static int size_available = 0;
static int writeIndex = 0;
static int readIndex = 0;

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
	emscripten_websocket_close(e->socket, 0, 0);
    return 0;
}

EM_BOOL WebSocketError(int eventType, const EmscriptenWebSocketErrorEvent *e, void *userData)
{
	printf("error(eventType=%d, userData=%d)\n", eventType, (int)userData);
    emscripten_websocket_close(e->socket, 0, 0);
	return 0;
}

EM_BOOL WebSocketMessage(int eventType, const EmscriptenWebSocketMessageEvent *e, void *userData)
{
	// printf("message(eventType=%d, userData=%d, data=%p, numBytes=%d, isText=%d)\n", eventType, (int)userData, e->data, e->numBytes, e->isText);

	for(int i=0; i<e->numBytes; i++)
	{
		if(size_available == BUFFER_SIZE)
		{
			printf("Network buffer is full! \n");
			return 1;
		}

		message_buffer[writeIndex] = e->data[i];
		writeIndex = (writeIndex + 1) % BUFFER_SIZE;
		size_available++;
	}

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

	attr.url = "ws://localhost:2900";

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
    return 0;
}

int SocketConnect(Socket* socket, char* host, int port) 
{
    // char buf[128];
    // sprintf(buf, "CONNECT %s:%d", host, port);
    // return emscripten_websocket_send_binary(proxy_socket, buf, strlen(buf));

	size_available = 0;
	writeIndex = 0;
	readIndex = 0;

	return 1;
}

int SocketConnectUdp(Socket* socket, char* host, int port)
{
    return SocketConnect(socket, host, port); 
}

int SocketRecv(Socket* socket, char* buf, int size)
{
	int read_bytes = 0;
	for(read_bytes=0; read_bytes<size; read_bytes++)
	{
		if(size_available == 0)
			break;

		buf[read_bytes] = message_buffer[readIndex];
		readIndex = (readIndex + 1) % BUFFER_SIZE;
		size_available--;
	}

	return read_bytes;
}

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
    return 0;
}