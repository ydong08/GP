
#include "UpdateTabStatusProc.h"
#include "GameCmd.h"
#include "Configure.h"
#include "Player.h"
#include "PlayerManager.h"
#include "GameCmd.h"
#include "ProcessManager.h"
#include "Logger.h"

REGISTER_PROCESS(GMSERVER_MSG_TABSTATUS, UpdateTabStatusProc);

int UpdateTabStatusProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int UpdateTabStatusProc::doResponse(CDLSocketHandler* client, InputPacket* inputPacket, Context* pt )
{
	int error_code = inputPacket->ReadShort();
	string error_mag = inputPacket->ReadString();
	_LOG_DEBUG_("========UpdateTabStatusProc:doResponse=========\n");
	_LOG_DEBUG_("Data Recv: error_code=[%d]\n",error_code);
	_LOG_DEBUG_("Data Recv: error_msg=[%s]\n",error_mag.c_str());
	if(error_code < 0)
		return EXIT;
	int uid = inputPacket->ReadInt();
	_LOG_DEBUG_("Data Recv: uid=[%d] \n",uid);
	short pstatus = inputPacket->ReadShort();
	_LOG_DEBUG_("Data Recv: pstatus=[%d] \n", pstatus);
	_LOG_DEBUG_("Data Recv: tid=[%d] \n", inputPacket->ReadInt());
	short tstatus = inputPacket->ReadShort();
	_LOG_DEBUG_("Data Recv: tstatus=[%d] \n", tstatus);
	_LOG_DEBUG_("Data Recv: time=[%d] \n", inputPacket->ReadShort());

	_LOG_DEBUG_("Data Recv: playerlimit=[%ld] \n", inputPacket->ReadInt64());
	_LOG_DEBUG_("Data Recv: bankerlimit=[%ld] \n", inputPacket->ReadInt64());
	_LOG_DEBUG_("Data Recv: tielimit=[%ld] \n", inputPacket->ReadInt64());
	_LOG_DEBUG_("Data Recv: Kplayerlimit=[%ld] \n", inputPacket->ReadInt64());
	_LOG_DEBUG_("Data Recv: Kbankerlimit=[%ld] \n", inputPacket->ReadInt64());

	Player* player = PlayerManager::getInstance()->getPlayer(uid);
	if(tstatus == STATUS_TABLE_BET)
	{
		_LOG_DEBUG_("bettttttttttttttttttttttttttttter status\n");
		if(Configure::getInstance()->level == 2 || Configure::getInstance()->level == 3)
		{
			player->betNum = 0;
			player->betcoin = 0;
			player->bettype = 0;
			if(Configure::getInstance()->level == 2)
			{
				_LOG_DEBUG_("start better status id:%d\n", player->id);
				player->startBetTimer(1 + rand()%5);
			}
			else
				player->startBetTimer(1);
		}
	}
	if(tstatus == STATUS_TABLE_IDLE)
	{
		player->playNum++;
		if((Configure::getInstance()->level == 2 || Configure::getInstance()->level == 3) && player->playNum >= player->willPlayCount)
			return EXIT;

		if(Configure::getInstance()->level == 1)
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

