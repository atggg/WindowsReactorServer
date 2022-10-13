#pragma once
#include<map>
#include<string>
#include<vector>
#include"Buffer.h"
class HttpRequest
{
public:
	HttpRequest();
	void clear();
	bool parse(Buffer& buff);
	std::string& method();
	std::string& url();
	std::string& path();
	std::map<std::string, std::string>& GET();
	std::string& version();
	std::map<std::string, std::string>& header();
	std::string& postData();
	std::map<std::string, std::string> &POST();
private:
	std::string _method;
	std::string _url;
	std::string _path;
	std::map<std::string, std::string> _GET;
	std::string _version;
	std::map<std::string, std::string> _header;
	std::string _postData;
	std::map<std::string, std::string> _POST;
};

