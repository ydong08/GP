#include "CallProcess.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include <string>
using namespace std;

CallProcess::CallProcess()
{
	this->name = "CallProcess";
}

CallProcess::~CallProcess()
{

} 

int CallProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{

	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_BET_CALL, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteInt(player->tid);
	if(player->limitcoin < (player->currMaxCoin - player->betCoinList[player->currRound]))
		requestPacket.WriteInt64(player->limitcoin);
	else
		requestPacket.WriteInt64(player->currMaxCoin - player->betCoinList[player->currRound]);
	requestPacket.End();	
	this->send(clientHandler, &requestPacket);
	printf("Send CallProcess Packet to Server\n");
	printf("Data Send: player->id=[%d]\n", player->id);
	printf("Data Send: player->name=[%s]\n", "robot");
	printf("Data Send: player->id=[%d]\n", player->id);
	printf("Data Send: player->money=[%ld]\n", player->money);
	printf("Data Send: player->currMaxCoin=[%d]\n", player->currMaxCoin);
	printf("Data Send: player->betCoinList=[%d]\n", player->betCoinList[player->currRound]);
	return 0;
}

int CallProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv CallProcess Packet From Server\n");
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
	short currRound = inputPacket->ReadByte();
	int callid = inputPacket->ReadInt();
	int64_t callcoin = inputPacket->ReadInt64();
	int64_t roundCoin = inputPacket->ReadInt64();
	int64_t countCoin = inputPacket->ReadInt64();
	player->currMaxCoin = inputPacket->ReadInt64();
	int nextid = inputPacket->ReadInt();
	int64_t roundbetcoin = inputPacket->ReadInt64();
	player->carrycoin = inputPacket->ReadInt64();
	int optype = inputPacket->ReadShort();
	int64_t limitcoin = inputPacket->ReadInt64();
	int64_t poolcoin = inputPacket->ReadInt64();
	printf("Data Recv: uid=[%d]\n",uid);
	printf("Data Recv: ustatus=[%d]\n",ustatus);
	printf("Data Recv: tid=[%d]\n",tid);
	printf("Data Recv: tstatus=[%d]\n",tstatus);
	printf("Data Recv: nextid=[%d]\n",nextid);
	if(tid != player->tid)
		return EXIT;

	player->betCoinList[currRound] = roundbetcoin;

	player->currRound = currRound;
	player->optype = optype;
	player->limitcoin = limitcoin;
	player->PoolCoin = poolcoin;

	if(uid == nextid)
	{
		player->startBetCoinTimer(uid, Configure::instance()->basebettime + rand()%4);
		//_LOG_DEBUG_("===Call===uid:[%d]===== startBetCoinTimer\n", uid);
	}

	return 0;
}

