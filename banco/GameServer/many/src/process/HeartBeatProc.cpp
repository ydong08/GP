#include "HeartBeatProc.h"
#include "Logger.h"
#include "HallManager.h"
#include "Room.h"
#include "ProtocolClientId.h"
#include "json/json.h"
#include "ProcessManager.h"

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

	Room* room = Room::getInstance();
    Table *table = NULL;
	if(tid >= 0)
		table = room->getTable();

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
	player->setActiveTime(time(NULL));
	
	return 0;
}

REGISTER_PROCESS(USER_HEART_BEAT, HeartBeatProc);
