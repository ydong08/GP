
#include "RotateProcess.h"

int RotateProcess::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int RotateProcess::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	int error_code = inputPacket->ReadShort();
	string error_mag = inputPacket->ReadString();
	printf("Data Recv: error_code=[%d]\n",error_code);
	printf("Data Recv: error_msg=[%s]\n",error_mag.c_str());
	if(error_code < 0)
		return EXIT;
	int uid = inputPacket->ReadInt();
	printf("Data Recv: uid=[%d] \n", uid);
	short pstatus = inputPacket->ReadShort();
	printf("Data Recv: pstatus=[%d] \n", pstatus);
	printf("Data Recv: tid=[%d] \n", inputPacket->ReadInt());
	short tstatus = inputPacket->ReadShort();
	printf("Data Recv: tstatus=[%d] \n", tstatus);
	int preuid = inputPacket->ReadInt();
	printf("Data Recv: preuid=[%d] \n", preuid);
	printf("Data Recv: preseatid=[%d] \n", inputPacket->ReadShort());
	int bankerid = inputPacket->ReadInt();
	printf("Data Recv: bankerid=[%d] \n", bankerid);
	printf("Data Recv: bankerseatid=[%d] \n", inputPacket->ReadShort());
	int64_t banker_money = inputPacket->ReadInt64();
	printf("Data Recv: bankerMoney=[%ld] \n", banker_money);
	string name = inputPacket->ReadString(); 
	string json = inputPacket->ReadString(); 
	short bankerhasNum = inputPacket->ReadByte(); 
	Player* player = PlayerManager::getInstance()->getPlayer(uid);
	player->bankeruid = bankerid;
	Player* banker = PlayerManager::getInstance()->getPlayer(bankerid);
	if (bankerid != 0 && banker == NULL)
	{
		banker = new Player();
		banker->init();
		banker->money = banker_money;
		PlayerManager::getInstance()->addPlayer(bankerid, banker);
	}
	LOGGER(E_LOG_INFO) << "bankerid = " << bankerid << " banker money = " << banker_money;
	PlayerManager::getInstance()->calculateAreaBetLimit(bankerid);
	player->bankerhasNum = bankerhasNum;
	if(preuid == uid)
	{
		//PlayerManager::getInstance()->delPlayer(uid);
		return EXIT;
	}
	return 0;
	//return EXIT;

}

