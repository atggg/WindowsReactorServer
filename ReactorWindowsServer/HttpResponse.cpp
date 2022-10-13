#include "HttpResponse.h"

HttpResponse::HttpResponse():_version(""),_state(0)
{
}

void HttpResponse::setVersion(std::string v)
{
	_version = v;
}

void HttpResponse::setState(int state)
{
	_state = state;
}

void HttpResponse::addHeader(std::pair<std::string, std::string> key_val)
{
	_header.insert(key_val);
}

void HttpResponse::clear()
{
	_version.clear();
	_state = 0;
	_stateString.clear();
	_header.clear();
}

std::string HttpResponse::makeHeadString()
{
	std::string headerString = "";
	headerString += _version + " " + std::to_string(_state) +" " + _stateString + "\r\n";
	for (auto m = _header.begin(); m != _header.end(); m++)
	{
		headerString += m->first + ": " + m->second + "\r\n";
	}
	headerString += "\r\n";
	return headerString;
}
