#include "PushForbidProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "RobotDefine.h"
#include <string>
using namespace std;

PushForbidProc::PushForbidProc()
{
	this->name = "PushForbidProc";
}

PushForbidProc::~PushForbidProc()
{

} 

int PushForbidProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int PushForbidProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = dynamic_cast<HallHandler*>(clientHandler);
	Player* player = PlayerManager::getInstance().getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if(retcode < 0 || player == NULL)
	{
		ULOGGER(E_LOG_WARNING, hallHandler->uid) << "[robotexit] retcode=" << retcode << " retmsg=" << retmsg;
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	ULOGGER(E_LOG_DEBUG, uid) << "uid=" << uid << " ustatus=" << ustatus << " tid=" << tid << " tstatus=" << tstatus;
	short count = inputPacket->ReadByte();
	for(int i = 0; i < count; ++i)
	{
		int anthorid = inputPacket->ReadInt();
		short forbidround = inputPacket->ReadByte();
		for(int j = 0; j < GAME_PLAYER; ++j)
		{
			if(player->player_array[j].id == anthorid)
			{
				player->player_array[j].forbidround = forbidround;
				break;
			}
		}
	}

	if (tid != player->tid) {
		ULOGGER(E_LOG_WARNING, uid) << "[robotexit] this robot tid:" << player->tid << " != tid:" << tid;
		return EXIT;
	}
	return 0;
}

REGISTER_PROCESS(PUSH_MSG_FORBID, PushForbidProc)