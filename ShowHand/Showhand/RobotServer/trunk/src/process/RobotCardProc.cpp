#include "RobotCardProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Options.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
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
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	//_LOG_DEBUG_("Recv RobotCardProc Packet From Server\n");
	/*printf("Recv RobotCardProc Packet From Server\n");
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
	short count = inputPacket->ReadShort();
	printf("Data Recv: uid=[%d]\n",uid);
	printf("Data Recv: ustatus=[%d]\n",ustatus);
	printf("Data Recv: tid=[%d]\n",tid);
	printf("Data Recv: tstatus=[%d]\n",tstatus);
	printf("Data Recv: count=[%d]\n",count);
	player->status = tstatus;
	if(tid != player->tid)
		return EXIT;
	for(int i = 0; i < count; i++)
	{
		int anthorid = inputPacket->ReadInt();
		//_LOG_DEBUG_("Data Recv: anthorid=[%d]\n",anthorid);
		for(int j = 0; j < GAME_PLAYER; ++j)
		{
			if(player->player_array[j].id == anthorid)
			{
				for(int m = 0; m < 5; m++)
					player->player_array[j].Knowcard_array[m] = inputPacket->ReadByte();
				break;
			}
		}
		if(player->id == anthorid)
		{
			for(int j = 0; j < 5; j++)
			{
				player->Knowcard_array[j] = inputPacket->ReadByte();
				//_LOG_DEBUG_("card[0x%02x]\n", player->Knowcard_array[j]);
			}
		}
	}
	
	return 0;
}


