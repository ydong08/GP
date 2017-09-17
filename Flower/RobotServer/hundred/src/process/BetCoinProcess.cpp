//---------------------------------------------------------------------------
#include "BetCoinProcess.h"
#include "HallHandler.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"

REGISTER_PROCESS(CLIENT_MSG_BET_COIN, BetCoinProcess)
//---------------------------------------------------------------------------
int BetCoinProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	OutputPacket packet;
	packet.Begin(CLIENT_MSG_BET_COIN, player->id);
	packet.WriteInt(player->id);
	packet.WriteInt(player->tid);
	packet.WriteShort(player->seat_id);//level
	packet.WriteShort(player->bettype);//
	packet.WriteInt64(player->betcoin);  
	packet.WriteInt(0);
	packet.WriteInt(0);
	packet.End();
	printf("Send BetCoinProcess Packet to Server\n");
	printf("Data Send: player->id=[%d]\n", player->id);
	printf("Data Send: tid=[%d]\n",player->tid);
	printf("Data Send: seat_id=[%d]\n",player->seat_id);
	printf("Data Send: bettype=[%d]\n",player->bettype);
	printf("Data Send: betcoin=[%ld]\n",player->betcoin);
	return this->send(client,&packet);
}

int BetCoinProcess::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	int error_code = inputPacket->ReadShort();
	string error_mag = inputPacket->ReadString();

	printf("Recv BetCoinProcess Packet From Server\n");
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
	


	int betid = inputPacket->ReadInt();

	printf("Data Recv: bettype=[%d] \n", inputPacket->ReadShort());
	printf("Data Recv: betmoney=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: playerlimit=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: bankerlimit=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: tielimit=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: Kplayerlimit=[%ld] \n", inputPacket->ReadInt64());
	int newmoney = inputPacket->ReadInt64();
	if(betid == uid)
	{
		Player* player = PlayerManager::getInstance()->getPlayer(uid);
		player->money = newmoney;
	}
	
	return 0;

}
