#include "BetTimeOutProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Options.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include <string>
using namespace std;

BetTimeOutProc::BetTimeOutProc()
{
	this->name = "BetTimeOutProc";
}

BetTimeOutProc::~BetTimeOutProc()
{

} 

int BetTimeOutProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int BetTimeOutProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv BetTimeOutProc Packet From Server\n");
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
	short curround= inputPacket->ReadShort();
	int timeoutuid = inputPacket->ReadInt();
	int nextid = inputPacket->ReadInt();
	int optype = inputPacket->ReadShort();
	int64_t limitcoin = inputPacket->ReadInt64();
	int64_t differcoin = inputPacket->ReadInt64();

	if(timeoutuid == uid)	
		player->hascard = false;
	else
	{
		for(int i = 0; i < GAME_PLAYER; ++i)
		{
			if(player->player_array[i].id == timeoutuid)
			{	
				player->player_array[i].hascard = false;
				break;
			}
		}
	}

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
			}
		}
	}
	if(uid == nextid)
	{
		player->startBetCoinTimer(uid, Options::instance()->basebettime + rand()%5);
	//	_LOG_DEBUG_("===BetTimeOut===uid:[%d]===== startBetCoinTimer\n", uid);
		LOGGER(E_LOG_DEBUG)<< "===BetTimeOut===uid:["<<uid<<"]===== startBetCoinTimer";

	}
	return 0;
}

