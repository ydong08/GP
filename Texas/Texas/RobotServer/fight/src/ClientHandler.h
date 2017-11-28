#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

#include "CDLSocketServer.h"
#include "CDLSocketHandler.h"
#include "DLDecoder.h"
#include "Packet.h"
#include <string>
using namespace std;

#ifdef MAX_CALL_BACK
#    undef MAX_CALL_BACK
#    define MAX_CALL_BACK 100
#else
#    define MAX_CALL_BACK 100
#endif
 
class ClientHandler: public CDLSocketHandler					
{
public:
	ClientHandler();
	virtual ~ClientHandler() ;
   
    string m_addrremote; //远端地址
	int m_nPort; //端口

	int addCallbackSeq(unsigned short seq);
	void deleteCallbackSeq(unsigned short seq);
	void clearCallbackSeq();
    int Send(OutputPacket* packet, bool encode=true)
	{
		if(encode)
			packet->EncryptBuffer();
        return CDLSocketHandler::Send((const char*)packet->packet_buf(), packet->packet_size()) >= 0 ? 0 : -1;
	}
	int close(){return 0;};
	int OnConnected(void); 
	int OnClose();
private:
	int OnPacketComplete(const char* data,int len);
	CDL_Decoder* CreateDecoder()
    {
		return &DLDecoder::getInstance();
    }

    void GetRemoteAddr(void);
	unsigned short callback_count;
	unsigned short callback_seq[MAX_CALL_BACK];
	InputPacket pPacket;

};

//实例化一个Server
typedef SocketServer<ClientHandler> RobotServer;

#endif

