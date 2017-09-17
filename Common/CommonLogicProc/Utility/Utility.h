#ifndef __Utility_H__
#define __Utility_H__

#include <string>
#include "json/json.h"
#include "tinyxml.h"

class Utility
{
public:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Parse json.

	static int ParseJsonInt(std::string &jsonStr, const char *key, int defaultValue);
	static int ParseJsonInt(Json::Value &parentNode, const char *key, int defaultValue);

	static std::string ParseJsonStr(std::string &jsonStr, const char *key, std::string defaultValue);
	static std::string ParseJsonStr(Json::Value &parentNode, const char *key, std::string defaultValue);
	static std::string DumpJson(Json::Value &json);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Math

	// [min, max]
	static int RandomRange(int min, int max);

	static bool Between(int dest, std::pair<int, int> range);
	static bool Between(int dest, int lowLimit, int upLimit);

	//获取牌的16进制字符串
	static 	std::string getCardHexStr(unsigned char* cbCardData, int nCardCount);
};

#endif // !__Utility_H__
