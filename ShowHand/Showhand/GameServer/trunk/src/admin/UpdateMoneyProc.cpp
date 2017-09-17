#include <string>

#include "UpdateMoneyProc.h"
#include "../GameCmd.h"
#include "../model/Room.h"
#include "Logger.h"
using namespace std;

int UpdateMoneyProc::doRequest(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
	return 0;
}

int UpdateMoneyProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
	short cmd = inputPacket->GetCmdType();

	_LOG_DEBUG_("==>[UpdateMoneyProc] [0x%04x] \n", cmd);
	
	int uid = inputPacket->ReadInt();
	int64_t money = inputPacket->ReadInt64();
	//int64_t safemoney = inputPacket->ReadInt64();
    
    _LOG_DEBUG_("[DATA Parse] uid=[%d] money=[%ld] \n", uid, money);
	Room* room = Room::getInstance();

	for(int i=0; i< room->getMaxUsers() ;++i)
	{
		Player * player = &room->players[i];
		if(player && player->id == uid)
		{
			player->money += money;
			//player->voucher = roll;
			_LOG_INFO_("Found uid=[%d] update money[%d]\n", uid, money);
			break;
		}
	}	
	return 0;	
}  

int UpdateMoneyProc::sendErrorMsg(ClientHandler* clientHandler, short errcode, char* errmsg) 
{
	OutputPacket response;
	response.Begin(ADMIN_GET_FREE_PLAYER);
	response.WriteShort(errcode);
	response.WriteString(errmsg);
	response.End();
	_LOG_DEBUG_("[DATA Reponse] erron=[%d] \n", errcode);
	_LOG_DEBUG_("[DATA Reponse] errmsg=[%s] \n", errmsg);
	this->send(clientHandler,&response);
	return 0;
}

 
