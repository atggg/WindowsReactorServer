#pragma once
#include<WinSock2.h>
#include "Dispatcher.h"
#include<iostream>
#include"EventLoop.h"
class SelectDispatcher:public Dispatcher
{
public:
    SelectDispatcher();
    virtual bool add(Channel& channel);
    virtual bool remove(Channel& channel);
    virtual bool modify(Channel& channel);
    virtual void dispatch(EventLoop& evLoop, int timeOut);
    virtual ~SelectDispatcher();
private:
    bool setFdSet(Channel& channel);
    bool unsetFdSet(Channel &channel);
    fd_set _readSet;
    fd_set _writeSet;
};

