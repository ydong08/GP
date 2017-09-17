//---------------------------------------------------------------------------

#include "ApplyBankerProc.h"
#include "HallHandler.h"
#include "Player.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"

REGISTER_PROCESS(CLIENT_MSG_APPLYBANKER, ApplyBankerProc)


//---------------------------------------------------------------------------
int ApplyBankerProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	OutputPacket packet;
	packet.Begin(CLIENT_MSG_APPLYBANKER, player->id);
	packet.WriteInt(player->id);
	packet.WriteInt(player->tid);
	packet.WriteShort(player->seat_id);//level
	packet.End();
	printf("Send ApplyBankerProc Packet to Server\n");
	printf("Data Send: player->id=[%d]\n", player->id);
	printf("Data Send: tid=[%d]\n",player->tid);
	printf("Data Send: seat_id=[%d]\n",player->seat_id);
	return this->send(client,&packet);
}

int ApplyBankerProc::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	int error_code = inputPacket->ReadShort();
	string error_mag = inputPacket->ReadString();

	printf("Recv ApplyBankerProc Packet From Server\n");
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


	/*Player* player = this->getPlayer();

	int applyid = inputPacket->ReadInt();
	if(applyid == uid)
	{
		//在上庄列表中
		player->bankerlist = 1;
	}

	printf("Data Recv: Applyid=[%d] \n", applyid);*/
	
	return 0;

}
