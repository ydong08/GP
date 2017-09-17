
#include "UpdateTabStatusProc.h"

int UpdateTabStatusProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int UpdateTabStatusProc::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	int error_code = inputPacket->ReadShort();
	string error_mag = inputPacket->ReadString();
	printf("========UpdateTabStatusProc:doResponse=========\n");
	printf("Data Recv: error_code=[%d]\n",error_code);
	printf("Data Recv: error_msg=[%s]\n",error_mag.c_str());
	if(error_code < 0)
		return EXIT;
	int uid = inputPacket->ReadInt();
	printf("Data Recv: uid=[%d] \n",uid);
	short pstatus = inputPacket->ReadShort();
	printf("Data Recv: pstatus=[%d] \n", pstatus);
	printf("Data Recv: tid=[%d] \n", inputPacket->ReadInt());
	short tstatus = inputPacket->ReadShort();
	printf("Data Recv: tstatus=[%d] \n", tstatus);
	printf("Data Recv: time=[%d] \n", inputPacket->ReadShort());

	printf("Data Recv: playerlimit=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: bankerlimit=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: tielimit=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: Kplayerlimit=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: Kbankerlimit=[%ld] \n", inputPacket->ReadInt64());

	Player* player = PlayerManager::getInstance()->getPlayer(uid);
	if(tstatus == STATUS_TABLE_BET)
	{
		printf("bettttttttttttttttttttttttttttter status\n");
		if(Configure::getInstance()->m_nLevel == 2 || Configure::getInstance()->m_nLevel == 3)
		{
			player->betNum = 0;
			player->betcoin = 0;
			player->bettype = 0;
			if(Configure::getInstance()->m_nLevel == 2)
			{
				printf("start better status id:%d\n", player->id);
				player->startBetTimer(1 + rand()%5);
			}
			else
				player->startBetTimer(1);
		}
	}
	if(tstatus == STATUS_TABLE_IDLE)
	{
		player->playNum++;
		if((Configure::getInstance()->m_nLevel == 2 || Configure::getInstance()->m_nLevel == 3) && player->playNum >= player->willPlayCount)
			return EXIT;

		if(Configure::getInstance()->m_nLevel == 1)
		{
			if(player->id != player->bankeruid)
			{
				if(player->playNum >= player->willPlayCount)
				{
					//_LOG_DEBUG_("player->id[%d] playNum:%d willPlayCount:%d\n",player->id, player->playNum, player->willPlayCount);
					return EXIT;
				}
			}

		}

	}
	return 0;

}

