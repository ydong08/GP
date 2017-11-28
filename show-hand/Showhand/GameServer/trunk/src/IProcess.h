#ifndef __IProcess_H__
#define __IProcess_H__
#include <string>
#include <map>
#include <json/json.h>
#include "CDLSocketHandler.h"
#include "ClientHandler.h"
#include "Packet.h"
#include "GameCmd.h"
#include "Table.h"

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
	//¾²Ì¬º¯Êý
	public:
		static int GameStart( Table* table);
		static int GameOver( Table* table,Player* winner, bool isthrowwin = false);
		static int sendRoundCard(Player* player, Table* table, Player* firstBetPlayer, Player* prewinner, short currRound);
		static int sendGameStartInfo(Player* player, Table* table);
		static int sendGameOverinfo(Player* player, Table* table, Player* winner);
		static int serverPushLeaveInfo(Table* table, Player* leavePlayer, short retno = 1);
		static int serverWarnPlayerKick(Table* table, Player* warnner, short timeCount);

		static int sendToRobotCard(Player* player, Table* table);

		static int updateDB_UserCoin(Player* player, Table* table, short nwin, short nlose, short nrunaway);
		static int updateDB_UserCoin(LeaverInfo* player, Table* table, short nwin, short nlose, short nrunaway);
		static int updateDB_UserCoin(Player* player, Table* table);
		static int updateDB_UserVoucher(Player* player, Table* table, int nroll, bool isboardtask = false);
		static int UpdateDBActiveTime(Player* player);
        static int NotifyPlayerNetStatus(Table* table, int uid, int signal, int type);

	protected:
		int sendErrorMsg(ClientHandler* clientHandler, int cmd, int uid, short errcode, const char* errmsg, unsigned short seq = 0);
		int send(CDLSocketHandler* clientHandler, OutputPacket* outPack, bool isEncrypt=true);
	
};

#endif 
