#ifndef __IProcess_H__
#define __IProcess_H__
#include <string>
#include "GameCmd.h"
#include "HallHandler.h"
#include "PlayerManager.h"
#include "Configure.h"
#include "Player.h"
#include "Logger.h"

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
		IProcess(){};
		virtual ~IProcess() {};
 
		virtual int doRequest(CDLSocketHandler* socketHandler, InputPacket* inputPacket, Context* pt) = 0;
		virtual int doResponse(CDLSocketHandler* socketHandler, InputPacket* inputPacket, Context* pt)  = 0; 

	public:
		std::string name;
		short cmd;
	
	protected:	
		int send(CDLSocketHandler* clientHandler, OutputPacket* outPack, bool isEncrypt=true);
	
};
#endif 
