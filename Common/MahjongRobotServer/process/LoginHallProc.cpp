#include "LoginHallProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "GameCmd.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include <string>
using namespace std;

REGISTER_PROCESS(HALL_USER_LOGIN, LoginHallProc)

LoginHallProc::LoginHallProc()
{
	this->name = "LoginHallProc";
}

LoginHallProc::~LoginHallProc()
{

} 

int LoginHallProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;
	OutputPacket requestPacket;
	requestPacket.Begin(HALL_USER_LOGIN, player->m_Uid);
	requestPacket.WriteInt(player->m_Uid);
	requestPacket.WriteString(player->name);
	requestPacket.WriteInt64(player->money);
	requestPacket.WriteString("{\"robot\":2}\n");
	requestPacket.End();
	this->send(clientHandler, &requestPacket);
	/*printf("Send LoginHallProc Packet to Server\n");
	printf("Data Send: player->id=[%d]\n", player->id);
	printf("Data Send: player->name=[%s]\n", "robot");
	printf("Data Send: player->money=[%ld]\n", player->money);*/
	return 0;
	
}

int LoginHallProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	_NOTUSED(clientHandler);
	/*int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	printf("Recv LoginHallProc Packet From Server\n");
	printf("Data Recv: retcode=[%d]\n",retcode);
	printf("Data Recv: retmsg=[%s]\n",retmsg.c_str());
	if(retcode != 0 && retcode != 1)
	{
		return EXIT;
	}
	int uid = inputPacket->ReadInt();
	if(retcode == 1)
	{
		int tid = inputPacket->ReadInt();
	}*/
	return CLIENT_MSG_LOGINCOMING;
}

