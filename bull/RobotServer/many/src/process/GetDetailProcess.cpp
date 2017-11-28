//---------------------------------------------------------------------------

#include "GetDetailProcess.h"

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
	int uid = inputPacket->ReadInt();
	Player* player = PlayerManager::getInstance()->getPlayer(uid);
	if (player == NULL)
	{
		LOGGER(E_LOG_ERROR) << "uid  = " << uid << " not exist";
		return EXIT;
	}
	printf("Data Recv: uid=[%d] \n", uid);
	short pstatus = inputPacket->ReadShort();
	//player->status = pstatus;
	printf("Data Recv: pstatus=[%d] \n", pstatus);
	printf("Data Recv: tid=[%d] \n", inputPacket->ReadInt());
	short tstatus = inputPacket->ReadShort();
	//player->table_status = tstatus;
	printf("Data Recv: tstatus=[%d] \n", tstatus);
	printf("Data Recv: limittime=[%d] \n", inputPacket->ReadByte());
	printf("Data Recv: count player=[%d] \n", inputPacket->ReadShort());
	printf("Data Recv: seatid=[%d] \n", inputPacket->ReadShort());
	printf("Data Recv: money=[%ld] \n", inputPacket->ReadInt64());
	for (int i = 1; i < 5; i++)
	{
		printf("Data Recv: bet money=[%ld] \n", inputPacket->ReadInt64());
	}
	int64_t tabBetArray[BETNUM] = { 0 };
	for (int i = 1; i < 5; i++)	//NOTO: from 1!
	{
		tabBetArray[i] = inputPacket->ReadInt64();
		LOGGER(E_LOG_INFO) << "tab bet array[" << i << "] = " << tabBetArray[i];
	}
	int bankeruid = inputPacket->ReadInt();
	int banker_sid = inputPacket->ReadShort();
	std::string banker_name = inputPacket->ReadString();
	int64_t banker_money = inputPacket->ReadInt64();
	Player* banker = PlayerManager::getInstance()->getPlayer(bankeruid);
	if (bankeruid != 0 && banker == NULL)
	{
		banker = new Player();
		banker->init();
		banker->money = banker_money;
		PlayerManager::getInstance()->addPlayer(bankeruid, banker);
	}
	player->bankeruid = bankeruid;
	LOGGER(E_LOG_INFO) << "bankerid = " << bankeruid << " banker money = " 
		<< banker_money << " banker name = " << banker_name;
	PlayerManager::getInstance()->calculateAreaBetLimit(bankeruid);

	for (int i = 1; i < 5; i++)	//NOTO: from 1!
	{
		int64_t playerBet = inputPacket->ReadInt64();	//记录玩家（非机器人）各区域下注总数
		PlayerManager::getInstance()->areaTotalBetArray[i] = tabBetArray[i] - playerBet;	//计算机器人各区域下注总数
		LOGGER(E_LOG_DEBUG) << "bettype = " << i << " robot current total bet = " 
			<< PlayerManager::getInstance()->areaTotalBetArray[i]
			<< "player bet = " << playerBet
			;
	}

	return 0;

}
