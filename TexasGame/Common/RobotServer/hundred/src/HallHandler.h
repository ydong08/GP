#ifndef __HallHandler_H__
#define __HallHandler_H__

#include <map>
#include "CDLSocketHandler.h"
#include "Packet.h"
#include "DLDecoder.h"

#ifdef CRYPT
#include "CryptDecoder.h"
#endif

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
	bool OnPacketCompleteEx(const char* data, int len);
		
	CDL_Decoder* CreateDecoder()
    {
#ifndef CRYPT
		return &DLDecoder::getInstance();
#else
		return &CryptDecoder::getInstance();
#endif
    }
public:
	int uid;
#ifdef CRYPT
private:
	std::string			m_strAuthKey;
#endif
};
#endif
