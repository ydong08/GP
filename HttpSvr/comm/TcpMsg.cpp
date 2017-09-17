#include "TcpMsg.h"

#include "../threadres.h"

#define	CMD_GET				0x0460	//获取用户的金币，乐券等信息
#define	CMD_SETMONEY		0x0462	//加金币
#define	CMD_PHP_SET_MOENY	0x0470	//加金币
#define	CMD_SETBANK			0x0463	//更新保险箱金币信息
#define	CMD_SETROLL			0x0464	//更新乐劵
#define	CMD_SETROLLEXP		0x0465	//更新过期乐劵
#define	CMD_NOTICEPAY		0x0090	//充值发信息
#define	CMD_HORN			0x0071	//发喇叭
#define	CMD_CHECK			0x0401	//检查用用户名
#define  CMD_SETEXP			0x0467	//设置经验
#define  CMD_SEND			0x0091	//向客户端发消息通用命令
#define  CMD_SENDROLLMSG	0x0073	//向客户端发小喇叭滚动消息
#define  PHP_PUSH_CARD		0x0501	//向客户端发送牌型
#define  CMD_SENDSERVER		0x04FF	//发送redis服务器跟新消息


CTcpMsg::CTcpMsg(void)
{
}

#define SEND_PACK_TO_MONEY(sendPack, recvPack) \
    CEvtClientMgr* evtCliMgr = ThreadResource::getInstance().getCliConnMgr();\
    if(NULL == evtCliMgr)\
    {\
        LOGGER(E_LOG_ERROR) << "money connect failed";\
        return false;\
    }\
    CClientSocketEx* connMoneySvr = evtCliMgr->GetMoneySvrConn();\
    if(NULL == connMoneySvr) \
    {\
        LOGGER(E_LOG_ERROR) << "get connectted money server failed";\
        return false;\
    }\
    if(connMoneySvr->SendEx(&packSend,false) == -1)\
    {\
        LOGGER(E_LOG_ERROR) << "money send failed";\
        return false;\
    }\
    if(connMoneySvr->RecvEx(recvPack) == -1)\
    {\
        LOGGER(E_LOG_ERROR) << "money send failed";\
        return false;\
    }

#define SEND_PACK_TO_USER(sendPack, recvPack) \
    CEvtClientMgr* evtCliMgr = ThreadResource::getInstance().getCliConnMgr();\
    if(NULL == evtCliMgr) \
    {\
        LOGGER(E_LOG_ERROR) << "user server connect failed";\
        return false; \
    }\
    CClientSocketEx* connUserSvr = evtCliMgr->GetUserSvrConn();\
    if(NULL == connUserSvr) \
    {\
        LOGGER(E_LOG_ERROR) << "get connectted user server failed";\
        return false; \
    }\
    if(connUserSvr->SendEx(&packSend,false) == -1) \
    {\
        LOGGER(E_LOG_ERROR) << "send msg to user server failed";\
        return false; \
    }\
    if(connUserSvr->RecvEx(recvPack) == -1) \
    {\
        LOGGER(E_LOG_ERROR) << "get user server response msg failed";\
        return false; \
    }\


CTcpMsg::~CTcpMsg(void)
{
}

bool CTcpMsg::getMoney(Json::Value &ret, int32_t uMid)
{
    OutputPacket packSend;
    packSend.Begin(CMD_GET);
    packSend.SetUid(uMid);
    packSend.WriteInt(uMid);
    packSend.End();
    
    InputPacket packRecv;
    SEND_PACK_TO_MONEY(packSend, packRecv);
    short retcode = packRecv.ReadShort();
    std::string retmsg = packRecv.ReadString();
    if (retcode == 0)
    {
        ret["mid"] = packRecv.ReadInt();
        ret["money"] = packRecv.ReadInt64();
        ret["freezemoney"] = packRecv.ReadInt64();
        ret["roll"] = packRecv.ReadInt();
        ret["roll1"] = packRecv.ReadInt();
        ret["coin"] = packRecv.ReadInt();
        ret["exp"] = packRecv.ReadInt();
        return true;
    }
    LOGGER(E_LOG_ERROR) << "get money from moneyserver failed";
    return false;
}

bool CTcpMsg::setMoney(Json::Value& jvRet, int32_t uMid, int64_t nMoney, uint8_t nType /*= 10*/)
{
	OutputPacket packSend;
	packSend.Begin(CMD_SETMONEY);
	packSend.SetUid(uMid);
	packSend.WriteInt(uMid);
	packSend.WriteByte(nType);
	packSend.WriteInt64(nMoney);
	packSend.End();

	if(SendAndGet( jvRet,  packSend )) return true;

	LOGGER(E_LOG_INFO) << "setRoll Error";
	return false;
}

