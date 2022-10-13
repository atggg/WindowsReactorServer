#pragma once
#include<string>
#include<WinSock2.h>
class Buffer
{
public:
	Buffer(size_t size);
	size_t size();
	void ExtendRoom(size_t size);
	size_t readSize();
	size_t writeSize();
	void append(const std::string data);
	size_t scoketRecv(int fd);
	size_t socketSend(int fd);
	std::string getAEction(std::string endStr);
	std::string getAllData();
private:
	std::string _data;
	size_t _readPos;
	size_t _writePos;
};

