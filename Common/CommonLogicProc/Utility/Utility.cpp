#include <cassert>
#include "Utility.h"
#include "Logger.h"
#include "ErrorMsg.h"

int Utility::ParseJsonInt(std::string &jsonStr, const char *key, int defaultValue)
{
	Json::Reader reader;
	Json::Value rootNode;
	if (jsonStr.size() < 2 || jsonStr[0] != '{' || !reader.parse(jsonStr, rootNode))
	{
		// 参数错误
		LOGGER(E_LOG_ERROR) << "Json format error!";
		return defaultValue;
	}

	return ParseJsonInt(rootNode, key, defaultValue);
}

int Utility::ParseJsonInt(Json::Value &parentNode, const char *key, int defaultValue)
{
	int retVal = defaultValue;

	try
	{
		retVal = parentNode[key].asInt();
	}
	catch (...)
	{
		retVal = defaultValue;
	}

	return retVal;
}

std::string Utility::ParseJsonStr(std::string & jsonStr, const char * key, std::string defaultValue)
{
	Json::Reader reader;
	Json::Value rootNode;
	if (jsonStr.size() < 2 || jsonStr[0] != '{' || !reader.parse(jsonStr, rootNode))
	{
		// 参数错误
		LOGGER(E_LOG_ERROR) << "Json format error!";
		return defaultValue;
	}

	return ParseJsonStr(rootNode, key, defaultValue);
}

std::string Utility::ParseJsonStr(Json::Value & parentNode, const char * key, std::string defaultValue)
{
	std::string retVal = defaultValue;

	try
	{
		retVal = parentNode[key].asString();
	}
	catch (...)
	{
		retVal = defaultValue;
	}

	return retVal;
}

std::string Utility::DumpJson(Json::Value &json)
{
	Json::FastWriter writer;
	return writer.write(json);
}

// [min, max]
// base库中的 Util::rand_range(int, int) 不包括上限
int Utility::RandomRange(int min, int max)
{
	assert(min < max);

	return (rand() % (max - min + 1)) + min;
}

bool Utility::Between(int dest, std::pair<int, int> range)
{
	return Between(dest, range.first, range.second);
}

bool Utility::Between(int dest, int lowLimit, int upLimit)
{
	return (lowLimit <= dest && dest <= upLimit) ? true : false;
}

std::string Utility::getCardHexStr(unsigned char* cbCardData, int nCardCount)
{
	std::string strCards;
	char buf[4] = {0};
	for (int i = 0; i < nCardCount; i++) {
		snprintf(buf, 8, "%02x", cbCardData[i]);
		strCards += buf;
	}
	return strCards;
}