#ifndef __HallHandler_H__
#define __HallHandler_H__

#include <map>
#include "CDLSocketHandler.h"
#include "Packet.h"
#include "DLDecoder.h"

#ifdef CRYPT
#include "CryptDecoder.h"
#endif

class HallHandler :public CDLSocketHandler, public CDLSocketHandler::PacketListener
{
public:
	HallHandler();
	virtual ~HallHandler();
    int OnConnected() ;
    int OnClose();
	
	int Send(OutputPacket* outPacket, bool isEncrypt);

private:
	bool OnPacketCompleteEx(const char* data, int len);
    int OnPacketComplete(const char* data, int len);
		
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
