#ifndef __HallHandler_H__
#define __HallHandler_H__

#include <map>
#include "CDLSocketHandler.h"
#include "Packet.h"

#ifdef CRYPT
#include "CryptDecoder.h"
#else
#include "DLDecoder.h"
#endif

typedef void(*onRobotClose)(int uid);

class HallHandler :public CDLSocketHandler
#ifdef CRYPT
	, public CDLSocketHandler::PacketListener
#endif
{
public:
    HallHandler();
    virtual ~HallHandler();
    int OnConnected() ;
    int OnClose();
    int OnPacketComplete(const char* data, int len);
		
    CDL_Decoder* CreateDecoder()
    {
#ifndef CRYPT
        return &DLDecoder::getInstance();
#else
		return &CryptDecoder::getInstance();
#endif
    }
    
    void setOnRobotClose(onRobotClose func) { OnRobotClose = func;}
public:
    int uid;

private:
	int Send(const char * buff, int len);
	bool OnPacketCompleteEx(const char* data, int len);

#ifdef CRYPT
private:
	std::string			m_strAuthKey;
	uint64_t			m_nUID;
#endif

private:
    onRobotClose OnRobotClose;
};
#endif
