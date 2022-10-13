#pragma once
#include<thread>
#include<string>
#include<mutex>
#include<condition_variable>
#include"EventLoop.h"
class WorkerThread
{
public:
	WorkerThread();
	WorkerThread(int index, EventLoop::IoType io);
	void setName(int index);
	void SetIoType(EventLoop::IoType io);
	EventLoop* getEventLoop();
	void Run();
private:
	void RunIng();
	std::thread* _th;
	std::string _thName;
	std::mutex _mtx;
	std::condition_variable _cond;
	EventLoop *_evLoop;
	EventLoop::IoType _io;
};

