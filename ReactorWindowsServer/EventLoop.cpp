#include "EventLoop.h"

EventLoop::EventLoop(IoType io):EventLoop("MainThread",io){}

EventLoop::EventLoop(std::string thName, IoType io):_thName(thName),_isQuit(false)
{
	if (io == SELECT)
	{
		_dispatchar = new SelectDispatcher();
	}
	_thId = std::this_thread::get_id();
	int ret = socketpair(AF_INET, SOCK_STREAM, 0, _sockPair);
	if (ret == -1)
	{
		std::cout << "socketpair Error" << std::endl;
		std::exit(0);
	}
	Channel channel(_sockPair[1], Channel::readEvent, std::bind(&EventLoop::readLocalMessage, this, std::placeholders::_1), nullptr, nullptr, this);
	AddTask(channel, ChannelElement::ADD);
}

bool EventLoop::Run()
{
	if (std::this_thread::get_id() != _thId)
	{
		return false;
	}
	while (!_isQuit)
	{
		_dispatchar->dispatch(*this,200);
		ProcessTask();
	}
	return true;
}

bool EventLoop::AddTask(Channel& channel, ChannelElement::TaskType type)
{
	_mtx.lock();
	ChannelElement node;
	node.channel = new Channel(channel);
	node.type = type;
	_que.push(node);
	_mtx.unlock();
	if (_thId == std::this_thread::get_id())
	{
		//当前子线程
		ProcessTask();
	}
	else
	{
		//当前主线程
		taskWakeup();
	}
	return false;
}

void EventLoop::ProcessTask()
{
	_mtx.lock();
	while (!_que.empty())
	{
		ChannelElement node = _que.front();
		_que.pop();
		if (node.type == ChannelElement::ADD)
		{
			_list.insert(std::make_pair(node.channel->getFd(), node.channel));
			_dispatchar->add(*node.channel);
		}
		else if (node.type == ChannelElement::DELETES)
		{
			auto c = _list.find(node.channel->getFd());
			if (c != _list.end())
			{
				_dispatchar->remove(*node.channel);
				delete c->second;
				_list.erase(c);
				delete node.channel;
			}
		}
		else if (node.type == ChannelElement::MODIFY)
		{
			auto c = _list.find(node.channel->getFd());
			if (c != _list.end())
			{
				delete c->second;
				c->second = node.channel;
				_dispatchar->modify(*node.channel);
			}
		}
	}
	_mtx.unlock();
}

void EventLoop::eventActivate(int fd, int events)
{
	auto c = _list.find(fd);
	if (c != _list.end())
	{
		Channel* channel = c->second;
		//读事件
		if (Channel::readEvent & events && channel->call(Channel::ReadCall) !=nullptr)
		{
			channel->call(Channel::ReadCall)(channel->getArg());
		}
		//写事件
		else if (Channel::WriteEvent & events && channel->call(Channel::WriteCall) != nullptr)
		{
			channel->call(Channel::WriteCall)(channel->getArg());
		}
		//断开事件
		else if (Channel::CloseEvent & events && channel->call(Channel::DestroyCall) != nullptr)
		{
			channel->call(Channel::DestroyCall)(channel->getArg());
			AddTask(*channel, ChannelElement::DELETES);
		}
	}
}

std::thread::id EventLoop::getThId()
{
	return _thId;
}

EventLoop::~EventLoop()
{
	if (_dispatchar != nullptr)
	{
		delete _dispatchar;
	}
	for (auto m = _list.begin(); m != _list.end(); m++)
	{
		delete m->second;
	}
}

void EventLoop::readLocalMessage(void* arg)
{
	EventLoop* evLoop = (EventLoop*)arg;
	char buf[256];
	recv(evLoop->_sockPair[1], buf, sizeof(buf),0);
}

void EventLoop::taskWakeup()
{
	const std::string msg = "test";
	send(_sockPair[0], msg.c_str(), msg.size(),0);
}
