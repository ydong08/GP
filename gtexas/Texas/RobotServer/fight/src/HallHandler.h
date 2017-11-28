#ifndef __HallHandler_H__
#define __HallHandler_H__

#include <map>
#include "CDLSocketHandler.h"
#include "Packet.h"
#include "DLDecoder.h"

class HallHandler :public CDLSocketHandler 
{
	public:
		HallHandler();
		virtual ~HallHandler();
        int OnConnected() ;
        int OnClose();
        int OnPacketComplete(const char* data, int len);
		
		CDL_Decoder* CreateDecoder()
    	{
			return &DLDecoder::getInstance();
    	}
	public:
		int uid;
	private:
		
};
#endif
