#include "WorkerThread.h"

WorkerThread::WorkerThread()
{
	_th = nullptr;
	_thName = "";
	_evLoop = nullptr;
}

WorkerThread::WorkerThread(int index,EventLoop::IoType io)
{
	_th = nullptr;
	_thName = "Thread-" + std::to_string(index);
	_evLoop = nullptr;
	_io = io;
}

void WorkerThread::setName(int index)
{
	_thName = "Thread-" + std::to_string(index);
}

EventLoop* WorkerThread::getEventLoop()
{
	return _evLoop;
}

void WorkerThread::SetIoType(EventLoop::IoType io)
{
	_io = io;
}

void WorkerThread::Run()
{
	
	if (_th == nullptr)
	{
		_th = new std::thread(std::bind(&WorkerThread::RunIng,this));
	}
	
	while (_evLoop == nullptr)
	{
		std::unique_lock<std::mutex> m(_mtx);
		_cond.wait(m);
	}
}

void WorkerThread::RunIng()
{
	_evLoop = new EventLoop(_thName, _io);
	_cond.notify_all();
	_evLoop->Run();
}
