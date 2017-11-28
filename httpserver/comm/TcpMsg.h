#pragma once

#include <stdint.h>
#include "json/json.h"
#include "Packet.h"

class CTcpMsg
{
public:
	CTcpMsg(void);
	~CTcpMsg(void);
	static bool setMoney(Json::Value& jvRet, int32_t uMid, int64_t nMoney, uint8_t nType = 10);
    static bool getMoney(Json::Value& ret, int32_t uMid);
	static bool setBank(Json::Value& jvRet, int32_t uMid, int64_t nMoney, uint8_t nType = 10);
	static bool get( Json::Value& jvRet, int32_t uMid);
	static bool setRoll( Json::Value& jvRet, int32_t uMid,int32_t nRoll,int8_t nType=10);
	static bool setRollExp( Json::Value& jvRet, int32_t uMid,int32_t nRoll,int8_t nType = 10);
    static bool setNewMoney(const std::string& dealNo, int32_t uMid);
    
    // user server
    static bool sendMsg(int32_t mid, short type, const std::string& msg);
private:
	static bool SendAndGet(Json::Value& jvRet, OutputPacket& packSend);
};

