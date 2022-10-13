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
	TaskType type;   // ��δ���ýڵ��е�channel
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
	//�ж��Ƿ��˳�
	bool _isQuit;
	Dispatcher* _dispatchar;
	//�߳�id
	std::thread::id _thId;
	//�߳�����
	std::string _thName;
	//��
	std::mutex _mtx;
	//socket��
	long long _sockPair[2];
	//�������
	std::queue<ChannelElement> _que;
	//
	std::map<int,Channel*> _list;
};


