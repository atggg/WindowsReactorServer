#include "Buffer.h"

Buffer::Buffer(size_t size):_readPos(0),_writePos(0)
{
	_data = std::string(size,0);
}

size_t Buffer::size()
{
	return _data.size();
}

void Buffer::ExtendRoom(size_t size)
{
	if (readSize() >= size)
	{
		return;
	}
	else if (_readPos + writeSize() >= size)
	{
		memcpy(&_data[0], &_data[0] + _readPos, readSize());
		_writePos = readSize();
		_readPos = 0;
	}
	else
	{
		_data += std::string(_data.size() + size, 0);
	}

}

size_t Buffer::readSize()
{
	return _writePos - _readPos;
}

size_t Buffer::writeSize()
{
	return _data.size() - _writePos;
}
//写入数据
void Buffer::append(const std::string data)
{
	if (data.size() <= 0)
	{
		return;
	}
	ExtendRoom(data.size());
	memcpy(&_data[0] + _writePos, &data[0], data.size());
	_writePos += data.size();
}

//socket写入数据
size_t Buffer::scoketRecv(int fd)
{
	std::string buff(1024, 0);
	size_t count = 0;
	int len = 0;
	while ((len = recv(fd, &buff[0], 1024, 0))!=-1 && len !=0)
	{
		count += len;
		append(std::string(buff.c_str(), len));
	}
	return count;
}

size_t Buffer::socketSend(int fd)
{
	size_t len =  send(fd, _data.c_str() + _readPos, readSize(), 0);
	_readPos += len;
	return len;
}

//获取一段数据
std::string Buffer::getAEction(std::string endStr)
{
	size_t pos = 0;
	if (endStr.empty())
	{
		return std::string();
	}
	if ((pos = _data.find(endStr, _readPos)) == std::string::npos)
	{
		return std::string();
	}
	pos += endStr.size();
	std::string rtStr(_data.c_str()+_readPos, pos-_readPos);
	_readPos = pos;
	return rtStr;
}
//获取没有读取完的全部数据
std::string Buffer::getAllData()
{
	std::string rtStr(_data.c_str() + _readPos, _writePos - _readPos);
	_readPos = _writePos;
	return rtStr;
}
