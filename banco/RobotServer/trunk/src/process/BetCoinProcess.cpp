//---------------------------------------------------------------------------



#include "BetCoinProcess.h"
#include "HallHandler.h"
#include "GameCmd.h"
#include "Player.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "Logger.h"

REGISTER_PROCESS(CLIENT_MSG_BET_COIN, BetCoinProcess);

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
    int user = inputPacket->GetUid();
    BYTE opt = inputPacket->GetOptType();
    int src = inputPacket->GetSource();
    int seq = inputPacket->GetSeqNum();
	int error_code = inputPacket->ReadShort();          // ret code
	string error_mag = inputPacket->ReadString();       // ret msg

	printf("Recv BetCoinProcess Packet From Server\n");
	printf("Data Recv: error_code=[%d]\n",error_code);
	printf("Data Recv: error_msg=[%s]\n",error_mag.c_str());
	if(error_code < 0)
		return EXIT;
	int uid = inputPacket->ReadInt();                // player id
	short status = inputPacket->ReadShort();         // player status
	int tid = inputPacket->ReadInt();                // table id
	short tstatus = inputPacket->ReadShort();        // table status
	printf("Data Recv: uid=[%d]\n",uid);
	printf("Data Recv: tid=[%d]\n",tid);

	int betid = inputPacket->ReadInt();             // beter id

	printf("Data Recv: bettype=[%d] \n", inputPacket->ReadShort());         // bet type
	printf("Data Recv: betmoney=[%ld] \n", inputPacket->ReadInt64());       // bet money
    for (int i = 1; i < BETNUM; ++i)                // player BetArray 
    {
        inputPacket->ReadInt64( );
    }
    int newmoney = inputPacket->ReadInt64();
    Player* player = NULL;
    if (betid == uid)
    {
        player = PlayerManager::getInstance()->getPlayer(uid);
        player->money = newmoney;
    }

    for (int i = 1; i < BETNUM; ++i)
    {
        if (player)
        {
            player->m_lBetArray[i] = inputPacket->ReadInt64();
        }
        else
        {
            inputPacket->ReadInt64();
        }
    }
    inputPacket->ReadInt();
    inputPacket->ReadInt();
    for (int i = 1; i < BETNUM; ++i)                // table betAreaLimit
    {
        inputPacket->ReadInt64();
    }

    //printf("Data Recv: playerlimit=[%ld] \n", inputPacket->ReadInt64());
	//printf("Data Recv: bankerlimit=[%ld] \n", inputPacket->ReadInt64());
	//printf("Data Recv: tielimit=[%ld] \n", inputPacket->ReadInt64());
	//int newmoney = inputPacket->ReadInt64();

	return 0;
}

