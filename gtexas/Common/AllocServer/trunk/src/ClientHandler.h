#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

#include "CDLSocketServer.h"
#include "CDLSocketHandler.h"
#include "DLDecoder.h"
#include "Packet.h"

#define NONSRV_HANDLE 0
#define HALLSRV_HANDLE 1
#define GMSRV_HANDLE 2

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
 
	int close(){return 0;};
	
	time_t keepalivetime;
	inline void setID(int id){this->id = id;};
	inline int getID(){return this->id ;};
	inline void setHandleType(int type){this->type = type;};
	inline int getHandleType(){return this->type ;};
	int OnConnected(void); 
	int OnClose();
private:

	int OnPacketComplete(const char* data,int len);
	CDL_Decoder* CreateDecoder()
    {
		return &DLDecoder::getInstance();
    }

	void GetRemoteAddr(void);

	/*  向UserServer注销所在服务的用户 */
	/*  GameServer关停时可用 */
	void logOutAllUsers();

	unsigned short callback_count;
	unsigned short callback_seq[MAX_CALL_BACK];
	InputPacket pPacket;
	int id;
	int type;

};

//实例化一个Server
typedef SocketServer<ClientHandler> AllocServer;

#endif

