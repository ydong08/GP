#include "AllinProcess.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include <string>
using namespace std;

AllinProcess::AllinProcess()
{
	this->name = "AllinProcess";
}

AllinProcess::~AllinProcess()
{

} 

int AllinProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{

	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_ALL_IN, player->id);
	requestPacket.WriteInt(player->id);
	requestPacket.WriteInt(player->tid);
	requestPacket.End();	
	this->send(clientHandler, &requestPacket);
	return 0;
}

int AllinProcess::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv AllinProcess Packet From Server\n");
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
	int allinid = inputPacket->ReadInt();
	int64_t allincoin = inputPacket->ReadInt64();
	int64_t roundCoin = inputPacket->ReadInt64();
	int64_t countCoin = inputPacket->ReadInt64();
	player->currMaxCoin = inputPacket->ReadInt64();
	int nextid = inputPacket->ReadInt();
	int64_t roundbetcoin = inputPacket->ReadInt64();
	player->carrycoin = inputPacket->ReadInt64();
	int optype = inputPacket->ReadShort();
	int64_t limitcoin = inputPacket->ReadInt64();
	int64_t poolcoin = inputPacket->ReadInt64();
	/*printf("Data Recv: uid=[%d]\n",uid);
	printf("Data Recv: ustatus=[%d]\n",ustatus);
	printf("Data Recv: tid=[%d]\n",tid);
	printf("Data Recv: tstatus=[%d]\n",tstatus);
	printf("Data Recv: comeid=[%d]\n",comeid);
	printf("Data Recv: seatid=[%d]\n",seatid);
	printf("Data Recv: playerindex=[%d]\n",playerindex);
	printf("Data Recv: num=[%d]\n\n",num);*/
	if(tid != player->tid)
		return EXIT;

	player->currRound = currRound;

	player->betCoinList[currRound] = roundbetcoin;
	player->PoolCoin = poolcoin;

	player->optype = optype;
	player->limitcoin = limitcoin;

	if(uid == nextid)
	{
		player->startBetCoinTimer(uid, Configure::instance()->basebettime + rand()%5);
		//_LOG_DEBUG_("==Allin====uid:[%d]===== startBetCoinTimer\n", uid);
	}

	return 0;
}

