#pragma once
#include"Channel.h"
class EventLoop;
class Dispatcher
{
public:
	virtual bool add(Channel& channel) = 0;
	virtual bool remove(Channel& channel) = 0;
	virtual bool modify(Channel& channel) = 0;
	virtual void dispatch(EventLoop & evLoop, int timeOut) = 0;
	virtual ~Dispatcher();
};

