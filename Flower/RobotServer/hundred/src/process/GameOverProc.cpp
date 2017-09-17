#include "GameOverProc.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "Configure.h"
#include "PlayerManager.h"
#include "Logger.h"

REGISTER_PROCESS(GMSERVER_MSG_GAMEOVER, GameOverProc)

int GameOverProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int GameOverProc::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
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
	//player->status = pstatus;
	printf("Data Recv: pstatus=[%d] \n", pstatus);
	printf("Data Recv: tid=[%d] \n", inputPacket->ReadInt());
	short tstatus = inputPacket->ReadShort();
	//player->table_status = tstatus;
	printf("Data Recv: tstatus=[%d] \n", tstatus);
	printf("Data Recv: Player Card:");
	for(int i = 0; i < 5; ++i)
	{
		for(int j = 0; j < 3; ++j)
			printf("0x%02x ",inputPacket->ReadByte());
		printf("cardtype:%d ",inputPacket->ReadByte());
		printf("times:%d\n",inputPacket->ReadByte());
		if(i > 0)
		{
			inputPacket->ReadInt64();
			inputPacket->ReadByte();
		}
	}
	
	printf("Data Recv: winscore=[%ld] \n", inputPacket->ReadInt64());
	printf("Data Recv: returnscore=[%ld] \n", inputPacket->ReadInt64());
	int64_t newmoney = inputPacket->ReadInt64();
	printf("Data Recv: moneyscore=[%ld] \n", newmoney);
	short limitnum =  inputPacket->ReadByte();
	printf("Data Recv: bankerNum=[%d] \n", limitnum);
	int64_t winscore = inputPacket->ReadInt64();
	printf("Data Recv: bankerMoney=[%ld] \n", winscore);
	_LOG_DEBUG_("limitnum:%d winscore:%ld\n", limitnum, winscore);
	Player* player = PlayerManager::getInstance()->getPlayer(uid);
	player->money = newmoney;
	player->stopBetTimer();
	if(Configure::getInstance().m_nLevel == 1)
	{
		if(uid == player->bankeruid)
		{
			player->bankerwinCount = winscore;
			player->bankerhasNum = limitnum;
			if(player->bankerwinCount > Configure::getInstance().bankerwincount)
			{
				if(rand()%10 < 9)
				{
					//_LOG_DEBUG_("player->id[%d] bankerwinCount:%d bankerwincount:%d\n",player->id, player->bankerwinCount, Configure::getInstance()->bankerwincount);
					return EXIT;
				}
			}
			if(player->bankerhasNum < Configure::getInstance().bankerhasnum)
			{
				if(rand()%10 < 5)
				{
					//_LOG_DEBUG_("player->id[%d] bankerhasNum:%d bankerhasnum:%d\n",player->id, player->bankerhasNum, Configure::getInstance()->bankerhasnum);
					return EXIT;
				}
			}
		}
	}
	//return EXIT;
	//return EXIT;
	return 0;

}

