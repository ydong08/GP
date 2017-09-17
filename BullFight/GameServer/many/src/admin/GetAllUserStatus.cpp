#include "GetAllUserStatus.h"
#include "ProtocolServerId.h"
#include "Room.h"
#include "Logger.h"
#include "ProcessManager.h"

int GetAllUserStatus::doRequest(CDLSocketHandler* client, InputPacket* inputPacket,Context* pt)
{
	_NOTUSED(pt);	
	short cmd = inputPacket->GetCmdType();
	short subcmd = inputPacket->GetSubCmd();
	_LOG_DEBUG_("==>[GetAllUserStatus] [0x%04x] \n", cmd);
	_LOG_DEBUG_("SubCmd=[%d] \n", subcmd);
	//Room* room = Room::getInstance();
	//for(int i=0; i< room->getMaxUsers() ;++i)
	//{
	//	Player * player = &room->players[i];
		/*time_t t = player->getActiveTime();
		char Time[20]={'\0'};
		strftime(Time,  20, "%Y-%m-%d %H:%M:%S",localtime(&t));
		_LOG_DEBUG_("m_nSeatID=[%d] tid=[%d] uid=[%d] um_nStatus=[%d] lasttime=[%s]\n", player->m_nSeatID, player->tid, player->id, player->m_nStatus,Time);*/
	//}

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

REGISTER_PROCESS(ADMIN_GET_ALL_PLAYER, GetAllUserStatus)