#pragma once
#include<thread>
#include<string>
#include<mutex>
#include<queue>
#include<map>
#include<iostream>
#include"SelectDispatcher.h"
#include"socketpair.h"

class EventLoop;
class ChannelElement;

class ChannelElement
{
public:
	enum TaskType
	{
		ADD, DELETES, MODIFY
	};
	TaskType type;   // 如何处理该节点中的channel
	Channel* channel;
};

class EventLoop
{
public:
	enum IoType
	{
		SELECT = 0x1,
	};
	EventLoop(IoType io);
	EventLoop(std::string thName, IoType io);
	bool Run();
	bool AddTask(Channel& channel, ChannelElement::TaskType type);
	void ProcessTask();
	void eventActivate(int fd,int events);
	std::thread::id getThId();
	~EventLoop();
	void readLocalMessage(void* arg);
	void taskWakeup();
private:
	//判断是否退出
	bool _isQuit;
	Dispatcher* _dispatchar;
	//线程id
	std::thread::id _thId;
	//线程名字
	std::string _thName;
	//锁
	std::mutex _mtx;
	//socket对
	long long _sockPair[2];
	//任务队列
	std::queue<ChannelElement> _que;
	//
	std::map<int,Channel*> _list;
};


