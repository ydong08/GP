#ifndef __JsonWrapper_H__
#define __JsonWrapper_H__

#include <string>
#include "json/json.h"

class JsonWrapper
{
public:
	JsonWrapper() {}
	~JsonWrapper() {}
	
public:
	static int parseJson(std::string &jsonStr, const char *szKey, int defaultValue);
	static int parseJson(Json::Value &parentNode, const char *szKey, int defaultValue);
	
	static std::string parseJson(std::string &jsonStr, const char *szKey, const std::string &defaultValue);
	static std::string parseJson(Json::Value &parentNode, const char *szKey, const std::string &defaultValue);
};

#endif /* __JsonWrapper_H__ */
