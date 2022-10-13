#pragma once
#include<functional>
class Channel
{
	
public:
	enum Event
	{
		readEvent = 0x2,
		WriteEvent = 0x4,
		CloseEvent = 0x8,
		ET = 0x10,
	};
	enum CallType
	{
		WriteCall,
		ReadCall,
		DestroyCall,
	};
	
	Channel(int fd,int events,std::function<void(void*)>readBack, std::function<void(void*)>writeBack, std::function<void(void*)>destroyBack,void*arg);
	Channel(const Channel& c);
	int getFd();
	int getEvent();
	bool isWriteEvent();
	void writeEvent(bool flag);
	void* getArg();
	std::function<void(void*)> call(CallType type);
private:
	//文件描述符号
	int _fd;
	//事件
	int _event;
	//回调参数
	void* _arg;
	//写回调
	std::function<void(void*)> _writeCallBack;
	//读回调
	std::function<void(void*)> _readCallBack;
	//销毁回调
	std::function<void(void*)> _destroyCallBack;
};

