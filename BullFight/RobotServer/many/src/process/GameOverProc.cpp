
#include "GameOverProc.h"

int GameOverProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int GameOverProc::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	int error_code = inputPacket->ReadShort();
	string error_mag = inputPacket->ReadString();
	if(error_code < 0)
		return EXIT;
	int uid = inputPacket->ReadInt();
	short pstatus = inputPacket->ReadShort();
	//player->status = pstatus;
	int tid = inputPacket->ReadInt();
	short tstatus = inputPacket->ReadShort();
	//player->table_status = tstatus;
	for(int i = 0; i < 5; ++i)
	{
		for(int j = 0; j < 5; ++j)
			printf("0x%02x ",inputPacket->ReadByte());
		printf("cardtype:%d ",inputPacket->ReadByte());
		printf("times:%d\n",inputPacket->ReadByte());
		if(i > 0)
		{
			inputPacket->ReadInt64();
			inputPacket->ReadByte();
		}
	}
	
	int64_t winscore = inputPacket->ReadInt64();
	int returnscore = inputPacket->ReadInt64();
	int64_t newmoney = inputPacket->ReadInt64();
	short limitnum =  inputPacket->ReadByte();
	int64_t bankermoney = inputPacket->ReadInt64();
	ULOGGER(E_LOG_DEBUG, uid) << "Data Recv: error_code = " << error_code
		<< " error_msg = " << error_mag
		<< " pstatus = " << pstatus
		<< " tid = " << tid
		<< " tstatus = " << tstatus
		<< "winscore = " << winscore
		<< "returnscore = " << returnscore
		<< " newmoney = " << newmoney
		<< " limitnum = " << limitnum
		<< " bankermoney = " << bankermoney
		;
	Player* player = PlayerManager::getInstance()->getPlayer(uid);
	player->money = newmoney;
	player->stopBetTimer();
	if(Configure::getInstance()->m_nLevel == 1)
	{
		if(uid == player->bankeruid)
		{
			player->bankerwinCount = winscore;
			player->bankerhasNum = limitnum;
			if(player->bankerwinCount > Configure::getInstance()->bankerwincount)
			{
				if(rand()%10 < 9)
				{
					//_LOG_DEBUG_("player->id[%d] bankerwinCount:%d bankerwincount:%d\n",player->id, player->bankerwinCount, Configure::getInstance()->bankerwincount);
					return EXIT;
				}
			}
			if(player->bankerhasNum < Configure::getInstance()->bankerhasnum)
			{
				if(rand()%10 < 5)
				{
					//_LOG_DEBUG_("player->id[%d] bankerhasNum:%d bankerhasnum:%d\n",player->id, player->bankerhasNum, Configure::getInstance()->bankerhasnum);
					return EXIT;
				}
			}
		}
	}
	PlayerManager::getInstance()->ResetBetArray();
	//return EXIT;
	//return EXIT;
	return 0;

}

