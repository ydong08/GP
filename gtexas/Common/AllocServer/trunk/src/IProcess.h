#ifndef __IProcess_H__
#define __IProcess_H__
#include <string>
#include <map>
#include "CDLSocketHandler.h"
#include "Packet.h"
#include "GameCmd.h"
#include "ClientHandler.h"
#include <json/json.h>

#define _NOTUSED(V) ((void) V)

class IProcess;


typedef struct Context
{
	unsigned int seq;
	char status;
	PacketHeader msgHeader;
	IProcess* process;
	CDLSocketHandler* client;
	void * data;
}Context;

class IProcess
{
	public:
		IProcess(){ };
		virtual ~IProcess() {};
		
		virtual int doRequest(CDLSocketHandler* socketHandler, InputPacket* inputPacket, Context* pt) = 0;
		virtual int doResponse(CDLSocketHandler* socketHandler, InputPacket* inputPacket, Context* pt )  = 0; 

	public:
		std::string name;
		short cmd;
	
	protected:
		int sendErrorMsg(ClientHandler* clientHandler, int cmd, int uid, short errcode, const char* errmsg, unsigned short seq = 0);
		int send(CDLSocketHandler* clientHandler, OutputPacket* outPack, bool isEncrypt=true);
	
};


#endif 
