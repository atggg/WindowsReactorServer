#include "HttpRequest.h"

HttpRequest::HttpRequest():_method(""),_url(""),_version(""),_postData("")
{
}

void HttpRequest::clear()
{
	_method.clear();
	_url.clear();
	_path.clear();
	_GET.clear();
	_version.clear();
	_header.clear();
	_postData.clear();
	_POST.clear();
	
}

bool HttpRequest::parse(Buffer& buff)
{
	//解析请求行的信息
	std::string line = buff.getAEction("\r\n");
	if (line != "")
	{
		size_t pos = line.find(" ", 0);
		size_t pos2 = 0;
		if (pos != std::string::npos)
		{
			_method = std::string(line.c_str(), pos);
		}
		else
		{
			return false;
		}
		pos += 1;
		pos2 = pos;
		pos = line.find(" ", pos);
		if (pos != std::string::npos)
		{
			_url = std::string(line.c_str() + pos2, pos - pos2);
		}
		else
		{
			return false;
		}
		pos += 1;
		pos2 = pos;
		pos = line.find("\r\n", pos);
		if (pos != std::string::npos)
		{
			_version = std::string(line.c_str() + pos2, pos - pos2);
		}
		else
		{
			return false;
		}
		pos = _url.find("?");
		std::string getData;
		if (pos != std::string::npos)
		{
			_path = std::string(_url.c_str(), pos);
			getData = std::string(_url.c_str()+pos +1);
		}
		else
		{
			_path = _url;
		}
		pos = getData.find("&");
		while (pos != std::string::npos || getData != "")
		{
			bool flag = false;
			std::string getDataline;
			if (pos != std::string::npos)
			{
				getDataline = std::string(getData.c_str(), pos);
				getData = std::string(getData.c_str() + pos + 1);
			}
			else
			{
				getDataline = getData;
				flag = true;
			}
			pos = getDataline.find("=");
			if (pos != std::string::npos)
			{
				std::string key(getDataline.c_str(), pos);
				std::string val(getDataline.c_str() + pos + 1);
				_GET.insert(std::make_pair(key, val));
			}
			else
			{
				return false;
			}
			if (flag)
			{
				break;
			}
			pos = getData.find("&");
		}
	}
	else
	{
		return false;
	}
	//开始解析请求头信息
	std::string headline = buff.getAEction("\r\n");
	while (headline != "" && headline !="\r\n")
	{
		size_t pos = 0;
		size_t pos2 = 0;
		pos = headline.find(":");
		if (pos == std::string::npos)
		{
			return false;
		}
		std::string key(headline.c_str(),pos);
		pos += 1;
		pos2 = pos;
		pos = headline.find("\r\n");
		std::string val(headline.c_str() + pos2, pos - pos2);
		//去除首位空格
		auto fun = [](std::string &str) {
			int s = str.find_first_not_of(" ");
			int e = str.find_last_not_of(" ");
			str = str.substr(s, e - s + 1);
			return;
		};
		fun(val);
		fun(key);
		_header.insert(std::make_pair(key, val));

		headline = buff.getAEction("\r\n");
	}
	//解析post数据
	if (_method == "post" || _method == "POST")
	{
		_postData = buff.getAllData();
	}
	auto Content_Type = _header.find("Content-Type");
	if (_postData!="" && Content_Type != _header.end() && Content_Type->second == "application/x-www-form-urlencoded")
	{
		std::string PostData = _postData;
		size_t pos = PostData.find("&");
		while (pos != std::string::npos || PostData != "")
		{
			bool flag = false;
			std::string PostDataline;
			if (pos != std::string::npos)
			{
				PostDataline = std::string(PostData.c_str(), pos);
				PostData = std::string(PostData.c_str() + pos + 1);
			}
			else
			{
				PostDataline = PostData;
				flag = true;
			}
			pos = PostDataline.find("=");
			if (pos != std::string::npos)
			{
				std::string key(PostDataline.c_str(), pos);
				std::string val(PostDataline.c_str() + pos + 1);
				_POST.insert(std::make_pair(key, val));
			}
			if (flag)
			{
				break;
			}
			pos = PostData.find("&");
		}
	}
	return true;
}

std::string& HttpRequest::method()
{
	return _method;
}

std::string& HttpRequest::url()
{
	return _url;
}

std::string& HttpRequest::path()
{
	return _path;
}

std::map<std::string, std::string>& HttpRequest::GET()
{
	return _GET;
}

std::string& HttpRequest::version()
{
	return _version;
}

std::map<std::string, std::string>& HttpRequest::header()
{
	return _header;
}

std::string& HttpRequest::postData()
{
	return _postData;
}

std::map<std::string, std::string>& HttpRequest::POST()
{
	return _POST;
}
