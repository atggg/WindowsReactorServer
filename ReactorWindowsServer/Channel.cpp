#include "Channel.h"

Channel::Channel(int fd, int events, std::function<void(void*)> readBack, std::function<void(void*)> writeBack, std::function<void(void*)> destroyBack, void* arg)
	:_fd(fd),_event(events),_readCallBack(readBack),_writeCallBack(writeBack), _destroyCallBack(destroyBack), _arg(arg)
{
}

Channel::Channel(const Channel& c)
{
	_fd = c._fd;
	_event = c._event;
	_readCallBack = c._readCallBack;
	_writeCallBack = c._writeCallBack;
	_destroyCallBack = c._destroyCallBack;
	_arg = c._arg;
}

int Channel::getFd()
{
	return _fd;
}

int Channel::getEvent()
{
	return _event;
}

bool Channel::isWriteEvent()
{
	return _event & WriteEvent;
}

void Channel::writeEvent(bool flag)
{
	if (flag)
	{
		_event |= WriteEvent;
	}
	else
	{
		_event &=~WriteEvent;
	}
}

void* Channel::getArg()
{
	return _arg;
}

std::function<void(void*)> Channel::call(CallType type)
{
	if (type == ReadCall)
	{
		return _readCallBack;
	}
	else if (type == WriteCall)
	{
		return _writeCallBack;
	}
	else if (type == DestroyCall)
	{
		return _destroyCallBack;
	}
}
