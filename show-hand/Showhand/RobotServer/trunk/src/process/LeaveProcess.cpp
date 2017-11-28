#include "LeaveProcess.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Options.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include <string>
using namespace std;

LeaveProcess::LeaveProcess()
{
	this->name = "LeaveProcess";
}

LeaveProcess::~LeaveProcess()
{

} 

int LeaveProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{

	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_LEAVE, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteInt(player->tid);
	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	/*printf("Send LeaveProcess Packet to Server\n");
	printf("Data Send: player->id=[%d]\n", player->id);
	printf("Data Send: player->id=[%d]\n", player->id);*/
	return 0;
}

int LeaveProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv LeaveProcess Packet From Server\n");
	printf("Data Recv: retcode=[%d]\n",retcode);
	printf("Data Recv: retmsg=[%s]\n",retmsg.c_str());*/
	if(retcode < 0 || player == NULL)
	{
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	int leaver = inputPacket->ReadInt();
	int nextid = inputPacket->ReadInt();
	int optype = inputPacket->ReadShort();
	int64_t limitcoin = inputPacket->ReadInt64();
	int64_t differcoin = inputPacket->ReadInt64();
	if(uid == leaver)
		return EXIT;
	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(player->player_array[i].id == leaver)
		{
			player->player_array[i].id = 0;
			player->player_array[i].hascard = false;
			break;
		}
	}
	if(uid == nextid)
	{
		player->startBetCoinTimer(uid, Options::instance()->basebettime + rand()%4);
		LOGGER(E_LOG_DEBUG)<< "===Leave===uid:["<<uid<<"]===== startBetCoinTimer";
		//_LOG_DEBUG_("===Leave===uid:[%d]===== startBetCoinTimer\n", uid);
		return 0;
	}

	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(player->player_array[i].id != 0 && player->player_array[i].source != 30)
		{
			LOGGER(E_LOG_DEBUG)<< "+++++++++++++player->player_array[i].id:"<<player->player_array[i].id<<"]";
			//_LOG_DEBUG_("+++++++++++++player->player_array[i].id:%d\n",player->player_array[i].id);
			return 0;
		}
	}
	return 0;
	//return EXIT;
}

