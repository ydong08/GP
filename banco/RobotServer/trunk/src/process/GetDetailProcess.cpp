//---------------------------------------------------------------------------

#include "GetDetailProcess.h"
#include "HallHandler.h"
#include "GameCmd.h"
#include "Player.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"

REGISTER_PROCESS(CLIENT_MSG_TABLEDET, GetDetailProcess);

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
    int32_t uid = inputPacket->ReadInt();
	printf("Data Recv: uid=[%d] \n", uid);
    Player* player = PlayerManager::getInstance()->getPlayer(uid);
	short pstatus = inputPacket->ReadShort();
	//player->status = pstatus;
	printf("Data Recv: pstatus=[%d] \n", pstatus);
	printf("Data Recv: tid=[%d] \n", inputPacket->ReadInt());
	short tstatus = inputPacket->ReadShort();
	//player->table_status = tstatus;
	printf("Data Recv: tstatus=[%d] \n", tstatus);
    inputPacket->ReadShort();    //printf("Data Recv: limittime=[%d] \n", inputPacket->ReadShort());
    inputPacket->ReadShort();    //printf("Data Recv: seatid=[%d] \n", inputPacket->ReadShort());
    inputPacket->ReadShort();    //printf("Data Recv: viptype=[%d] \n" , inputPacket->ReadShort());
	printf("Data Recv: money=[%ld] \n", inputPacket->ReadInt64());

    for (int i = 1; i < BETNUM; ++i)
    {
        inputPacket->ReadInt64();
    }

    for (int i = 1; i < BETNUM; ++i)
    {
        player->m_lBetArray[i] = inputPacket->ReadInt64();
    }

    inputPacket->ReadInt();      // banker id
    inputPacket->ReadShort( );   // banker SeatID
    inputPacket->ReadString( );  // banker name
    inputPacket->ReadInt64( );   // banke Money
    inputPacket->ReadString( );  // banker json
    inputPacket->ReadByte( );    // table  MaxBankerNum - table bankernum
    inputPacket->ReadByte( );    // player is in banker list flag
    inputPacket->ReadInt64( );   // table Bankerlimit
    inputPacket->ReadInt( );     // table LimitCoin
    inputPacket->ReadInt64( );   // player Return Score

    for (int i = 1; i < BETNUM; ++i)
    {
        inputPacket->ReadInt64();
    }

    inputPacket->ReadInt64( );   // table MinBetNum
    inputPacket->ReadInt( );     //table MaxBetNum
    for (int i = 0; i < CHIP_COUNT; i++)  //table ChipArray
    {
        inputPacket->ReadInt( );
    }

    /*
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
    */

	return 0;

}
