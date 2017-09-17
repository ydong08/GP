#include "PushOperateNotify.h"
#include "HallHandler.h"
#include "Logger.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include <string>
using namespace std;

REGISTER_PROCESS(CLIENT_MSG_OPERATE_CARD, PushOperateNotify)

PushOperateNotify::PushOperateNotify()
{
	this->name = "PushOperateNotify";
}

PushOperateNotify::~PushOperateNotify()
{

} 

int PushOperateNotify::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if (player == NULL)
		return 0;

	//设置变量
	player->action_ = WIK_NULL;
	player->m_cbActionCard = 0;

	OutputPacket requestPacket;
	requestPacket.Begin(CLIENT_MSG_OPERATE_CARD, player->m_Uid);
	requestPacket.WriteInt(player->m_Uid);
	requestPacket.WriteByte(player->OperateCode_);
	requestPacket.WriteByte(player->OperateCard_);

	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	return 0;
}

int PushOperateNotify::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv SendCardProc Packet From Server\n");
	printf("Data Recv: retcode=[%d]\n",retcode);
	printf("Data Recv: retmsg=[%s]\n",retmsg.c_str());*/
	if (retcode < 0 || player == NULL)
	{
		return EXIT;
	}
	int uid = inputPacket->ReadInt();

	if (uid != player->m_Uid)
	{
		LOGGER(E_LOG_ERROR) << "PushOperateNotify doResponse error[" << uid << "!=" << player->m_Uid << "]";
		return -1;
	}
	int operate_uid = inputPacket->ReadInt();
	int resume_user_pos = inputPacket->ReadInt();
	int provide_card_data = inputPacket->ReadByte();
	int action = inputPacket->ReadByte();
	
	if ((action != WIK_NULL))
	{
		player->action_ = action;
		player->m_cbActionCard = provide_card_data;
		player->startOutCardTimer(uid, 2 + rand() % 3);
	}

	return 0;
}


