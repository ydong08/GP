#include "CheckProcess.h"
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

CheckProcess::CheckProcess()
{
	this->name = "CheckProcess";
}

CheckProcess::~CheckProcess()
{

} 

int CheckProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance().getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_CHECK_CARD, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteInt(player->tid);
	requestPacket.End();	
	this->send(clientHandler, &requestPacket);
	ULOGGER(E_LOG_DEBUG, player->id) << "name=" << player->name << "money=" << player->money << " currmax=" << player->currmax;
	return 0;
}

int CheckProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance().getPlayer(hallHandler->uid);
	int retcode   = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	if (retcode < 0 || player == NULL) {
		if(retcode == -1)
			return 0;
		ULOGGER(E_LOG_INFO, hallHandler->uid) << "[robot exit]";
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	player->currRound = inputPacket->ReadByte();
	int checkid = inputPacket->ReadInt();
	if(checkid == player->id) {
		player->card_array[0] = inputPacket->ReadByte();
		player->card_array[1] = inputPacket->ReadByte();
		player->card_array[2] = inputPacket->ReadByte();
		player->CardType = inputPacket->ReadByte();
		BYTE bRobotCardArray[3];
		memcpy(bRobotCardArray,player->card_array,sizeof(player->card_array));
		player->m_GameLogic.SortCardList(bRobotCardArray, 3);
		player->maxValue = bRobotCardArray[0];
		player->hascard = 2;
	} else {		
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			if(player->player_array[i].id == checkid) {
				player->player_array[i].hascard = 2;
				ULOGGER(E_LOG_INFO, player->id) << "checkid=" << checkid  << " player_array.id" << player->player_array[i].id  << " hascard=" << player->player_array[i].hascard;
				break;
			}
		}

		if (player->hascard == 1) {
			player->startCheckTimer(uid, ROBOT_BASE_CHECK_TIMEOUT + rand() % ROBOT_RAND_CHECK_TIMEOUT);
			ULOGGER(E_LOG_INFO, player->id) << "checkid=" << checkid << " hascard=" << player->hascard;
		}
	}
	ULOGGER(E_LOG_INFO, uid) << "ustatus=" << ustatus << " tid=" << tid << " checkid=" << checkid;
	if (tid != player->tid) {
		ULOGGER(E_LOG_INFO, player->id) << "[robot exit]";
		return EXIT;
	}
	return 0;
}

REGISTER_PROCESS(CLIENT_MSG_CHECK_CARD, CheckProcess);