bool CTcpMsg::setNewMoney(const std::string &dealNo, int32_t uMid)
{
    OutputPacket packSend;
    packSend.Begin(CMD_PHP_SET_MOENY);
    packSend.SetUid(uMid);
    packSend.WriteString(dealNo);
    packSend.End();
    
    InputPacket packRecv;
    SEND_PACK_TO_MONEY(packSend, packRecv);
    short retcode = packRecv.ReadShort();
    if (retcode == 0)
    {
        LOGGER(E_LOG_INFO) << "recv msg from money success";
        return true;
    }
    else
    {
        LOGGER(E_LOG_INFO) << "recv msg from money failed, retcode = " << retcode;
        return false;
    }
}

bool CTcpMsg::setBank( Json::Value& jvRet, int32_t uMid, int64_t nMoney, uint8_t nType /*= 10*/ )
{
	OutputPacket packSend;
	packSend.Begin(CMD_SETBANK);
	packSend.SetUid(uMid);
	packSend.WriteInt(uMid);
	packSend.WriteByte(nType);
	packSend.WriteInt64(nMoney);
	packSend.End();

	if(SendAndGet( jvRet,  packSend )) return true;

	LOGGER(E_LOG_INFO) << "setRoll Error";
	return false;
}

bool CTcpMsg::get( Json::Value& jvRet, int32_t uMid)
{
	if (uMid < 1000 || uMid > 100000000)
	{
		return false;
	}
	OutputPacket packSend;
	packSend.Begin(CMD_GET);
	packSend.SetUid(uMid);
	packSend.WriteInt(uMid);
	packSend.End();

	if(SendAndGet( jvRet,  packSend )) return true;

	LOGGER(E_LOG_INFO) << "setRoll Error";
	return false;
}

bool CTcpMsg::setRoll( Json::Value& jvRet, int32_t uMid,int32_t nRoll,int8_t nType /*= 10*/)
{
	if (uMid < 0)
	{
		return false;
	}

	OutputPacket packSend;
	packSend.Begin(CMD_SETROLL);
	packSend.SetUid(uMid);
	packSend.WriteInt(uMid);
	packSend.WriteByte(nType); 
	packSend.WriteInt(nRoll); 
	packSend.End();
	if(SendAndGet( jvRet,  packSend )) return true;
	
	LOGGER(E_LOG_INFO) << "setRoll Error";
	return false;
}

bool CTcpMsg::setRollExp( Json::Value& jvRet, int32_t uMid,int32_t nRoll,int8_t nType /*= 10*/)
{
	if (uMid < 0)
	{
		return false;
	}

	OutputPacket packSend;
	packSend.Begin(CMD_SETROLLEXP);
	packSend.SetUid(uMid);
	packSend.WriteInt(uMid);
	packSend.WriteByte(nType); 
	packSend.WriteInt(nRoll); 
	packSend.End();
	if(SendAndGet( jvRet,  packSend )) return true;

	LOGGER(E_LOG_INFO) << "setRoll Error";
	return false;
}

bool CTcpMsg::sendMsg(int32_t mid, short type, const std::string &msg)
{
    OutputPacket packSend;
    packSend.Begin(CMD_SEND);
    packSend.SetUid(0);
    packSend.WriteInt(mid);
    packSend.WriteShort(type);
    packSend.WriteString(msg);
    packSend.End();
    
    InputPacket packRecv;
    SEND_PACK_TO_USER(packSend, packRecv);
    short retcode = packRecv.ReadShort();
    if (retcode == 0)
    {
        LOGGER(E_LOG_INFO) << "recv msg from user success";
        return true;
    }
    else
    {
        LOGGER(E_LOG_INFO) << "recv msg from user failed";
        return false;
    }
}

bool CTcpMsg::SendAndGet( Json::Value& jvRet, OutputPacket& packSend )
{
	CEvtClientMgr* evtCliMgr = ThreadResource::getInstance().getCliConnMgr();
	if(NULL == evtCliMgr) return false;
	CClientSocketEx* connMoneySvr = evtCliMgr->GetMoneySvrConn();
	if(NULL == connMoneySvr) return false;
	if(connMoneySvr->SendEx(&packSend,false) == -1) return false;

	InputPacket packRecv;
	if(connMoneySvr->RecvEx(packRecv) == -1) return false;

	//判断包的正确性，这个函数没有，先跳过吧

	short nRetCode = packRecv.ReadShort();
	std::string strMsg = packRecv.ReadString();
	if (0 == nRetCode)
	{
		jvRet["mid"] = packRecv.ReadInt();
		jvRet["money"] = packRecv.ReadInt64();
		jvRet["freezemoney"] = packRecv.ReadInt64();
		jvRet["roll"] = packRecv.ReadInt();
		jvRet["roll1"] = packRecv.ReadInt();
		jvRet["coin"] = packRecv.ReadInt();
		jvRet["exp"] = packRecv.ReadInt();
		return true;
	}
	return false;
}
