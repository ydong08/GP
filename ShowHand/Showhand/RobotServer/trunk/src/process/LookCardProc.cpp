#include "LookCardProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Options.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include <string>
using namespace std;

LookCardProc::LookCardProc()
{
	this->name = "LookCardProc";
}

LookCardProc::~LookCardProc()
{

} 

int LookCardProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{

	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_LOOK_CARD, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteInt(player->tid);
	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	/*printf("Send LookCardProc Packet to Server\n");
	printf("Data Send: player->id=[%d]\n", player->id);
	printf("Data Send: player->name=[%s]\n", "robot");
	printf("Data Send: player->id=[%d]\n", player->id);
	printf("Data Send: player->money=[%ld]\n", player->money);
	printf("Data Send: player->clevel=[%d]\n", player->clevel);*/
	return 0;
}

int LookCardProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv LookCardProc Packet From Server\n");
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
	short currRound = inputPacket->ReadShort();
	int lookid = inputPacket->ReadInt();
	int nextid = inputPacket->ReadInt();
	int optype = inputPacket->ReadShort();
	int64_t limitcoin = inputPacket->ReadInt64();
	int64_t differcoin = inputPacket->ReadInt64();
	/*printf("Data Recv: uid=[%d]\n",uid);
	printf("Data Recv: ustatus=[%d]\n",ustatus);
	printf("Data Recv: tid=[%d]\n",tid);
	printf("Data Recv: tstatus=[%d]\n",tstatus);
	printf("Data Recv: currRound=[%d]\n",currRound);
	printf("Data Recv: lookid=[%d]\n",lookid);
	printf("Data Recv: nextid=[%d]\n",nextid);
	printf("Data Recv: optype=[%d]\n\n",optype);*/
	if(tid != player->tid)
		return EXIT;

	if(uid == nextid)
	{
		player->optype = optype;
		player->limitcoin = limitcoin;
		player->diffcoin = differcoin;
	}
	else
	{
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			if(player->player_array[i].id == nextid)
			{
				player->player_array[i].optype = optype;
				player->player_array[i].limitcoin = limitcoin;
				player->player_array[i].diffcoin = differcoin;
				break;
			}
		}
	}
	if(uid == nextid)
	{
		player->startBetCoinTimer(uid, Options::instance()->basebettime + rand()%5);
		//_LOG_DEBUG_("===Look===uid:[%d]===== startBetCoinTimer\n", uid);
		LOGGER(E_LOG_DEBUG)<< "===Look===uid:["<<uid<<"]===== startBetCoinTimer";

	}
	return 0;
}

