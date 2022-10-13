#pragma once
#include"Buffer.h"
#include"EventLoop.h"
#include"HttpRequest.h"
#include"HttpResponse.h"
#include<filesystem>
#include<fstream>
class TcpConnection
{
public:
	TcpConnection(int fd, EventLoop *evLoop);
	~TcpConnection();
	void writeProcess(void* arg);
	void readProcess(void *arg);
	void closeProcess(void *arg);

	void HttpProcess();
private:
	// ͨ���ļ�����ȡ�ļ�������
	const char* getFiletype(const char* name);
	EventLoop* _evLoop;
	Buffer *_rbuff;
	Buffer *_wbuff;
	Channel *_channel;
	HttpRequest* _HttpRequest;
	HttpResponse* _HttpResponse;

};

