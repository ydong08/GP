#include "PushGameStartProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include <string>
using namespace std;

PushGameStartProc::PushGameStartProc()
{
	this->name = "PushGameStartProc";
}

PushGameStartProc::~PushGameStartProc()
{

} 

int PushGameStartProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int PushGameStartProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv PushGameStartProc Packet From Server\n");
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
	int64_t ante = inputPacket->ReadInt64();
	int64_t gamelimit= inputPacket->ReadInt64();
	player->tax = inputPacket->ReadInt64();
	inputPacket->ReadInt64();
	inputPacket->ReadByte();
	inputPacket->ReadInt();
	inputPacket->ReadInt64();
	inputPacket->ReadInt();
	inputPacket->ReadInt64();
	inputPacket->ReadByte(); 
	short num = inputPacket->ReadByte(); 
	printf("Data Recv: ante=[%ld]\n",ante);
	/*printf("Data Recv: ustatus=[%d]\n",ustatus);
	printf("Data Recv: tid=[%d]\n",tid);
	printf("Data Recv: tstatus=[%d]\n",tstatus);
	printf("Data Recv: startid=[%d]\n",startid);
	printf("Data Recv: startstatus=[%d]\n",startstatus);
	printf("Data Recv: carrycoin=[%ld]\n",carrycoin);*/
	if(tid != player->tid)
		return EXIT;
	player->status = tstatus;
	player->ante = ante;
	player->PoolCoin = ante + ante/2;
	player->currMaxCoin = ante;

	player->status = ustatus;
	
	for(int i = 0; i < num; i++)
	{
		int anthorid = inputPacket->ReadInt();
		short status = inputPacket->ReadShort();
		short playeridx = inputPacket->ReadByte();
		int64_t carrycoin = inputPacket->ReadInt64();
		for(int j = 0; j < GAME_PLAYER; ++j)
		{
			if(player->player_array[j].id == anthorid)
			{
				player->player_array[j].carrycoin = carrycoin;
				break;
			}
		}
		if(player->id == anthorid)
			player->carrycoin = carrycoin;
	}
	return 0;
}


