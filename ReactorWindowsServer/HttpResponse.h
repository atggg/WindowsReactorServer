#pragma once
#include<string>
#include<map>
class HttpResponse
{
public:
	HttpResponse();
	void setVersion(std::string v);
	void setState(int state);
	void addHeader(std::pair<std::string,std::string> key_val);
	void clear();
	std::string makeHeadString();
private:

	std::string _version;
	unsigned int _state;
	std::string _stateString;
	std::map<std::string, std::string> _header;
};

