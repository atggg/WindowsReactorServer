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
	//�ļ���������
	int _fd;
	//�¼�
	int _event;
	//�ص�����
	void* _arg;
	//д�ص�
	std::function<void(void*)> _writeCallBack;
	//���ص�
	std::function<void(void*)> _readCallBack;
	//���ٻص�
	std::function<void(void*)> _destroyCallBack;
};

