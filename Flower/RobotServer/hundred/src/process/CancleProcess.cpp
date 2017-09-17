
#include "CancleProcess.h"
#include "HallHandler.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"

REGISTER_PROCESS(CLIENT_MSG_CANCLEBANKER, CancleProcess)

int CancleProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	OutputPacket packet;
	packet.Begin(CLIENT_MSG_CANCLEBANKER, player->id);
	packet.WriteInt(player->id);
	packet.WriteInt(player->tid);
	packet.WriteShort(player->seat_id);//level
	packet.End();
	printf("Send CancleProcess Packet to Server\n");
	printf("Data Send: player->id=[%d]\n", player->id);
	printf("Data Send: tid=[%d]\n",player->tid);
	printf("Data Send: seat_id=[%d]\n",player->seat_id);
	return this->send(client,&packet);
}

int CancleProcess::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	int error_code = inputPacket->ReadShort();
	string error_mag = inputPacket->ReadString();

	printf("Recv CancleProcess Packet From Server\n");
	printf("Data Recv: error_code=[%d]\n",error_code);
	printf("Data Recv: error_msg=[%s]\n",error_mag.c_str());
	if(error_code < 0)
		return EXIT;
	int uid = inputPacket->ReadInt();
	short status = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	printf("Data Recv: uid=[%d]\n",uid);
	printf("Data Recv: tid=[%d]\n",tid);

	int canlceid = inputPacket->ReadInt();

	printf("Data Recv: canlceid=[%d] \n", canlceid);
	
	return 0;

}
