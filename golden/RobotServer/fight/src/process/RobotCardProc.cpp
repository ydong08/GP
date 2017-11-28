#include "RobotCardProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Despatch.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include <string>
using namespace std;

RobotCardProc::RobotCardProc()
{
	this->name = "RobotCardProc";
}

RobotCardProc::~RobotCardProc()
{

} 

int RobotCardProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int RobotCardProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
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
	short count = inputPacket->ReadByte();
	ULOGGER(E_LOG_DEBUG, uid) << "user status:" << ustatus << " tid:" << tid << " table status:" << tstatus << " count:" << count;
	player->status = tstatus;
	if (tid != player->tid) {
		ULOGGER(E_LOG_INFO, uid) << "[robot exit] tid=" << tid << " not equal player->tid=" << player->tid;
		return EXIT;
	}
	for(int i = 0; i < count; i++)
	{
		int anthorid = inputPacket->ReadInt();
		ULOGGER(E_LOG_DEBUG, uid) << "anthorid=" << anthorid;
		for(int j = 0; j < GAME_PLAYER; ++j)
		{
			if(player->player_array[j].id == anthorid)
			{
				for(int m = 0; m < 3; m++)
				{
					player->player_array[j].Knowcard_array[m] = inputPacket->ReadByte();
					ULOGGER(E_LOG_DEBUG, uid) << "m=" << m << " card=" << TOHEX(player->player_array[j].Knowcard_array[m]);
				}
				break;
			}
		}
		if(player->id == anthorid)
		{
			for(int m = 0; m < 3; m++)
			{
				player->Knowcard_array[m] = inputPacket->ReadByte();
				ULOGGER(E_LOG_DEBUG, uid) << "robot self card=" << TOHEX(player->Knowcard_array[m]);
			}
		}
	}
	return 0;
}

REGISTER_PROCESS(SERVER_MSG_ROBOT, RobotCardProc)