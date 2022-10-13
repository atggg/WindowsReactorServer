#include "TcpConnection.h"

TcpConnection::TcpConnection(int fd, EventLoop* evLoop)
{
	_evLoop = evLoop;
	_rbuff = new Buffer(10240);
	_wbuff = new Buffer(10240);
	_HttpRequest = new HttpRequest;
	_HttpResponse = new HttpResponse;
	unsigned long ul = 1;
	ioctlsocket(fd, FIONBIO, &ul);
	_channel = new Channel(fd, Channel::readEvent, 
		std::bind(&TcpConnection::readProcess,this, std::placeholders::_1), 
		std::bind(&TcpConnection::writeProcess, this, std::placeholders::_1), 
		std::bind(&TcpConnection::closeProcess, this, std::placeholders::_1)
		, this);
	_evLoop->AddTask(*_channel, ChannelElement::ADD);
}

TcpConnection::~TcpConnection()
{
	if (_rbuff != nullptr)
	{
		delete _rbuff;
		_rbuff = nullptr;
	}
	if (_wbuff != nullptr)
	{
		delete _wbuff;
		_wbuff = nullptr;
	}
	if (_channel != nullptr)
	{
		delete _channel;
		_channel = nullptr;
	}
}

void TcpConnection::writeProcess(void* arg)
{
	if (arg != this)
	{
		std::cout << "writeProcess error" << std::endl;
		std::exit(0);
	}
	std::cout << "写事件触发" << std::endl;
	_wbuff->socketSend(_channel->getFd());
	if (_wbuff->readSize() == 0)
	{
		_channel->writeEvent(false);
		_evLoop->AddTask(*_channel, ChannelElement::MODIFY);
	}
}

void TcpConnection::readProcess(void* arg)
{
	if (arg != this)
	{
		std::cout << "writeProcess error" << std::endl;
		std::exit(0);
	}
	std::cout << "读事件触发" << std::endl;
	size_t len =  _rbuff->scoketRecv(_channel->getFd());
	if (len == 0)
	{
		//调用关闭回调
		_evLoop->eventActivate(_channel->getFd(), Channel::CloseEvent);
		return;
	}
	 if (_HttpRequest->parse(*_rbuff))
	 {
		 //解析请求头成功
		 //处理http请求
		 HttpProcess();
	 }
	 else
	 {
		 //解析失败
		 _HttpResponse->setState(400);
		 _HttpResponse->setVersion("HTTP/1.1");
		 _HttpResponse->addHeader(std::make_pair(std::string("Content-Length"), std::string("0")));
		 _wbuff->append(_HttpResponse->makeHeadString());
	 }
	 //开启写事件
	 _channel->writeEvent(true);
	 _evLoop->AddTask(*_channel, ChannelElement::MODIFY);
}

void TcpConnection::closeProcess(void* arg)
{
	if (arg != this)
	{
		std::cout << "writeProcess error" << std::endl;
		std::exit(0);
	}
	std::cout << "关闭事件触发" << std::endl;
	delete this;
}

