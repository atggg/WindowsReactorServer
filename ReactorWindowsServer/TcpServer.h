#pragma once
#include"ReactorPool.h"
class TcpServer
{
public:
	TcpServer(int port,int thNum);
	void run();
	void acceptConnect(void *arg);
private:
	bool listenInit(int port);
	int _fd;
	int _port;
	int _thNum;
	EventLoop *_mainLoop;
	ReactorPool* _reactorPool;
};

