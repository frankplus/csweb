#include "HttpManager.h"
#include <emscripten/fetch.h>

using namespace std;

void downloadSucceeded(emscripten_fetch_t *fetch) {
	printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
	// The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
	HttpManager *httpManager = HttpManager::getInstance();
	httpManager->SetResponse(fetch->data, fetch->numBytes);
	emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void downloadFailed(emscripten_fetch_t *fetch) {
  printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
  emscripten_fetch_close(fetch); // Also free data on failure.
}

//------------------------------------------------------------------------------------------------
void HttpManager::Init()
{
	strcpy(mBuffer,"");
	strcpy(mHost,"");
	strcpy(mHostText,"");
	mPort = 0;
	mContentSize = -1;
}

//------------------------------------------------------------------------------------------------
void HttpManager::Connect(char* host, char *hosttext, int port)
{
	strcpy(mHost,host);
	strcpy(mHostText,hosttext);
	mPort = port;
}

//------------------------------------------------------------------------------------------------
void HttpManager::Reconnect()
{
}

//------------------------------------------------------------------------------------------------
void HttpManager::Disconnect()
{
}

//------------------------------------------------------------------------------------------------
void HttpManager::SendRequest(char* page, char *data, int type)
{
	string url = "https://" + string(mHost) + ":" + to_string(mPort) + string(page) ;
	const char *req_type = type == REQUEST_POST ? "POST" : "GET";

	char content_length[10];
	sprintf(content_length, "%d", strlen(data));
	static const char* custom_headers[3] = {
		"Content-Type", "application/x-www-form-urlencoded",
		nullptr
	};

	emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	strcpy(attr.requestMethod, req_type);
	attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
	attr.onsuccess = downloadSucceeded;
	attr.onerror = downloadFailed;
	attr.requestData = data;
	attr.requestDataSize = strlen(data);
	attr.requestHeaders = attr.requestHeaders;
	attr.requestHeaders = custom_headers;

	emscripten_fetch(&attr, url.c_str());

	printf("url = %s - data = %s - host = %s \n", url.c_str(), data, mHostText);
}

//------------------------------------------------------------------------------------------------
int HttpManager::GetResponse(char* buffer)
{
	//char* buffer = NULL;
	if (mResponses.size() > 0) {
		Response response = mResponses.front();
		memcpy(buffer,response.buffer,response.size);
		buffer[response.size] = '\0';
		mResponses.pop_front();
		return response.size;
	}

	return -1;
}

//------------------------------------------------------------------------------------------------
int HttpManager::GetContentSize()
{
	int temp = mContentSize;
	mContentSize = -1; //reset
	return temp;
}

//------------------------------------------------------------------------------------------------
bool HttpManager::ResponseReady()
{
	if (mResponses.size() > 0) {
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------
void HttpManager::ClearRequests()
{
	while (mRequests.size() > 0) {
		mRequests.pop_front();
	}
}

//------------------------------------------------------------------------------------------------
void HttpManager::SetResponse(const char* res, int size)
{
	Response response;
	memcpy(response.buffer,res,size);
	response.size = size;
	mResponses.push_back(response);
}

//------------------------------------------------------------------------------------------------
void HttpManager::Update(float dt)
{

}