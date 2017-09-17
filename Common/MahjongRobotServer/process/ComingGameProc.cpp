#include "ComingGameProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "ProcessManager.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "Protocol.h"
#include <string>
using namespace std;

REGISTER_PROCESS(CLIENT_MSG_LOGINCOMING, ComingGameProc)

ComingGameProc::ComingGameProc()
{
	this->name = "ComingGameProc";
}

ComingGameProc::~ComingGameProc()
{

} 

int ComingGameProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;

	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_LOGINCOMING, player->m_Uid);
	requestPacket.WriteInt(player->m_Uid);
	requestPacket.WriteString(player->name);
	requestPacket.WriteInt(player->m_TableId);
	requestPacket.WriteInt64(player->money);
	requestPacket.WriteShort(player->clevel);
	requestPacket.WriteString(player->json);
	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	return 0;
}

int ComingGameProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	
	if (retcode < 0 || player == NULL)
		return EXIT;
	
	int uid = inputPacket->ReadInt();
	short ustatus = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	int tempComid = inputPacket->ReadInt();
	int tempSeatID = inputPacket->ReadShort();
	int basechip = inputPacket->ReadByte();
	int totalmul = inputPacket->ReadInt();
	int tmp3 = inputPacket->ReadShort();
	
	if (tid != player->m_TableId)
		return EXIT;

	player->tstatus = tstatus;
	if (uid == tempComid)
	{
		player->status = ustatus;
		player->m_UserIndex = tempSeatID;
		player->m_TableIndex = tempSeatID;
	}

	player->m_AllUid[player->m_TableIndex] = uid;
	BYTE tempCounter = inputPacket->ReadByte();
	for (int i = 0; i < tempCounter; i++)
	{
		int temptempUid = inputPacket->ReadInt();
		std::string tempUserName = inputPacket->ReadString();
		short temptempUserState = inputPacket->ReadShort();
		BYTE seat_id = inputPacket->ReadByte();
		int64_t tempMoney = inputPacket->ReadInt64();

		if (GAME_TYPE == E_GUOBIAO_MAHJONG_GAME_ID)
		{
			inputPacket->ReadInt();
			inputPacket->ReadInt();
		}

		std::string tempUserJson = inputPacket->ReadString();
		player->m_AllUid[seat_id] = temptempUid;

		if (GAME_TYPE == E_ZHENGZHOU_MAHJONG_GAME_ID)
		{
			inputPacket->ReadInt64();
			inputPacket->ReadInt64();
			inputPacket->ReadInt64();
			inputPacket->ReadInt();
			inputPacket->ReadInt();
			inputPacket->ReadInt();
			inputPacket->ReadInt();
		}
	}

	int ready_time = inputPacket->ReadInt();
	if (uid == tempComid)
	{
		player->startHeartTimer(uid, 15);
		if (STATUS_PLAYER_COMING == player->status)
			return CLIENT_MSG_START_GAME;
		else
			return CLIENT_MSG_TABLEDET;
	}

	return 0;
}

