#include "KickOutPlayerProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "Configure.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include <string>
using namespace std;

KickOutPlayerProc::KickOutPlayerProc()
{
	this->name = "KickOutPlayerProc";
}

KickOutPlayerProc::~KickOutPlayerProc()
{

} 

int KickOutPlayerProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int KickOutPlayerProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv KickOutPlayerProc Packet From Server\n");
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
		player->startBetCoinTimer(uid, Configure::instance()->basebettime + rand()%6);
		//_LOG_DEBUG_("===KickOut===uid:[%d]===== startBetCoinTimer\n", uid);
		return 0;
	}

	for(int i = 0; i < GAME_PLAYER; ++i)
	{
		if(player->player_array[i].id != 0 && player->player_array[i].source != 30)
		{
			return 0;
		}
	}
	return 0;
	//return EXIT;
}


