//---------------------------------------------------------------------------
#include "LoginComingProc.h"
#include "HallHandler.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "Configure.h"

REGISTER_PROCESS(HALL_GET_MEMBERS, LoginComingProc)
//---------------------------------------------------------------------------
int LoginComingProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	OutputPacket packet;
	packet.Begin(CLIENT_MSG_LOGINCOMING, player->id);
	packet.WriteInt(player->id) ;
	packet.WriteString(player->name) ;
	packet.WriteInt(player->tid) ;
	packet.WriteInt64(player->money);
	packet.WriteShort(5) ;
	packet.WriteString(player->json);
	packet.End();
	printf("Send LoginComingProc Packet to Server\n");
	printf("Data Send: id=[%d]\n", player->id);
	printf("Data Send: name=[%s]\n", player->name);
	printf("Data Send: tid=[%d]\n", player->tid);
	printf("Data Send: money=[%ld]\n",player->money);
	return this->send(client,&packet);
}

int LoginComingProc::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	int error_code = inputPacket->ReadShort();
	string error_mag = (inputPacket->ReadString()).c_str();

	printf("Recv LoginComingProc Packet From Server\n");
	printf("Data Recv: error_code=[%d]\n",error_code);
	printf("Data Recv: error_msg=[%s]\n",error_mag.c_str());

	if(error_code < 0)
		return EXIT;
	//Player* player = this->getPlayer();
	int uid = inputPacket->ReadInt();
	printf("Data Recv: uid=[%d] \n", uid);
	short pstatus = inputPacket->ReadShort();
	
	printf("Data Recv: pstatus=[%d] \n", pstatus);
	int tid = inputPacket->ReadInt();
	printf("Data Recv: tid=[%d] \n",tid); 
	Player* player = PlayerManager::getInstance()->getPlayer(uid);

	if(player && player->tid != tid)
		return EXIT;
	short tstatus = inputPacket->ReadShort();
	//player->table_status = tstatus;
	printf("Data Recv: tstatus=[%d] \n", tstatus);
	int comingUid = inputPacket->ReadInt();
	printf("Data Recv: comingUid=[%d] \n", comingUid);
	short countplayer = inputPacket->ReadShort();
	short sid = inputPacket->ReadShort();

	printf("Data Recv: seat_id=[%d] \n", sid);
	int64_t lmoney = inputPacket->ReadInt64();

	short limittime = inputPacket->ReadByte();
	
	if(uid == comingUid)
	{
		player->seat_id = sid;
		player->startHeartTimer(30);
		if(Configure::getInstance().m_nLevel == 1)
			return CLIENT_MSG_APPLYBANKER;
		if(Configure::getInstance().m_nLevel == 2 && tstatus == STATUS_TABLE_BET && limittime > 1)
			player->startBetTimer(1);
		if(Configure::getInstance().m_nLevel == 3 && tstatus == STATUS_TABLE_BET)
			player->startBetTimer(1);

	}
	return 0;

}
