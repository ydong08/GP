//---------------------------------------------------------------------------
#include "LoginProcess.h"
#include <time.h>
#include <stdlib.h>
#include "HallHandler.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"

REGISTER_PROCESS(HALL_USER_LOGIN, LoginProcess)

int LoginProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	OutputPacket packet;
	packet.Begin(HALL_USER_LOGIN, player->id);
	packet.WriteInt(player->id) ;   //id   
	packet.WriteString(player->name) ;   //id  
	packet.WriteInt64(player->money);  
	packet.WriteString(player->json);
	packet.WriteString("mtkey");
	packet.End();
	printf("Send Login Packet to Server\n");
	printf("Data Send: player->id=[%d]\n", player->id);
	printf("Data Send: player->name=[%s]\n", player->name);
	printf("Data Send: player->money=[%ld]\n", player->money);
	return this->send(client,&packet);
}

int LoginProcess::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	int error_code = inputPacket->ReadShort();
	string error_mag = inputPacket->ReadString();
	printf("Recv Login Packet From Server\n");
	printf("Data Recv: error_code=[%d]\n",error_code);
	printf("Data Recv: error_msg=[%s]\n",error_mag.c_str());
	return CLIENT_MSG_LOGINCOMING;
	//return HALL_GET_MEMBERS;
}