void TcpConnection::HttpProcess()
{
	//判断发送文件还是数据
	std::string path =  _HttpRequest->path();
	if (path == "/")
	{
		path = "./";
	}
	else
	{
		path.erase(path.begin());
	}
	std::filesystem::path file(path);
	if (!std::filesystem::exists(file))
	{
		//没有这个路径
		std::cout << "文件或者目录不存在" << std::endl;
		_HttpResponse->setState(404);
		_HttpResponse->setVersion("HTTP/1.1");
		_HttpResponse->addHeader(std::make_pair(std::string("Content-Type"), getFiletype(".html")));
		_HttpResponse->addHeader(std::make_pair(std::string("Content-Length"), std::string("0")));
		_wbuff->append(_HttpResponse->makeHeadString());
		goto end;

	}
	if (std::filesystem::is_directory(file))
	{
		//是目录
		std::cout << "开始发送目录" << std::endl;


		//组织目录的数据
		std::string dirHtml = "<html><head><title>"+path+"</title></head><body><table>";
		auto begin = std::filesystem::directory_iterator(file); //获取文件系统迭代器
		auto end = std::filesystem::directory_iterator();    //end迭代器
		for (auto it = begin; it != end; it++) {
			auto& entry = *it;
			std::string fileName = entry.path().string();
			fileName.erase(0, path.size());
			size_t fileSize = entry.file_size();
			if (std::filesystem::is_regular_file(entry)) {
				//如果是文件
				dirHtml += "<tr><td><a href=\""+fileName + "\">" + fileName + "</a></td><td>" + std::to_string(fileSize) + "</td><td>FILE</td></tr>";
			}
			else if (std::filesystem::is_directory(entry)) {
				dirHtml += "<tr><td><a href=\"" + fileName + "/\">" + fileName + "</a></td><td>" + std::to_string(fileSize) + "</td><td>DIR</td></tr>";
			}
		}
		dirHtml += "</table></body></html>";
		_HttpResponse->setState(200);
		_HttpResponse->setVersion("HTTP/1.1");
		_HttpResponse->addHeader(std::make_pair(std::string("Content-Type"), getFiletype(".html")));
		_HttpResponse->addHeader(std::make_pair(std::string("Content-Length"), std::to_string(dirHtml.size())));
		_wbuff->append(_HttpResponse->makeHeadString());
		_wbuff->append(dirHtml);
	}
	else
	{
		//是文件
		std::cout << "开始发送文件" << std::endl;
		std::ifstream fileHander(path, std::ios::binary | std::ios::ate);
		size_t fileSize = 0;
		int state = 200;
		std::string fileData = "";
		if (fileHander.is_open())
		{
			fileSize = fileHander.tellg();
			fileHander.seekg(std::ios::beg);
			fileData+=std::string(fileSize, 0);
			fileHander.read(&fileData[0], fileSize);
		}
		else
		{
			state = 502;
			fileSize = 0;
		}
		
		_HttpResponse->setState(state);
		_HttpResponse->setVersion("HTTP/1.1");
		_HttpResponse->addHeader(std::make_pair(std::string("Content-Type"), getFiletype(state /100 == 2? path.c_str():".html")));
		_HttpResponse->addHeader(std::make_pair(std::string("Content-Length"), std::to_string(fileSize)));
		_wbuff->append(_HttpResponse->makeHeadString());
		_wbuff->append(fileData);
	}
end:
	_HttpRequest->clear();
	_HttpResponse->clear();
}

const char* TcpConnection::getFiletype(const char* name)
{
	char* dot;

	// 自右向左查找‘.’字符, 如不存在返回NULL
	dot = (char*)strrchr(name, '.');
	if (dot == NULL)
		return "text/plain; charset=utf-8";
	if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
		return "text/html; charset=utf-8";
	if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
		return "image/jpeg";
	if (strcmp(dot, ".gif") == 0)
		return "image/gif";
	if (strcmp(dot, ".png") == 0)
		return "image/png";
	if (strcmp(dot, ".css") == 0)
		return "text/css";
	if (strcmp(dot, ".au") == 0)
		return "audio/basic";
	if (strcmp(dot, ".wav") == 0)
		return "audio/wav";
	if (strcmp(dot, ".avi") == 0)
		return "video/x-msvideo";
	if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
		return "video/quicktime";
	if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
		return "video/mpeg";
	if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
		return "model/vrml";
	if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
		return "audio/midi";
	if (strcmp(dot, ".mp3") == 0)
		return "audio/mpeg";
	if (strcmp(dot, ".mp4") == 0 || strcmp(dot, ".m4a") == 0)
		return "audio/mp4";
	if (strcmp(dot, ".ogg") == 0)
		return "application/ogg";
	if (strcmp(dot, ".pac") == 0)
		return "application/x-ns-proxy-autoconfig";

	return "text/plain; charset=utf-8";
}
