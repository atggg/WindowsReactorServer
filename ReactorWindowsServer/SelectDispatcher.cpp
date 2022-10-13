#include "SelectDispatcher.h"
#define MAX 1024

SelectDispatcher::SelectDispatcher()
{
	FD_ZERO(&_readSet);
	FD_ZERO(&_writeSet);
}

bool SelectDispatcher::add(Channel& channel)
{
	if (channel.getFd() >= MAX)
	{
		return false;
	}
	return setFdSet(channel);
}

bool SelectDispatcher::remove(Channel& channel)
{
	if (channel.getFd() >= MAX)
	{
		return false;
	}
	return unsetFdSet(channel);
}

bool SelectDispatcher::modify(Channel& channel)
{
	if (channel.getFd() >= MAX)
	{
		return false;
	}
	unsetFdSet(channel);
	return setFdSet(channel);
}

void SelectDispatcher::dispatch(EventLoop& evLoop, int timeOut)
{
	timeval val;
	val.tv_sec = timeOut;
	val.tv_usec = 0;
	fd_set rtmp = _readSet;
	fd_set wtmp = _writeSet;
	int count = select(MAX, &rtmp, &wtmp, nullptr, &val);
	if (count == SOCKET_ERROR && WSAENOTSOCK!= WSAGetLastError() && WSAEINVAL != WSAGetLastError())
	{
		std::cout << "select Rerror" << WSAGetLastError() << std::endl;
		std::exit(0);
	}
	if (count == SOCKET_ERROR &&  WSAEINVAL == WSAGetLastError())
	{
		goto end;
	}
	for (int i = 0; i < MAX; i++)
	{
		if (FD_ISSET(i,&rtmp))
		{
			//读事件
			evLoop.eventActivate(i, Channel::readEvent);
		}
		if (FD_ISSET(i,&wtmp))
		{
			//写事件
			evLoop.eventActivate(i, Channel::WriteEvent);
		}
	}
end:
	val.tv_usec = 0;
}

SelectDispatcher::~SelectDispatcher()
{
	
}

bool SelectDispatcher::setFdSet(Channel& channel)
{
	if (channel.getEvent() & Channel::readEvent)
	{
		FD_SET(channel.getFd(),& _readSet);
	}
	if (channel.getEvent() & Channel::WriteEvent)
	{
		FD_SET(channel.getFd(), &_writeSet);
	}
	return true;
}

bool SelectDispatcher::unsetFdSet(Channel& channel)
{
	FD_CLR(channel.getFd(),&_readSet);
	FD_CLR(channel.getFd(), &_writeSet);
	return true;
}
