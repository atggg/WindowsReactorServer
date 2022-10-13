#include "ReactorPool.h"

ReactorPool::ReactorPool(EventLoop* mainLoop, int thNum, EventLoop::IoType io)
:_mainLoop(mainLoop),_thNum(thNum),_isStart(false),_index(0)
{
	_workThreads = new WorkerThread[thNum];
	_io = io;
}

void ReactorPool::run()
{
	if (_mainLoop->getThId() != std::this_thread::get_id())
	{
		std::cout << "takeWorkerEventLoop 函数不是主线程" << std::endl;
		std::exit(0);
	}
	_isStart = true;
	for (int i = 0; i < _thNum; i++)
	{
		_workThreads[i].setName(i);
		_workThreads[i].SetIoType(_io);
		_workThreads[i].Run();
	}
}

EventLoop* ReactorPool::takeWorkerEventLoop()
{
	if (_mainLoop->getThId() != std::this_thread::get_id())
	{
		std::cout << "takeWorkerEventLoop 函数不是主线程" << std::endl;
		std::exit(0);
	}
	EventLoop* evLoop = _mainLoop;
	if (_thNum > 0)
	{
		evLoop = _workThreads[_index].getEventLoop();
		_index = ++_index % _thNum;
	}
	return evLoop;
}
