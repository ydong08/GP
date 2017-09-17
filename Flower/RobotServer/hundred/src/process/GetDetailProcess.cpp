//---------------------------------------------------------------------------
#include "GetDetailProcess.h"
#include "HallHandler.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"

REGISTER_PROCESS(CLIENT_MSG_TABLEDET, GetDetailProcess)
//---------------------------------------------------------------------------
int GetDetailProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	OutputPacket packet;
	packet.Begin(CLIENT_MSG_TABLEDET, player->id);
	packet.WriteInt(player->id);
	packet.WriteInt(player->tid);
	packet.End();
	printf("Send GetDetailProcess Packet to Server\n");
	printf("Data Send: id=[%d]\n", player->id);
	printf("Data Send: tid=[%d]\n",player->tid);
	return this->send(client,&packet);
}

int GetDetailProcess::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	int error_code = inputPacket->ReadShort();
	string error_mag = inputPacket->ReadString();
	printf("Recv GetDetailProcess Packet From Server\n");
	printf("Data Recv: error_code=[%d]\n",error_code);
	printf("Data Recv: error_msg=[%s]\n",error_mag.c_str());

	if(error_code < 0)
		return EXIT;

	//Player* player = this->getPlayer();
	printf("Data Recv: uid=[%d] \n", inputPacket->ReadInt());
	short pstatus = inputPacket->ReadShort();
	//player->status = pstatus;
	printf("Data Recv: pstatus=[%d] \n", pstatus);
	printf("Data Recv: tid=[%d] \n", inputPacket->ReadInt());
	short tstatus = inputPacket->ReadShort();
	//player->table_status = tstatus;
	printf("Data Recv: tstatus=[%d] \n", tstatus);
	printf("Data Recv: limittime=[%d] \n", inputPacket->ReadShort());
	printf("Data Recv: seatid=[%d] \n", inputPacket->ReadShort());
	printf("Data Recv: viptype=[%d] \n", inputPacket->ReadShort());
	printf("Data Recv: money=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: winnum=[%d] \n", inputPacket->ReadInt());
	printf("Data Recv: losenum=[%d] \n", inputPacket->ReadInt());
	printf("Data Recv: tienum=[%d] \n", inputPacket->ReadInt());
	printf("Data Recv: MoneyCount=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: nwin=[%d] \n", inputPacket->ReadShort());
	printf("Data Recv: nlose=[%d] \n", inputPacket->ReadShort());
	printf("Data Recv: betplayer=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: betbanker=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: bettie=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: betKplayer=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: betKbanker=[%ld] \n", inputPacket->ReadInt64());

	printf("Data Recv: Bankerid=[%d] \n", inputPacket->ReadInt());
	printf("Data Recv: Bankerseatid=[%d] \n", inputPacket->ReadShort());
	printf("Data Recv: bankerNum=[%d] \n", inputPacket->ReadByte());
	printf("Data Recv: bankerMoney=[%ld] \n", inputPacket->ReadInt64());

	printf("Data Recv: table betplayer=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: table betbanker=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: table bettie=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: table betKplayer=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: table betKbanker=[%ld] \n", inputPacket->ReadInt64());

	printf("Data Recv: playerlimit=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: bankerlimit=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: tielimit=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: Kplayerlimit=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: Kbankerlimit=[%ld] \n", inputPacket->ReadInt64());

	return 0;

}
