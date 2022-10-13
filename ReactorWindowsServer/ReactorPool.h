#pragma once
#include"EventLoop.h"
#include"WorkerThread.h"
class ReactorPool
{
public:
	ReactorPool(EventLoop * mainLoop,int thNum,EventLoop::IoType io);
	void run();
	EventLoop* takeWorkerEventLoop();
private:
	EventLoop* _mainLoop;
	EventLoop::IoType _io;
	bool _isStart;
	int _thNum;
	WorkerThread* _workThreads;
	int _index;
};

