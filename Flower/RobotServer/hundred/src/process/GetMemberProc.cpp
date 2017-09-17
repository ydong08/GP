//---------------------------------------------------------------------------
#include "GetMemberProc.h"
#include "HallHandler.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"

REGISTER_PROCESS(HALL_GET_MEMBERS, GetMemberProc)

//---------------------------------------------------------------------------
int GetMemberProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	OutputPacket packet;
	packet.Begin(HALL_GET_MEMBERS, player->id);
	packet.WriteInt(player->id);
	packet.End();
	printf("Send GetMemberProc Packet to Server\n");
	printf("Data Send: player->id=[%d]\n", player->id);
	return this->send(client,&packet);
}

int GetMemberProc::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	int error_code = inputPacket->ReadShort();
	string error_mag = inputPacket->ReadString();

	printf("Recv GetMemberProc Packet From Server\n");
	printf("Data Recv: error_code=[%d]\n",error_code);
	printf("Data Recv: error_msg=[%s]\n",error_mag.c_str());
	if(error_code < 0)
		return EXIT;
	
	return 0;

}
