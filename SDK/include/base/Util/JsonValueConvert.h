#ifndef JsonValueConvert_h__
#define JsonValueConvert_h__
#pragma once

#include "json/json.h"

namespace Json
{
	int32_t SafeConverToInt32(const Json::Value& jvConvert, int32_t nDefRet = 0);
	uint32_t SafeConverToUInt32(const Json::Value& jvConvert, uint32_t nDefRet = 0);
	int64_t SafeConverToInt64(const Json::Value& jvConvert, int64_t nDefRet = 0);
	std::string SafeConverToStr(const Json::Value& jvConvert, std::string nDefRet = "");

	//还要支持中括号等等，以后如果有需要再完成吧
#if 0
	class JSON_API ValueEx : public Value 
	{
	public:
		ValueEx(ValueType type = nullValue) :Value(type) {};
		ValueEx(Int value) : Value(value) {};
		ValueEx(UInt value) : Value(value) {};
#if defined(JSON_HAS_INT64)
		ValueEx(Int64 value) : Value(value) {};
		ValueEx(UInt64 value) : Value(value) {};
#endif // if defined(JSON_HAS_INT64)
		ValueEx(double value) : Value(value) {};
		ValueEx(const char* value) : Value(value) {}; ///< Copy til first 0. (NULL causes to seg-fault.)
		ValueEx(const char* begin, const char* end) : Value(begin, end) {}; ///< Copy all, incl zeroes.
		ValueEx(const StaticString& value) : Value(value) {};
		ValueEx(const JSONCPP_STRING& value) : Value(value) {}; ///< Copy data() til size(). Embedded zeroes too.
#ifdef JSON_USE_CPPTL
		ValueEx(const CppTL::ConstString& value) : Value(value) {};
#endif
		ValueEx(bool value) : Value(value) {};
		ValueEx(const Value& other) : Value(other) {};
#if JSON_HAS_RVALUE_REFERENCES
		ValueEx(Value&& other) : Value(other) {};
#endif
		int32_t asInt32Safe(int32_t nDefRet = 0);
		uint32_t asUInt32Safe(uint32_t nDefRet = 0);
#if defined(JSON_HAS_INT64)
		int64_t asInt64Safe(int64_t nDefRet = 0);
		uint64_t asUInt64Safe(uint64_t nDefRet = 0);
#endif // if defined(JSON_HAS_INT64)
		std::string asStrSafe(std::string nDefRet = "");
	};
#endif
}



#endif // JsonValueConvert_h__


