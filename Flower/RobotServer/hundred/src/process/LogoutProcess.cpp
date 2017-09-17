//---------------------------------------------------------------------------
#include "LogoutProcess.h"
#include "HallHandler.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"

REGISTER_PROCESS(CLIENT_MSG_LEAVE, LogoutProcess)
//---------------------------------------------------------------------------
int LogoutProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	OutputPacket packet;
	packet.Begin(CLIENT_MSG_LEAVE, player->id);;
	packet.WriteInt(player->id);
	packet.WriteInt(player->tid) ;
	packet.WriteShort(player->seat_id) ;
	packet.End();
	printf("Send LogoutProcess Packet to Server\n");
	printf("Data Send: id=[%d]\n", player->id);
	printf("Data Send: tid=[%d]\n",player->tid);
	printf("Data Send: seat_id=[%d]\n",player->seat_id);
	return this->send(client,&packet);
}

int LogoutProcess::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	int error_code = inputPacket->ReadShort();
	string error_mag = inputPacket->ReadString();

	printf("Recv Leave Packet From Server\n");
	printf("Data Recv: error_code=[%d]\n",error_code);
	printf("Data Recv: error_msg=[%s]\n",error_mag.c_str());
	if(error_code < 0)
		return EXIT;

	int uid = inputPacket->ReadInt();
	short status = inputPacket->ReadShort();
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	int leaver = inputPacket->ReadInt();
	printf("Data Recv: uid=[%d]\n",uid);
	printf("Data Recv: tid=[%d]\n",tid);
	if(uid == leaver)
		return EXIT;

//	return HALL_MSG_CHANGETAB;
	return 0;

}
