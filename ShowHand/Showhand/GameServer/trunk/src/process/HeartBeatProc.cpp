#include "HeartBeatProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "UdpManager.h"
#include "LogServerConnect.h"

HeartBeatProc::HeartBeatProc()
{
	this->name = "HeartBeatProc";
}

HeartBeatProc::~HeartBeatProc()
{

} 

int HeartBeatProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	int cmd = pPacket->GetCmdType() ;

	int uid = pPacket->ReadInt();
	int stid = pPacket->ReadInt();
	short tid = stid & 0x0000FFFF;
	_LOG_DEBUG_("[Recv Data] cmd=[0x%04x] uid=[%d] realtid=[%d]\n", cmd, uid ,tid);

	Room* room = Room::getInstance();
    Table *table = NULL;
	if(tid >= 0)
		table = room->getTable(tid);

	if(table==NULL)
	{
		_LOG_ERROR_("HeartProcess:[Cann't get Table] tid=[%d]\n",tid);
		return 0;
	}

	Player* player = table->getPlayer(uid);
	if(player==NULL)
	{
		_LOG_ERROR_("HeartProcess:[Cann't get player,table->getPlayer(uid)=NULL] uid[%d] source[%d]\n",uid,pPacket->GetSource());
		return 0;
	}

	Json::Value data;
	data["errcode"] = 0;
	_UDP_REPORT_(player->id, cmd,"%s",data.toStyledString().c_str());

	player->setActiveTime(time(NULL));
	
	return 0;
}

int HeartBeatProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	_NOTUSED(inputPacket);
	_NOTUSED(pt);
	return 0;
}
