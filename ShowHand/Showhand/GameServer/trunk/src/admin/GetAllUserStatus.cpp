#include <string>

#include "GetAllUserStatus.h"
#include "../GameCmd.h"
#include "../model/Room.h"
#include "Logger.h"
using namespace std;

int GetAllUserStatus::doRequest(CDLSocketHandler* client, InputPacket* inputPacket,Context* pt)
{
	_NOTUSED(pt);	
	short cmd = inputPacket->GetCmdType();
	short subcmd = inputPacket->GetSubCmd();
	_LOG_DEBUG_("==>[GetAllUserStatus] [0x%04x] \n", cmd);
	_LOG_DEBUG_("SubCmd=[%d] \n", subcmd);
	Room* room = Room::getInstance();
	for(int i=0; i< room->getMaxUsers() ;++i)
	{
		Player * player = &room->players[i];
		time_t t = player->getActiveTime();
		char Time[20]={'\0'};
		strftime(Time,  20, "%Y-%m-%d %H:%M:%S",localtime(&t));
		_LOG_DEBUG_("seat_id=[%d] tid=[%d] uid=[%d] ustatus=[%d] lasttime=[%s]\n", player->seat_id, player->tid, player->id, player->status,Time);
	}

	OutputPacket response;
	response.Begin(cmd);
	response.WriteShort(0);
	response.WriteString("Ok");
	response.End();
	_LOG_DEBUG_("[DATA Reponse] erron=[%d] \n", 0);
	_LOG_DEBUG_("[DATA Reponse] errmsg=[%s] \n", "Ok");
	this->send(client, &response);
	return 0;
}

int GetAllUserStatus::doResponse(CDLSocketHandler* client, InputPacket* inputPacket,Context* pt)
{
	_NOTUSED(client);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);	
	return 0;	
}  

