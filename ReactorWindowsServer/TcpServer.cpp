#include "TcpServer.h"
#include<WinSock2.h>
#include"TcpConnection.h"

TcpServer::TcpServer(int port, int thNum)
	:_thNum(thNum),_port(port)
{
	if (!listenInit(port))
	{
		std::cout << "������ʼ��ʧ����" << std::endl;
		std::exit(0);
	}
	_mainLoop = new EventLoop(EventLoop::SELECT);
	_reactorPool = new ReactorPool(_mainLoop, thNum,EventLoop::SELECT);
}

void TcpServer::run()
{
	_reactorPool->run();
	Channel channel(_fd, Channel::readEvent, std::bind(&TcpServer::acceptConnect, this, std::placeholders::_1), nullptr, nullptr, nullptr);
	_mainLoop->AddTask(channel, ChannelElement::ADD);
	_mainLoop->Run();
}

void TcpServer::acceptConnect(void* arg)
{
	sockaddr_in caddr;
	int len = sizeof(caddr);
	int cfd = accept(_fd, (sockaddr*)&caddr, &len);
	if (cfd != -1)
	{
		std::cout << "�յ�һ������" << std::endl;
		new TcpConnection(cfd, _reactorPool->takeWorkerEventLoop());
	}
}

bool TcpServer::listenInit(int port)
{
	//��ʼ��WSA  
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return false;
	}

	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1)
	{
		std::cout << "socket Error" << std::endl;
		return false;
	}
	int rc = 0;
	//rc = setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, nullptr, 0);
	//if (rc == -1)
	//{
	//	std::cout << "setsockopt Error" << std::endl;
	//	return false;
	//}
	sockaddr_in addr;
	addr.sin_addr.s_addr = 0;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	rc = bind(_fd, (sockaddr*)&addr, sizeof(addr));
	if (rc == -1)
	{
		std::cout << "bind Error" << std::endl;
		return false;
	}
	rc = listen(_fd, 128);
	if (rc == -1)
	{
		std::cout << "listen Error" << std::endl;
		return false;
	}
	return true;
}
