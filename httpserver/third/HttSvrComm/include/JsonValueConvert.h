#ifndef JsonValueConvert_h__
#define JsonValueConvert_h__
#pragma once

#include "json/json.h"

namespace Json
{
	int32_t SafeConverToInt32(const Json::Value& jvConvert, int32_t nDefRet = 0);
	uint32_t SafeConverToUInt32(const Json::Value& jvConvert, uint32_t nDefRet = 0);
	int64_t SafeConverToInt64(const Json::Value& jvConvert, int64_t nDefRet = 0);
	Json::Value SafeConverToObj(Json::Value& jvConvert);
	std::string SafeConverToStr(const Json::Value& jvConvert, std::string nDefRet = "");
}

#endif // JsonValueConvert_h__