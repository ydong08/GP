#include "BetTimeOutProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
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
	short curround= inputPacket->ReadByte();
	int timeoutuid = inputPacket->ReadInt();
	player->currMaxCoin = inputPacket->ReadInt64();
	int nextid = inputPacket->ReadInt();
	int64_t roundbetcoin = inputPacket->ReadInt64();
	player->carrycoin = inputPacket->ReadInt64();
	int optype = inputPacket->ReadShort();
	int64_t limitcoin = inputPacket->ReadInt64();
	int64_t poolcoin = inputPacket->ReadInt64();

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

	player->betCoinList[curround] = roundbetcoin;
	player->PoolCoin = poolcoin;
	player->optype = optype;
	player->limitcoin = limitcoin;

	if(uid == nextid)
	{
		player->startBetCoinTimer(uid, Configure::instance()->basebettime + rand()%5);
		//_LOG_DEBUG_("===BetTimeOut===uid:[%d]===== startBetCoinTimer\n", uid);
	}
	return 0;
}

