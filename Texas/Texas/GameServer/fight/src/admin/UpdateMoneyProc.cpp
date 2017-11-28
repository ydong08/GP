#include "UpdateMoneyProc.h"
#include "GameCmd.h"
#include "Room.h"
#include "Logger.h"
#include "ProcessManager.h"
#include "ProtocolServerId.h"


int UpdateMoneyProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
	short cmd = inputPacket->GetCmdType();

	_LOG_DEBUG_("==>[UpdateMoneyProc] [0x%04x] \n", cmd);
	
	int uid = inputPacket->ReadInt();
	int64_t m_lMoney = inputPacket->ReadInt64();

	_LOG_DEBUG_("[DATA Parse] uid=[%d]\n", uid);
	_LOG_DEBUG_("[DATA Parse] m_lMoney=[%ld]\n", m_lMoney);

	Room* room = Room::getInstance();

	for(int i=0; i< room->getMaxUsers() ;++i)
	{
		Player * player = &room->players[i];
		if(player && player->id == uid)
		{
			player->m_lMoney += m_lMoney;
			//player->m_nRoll = roll;
			_LOG_INFO_("Found uid=[%d] update m_lMoney[%d]\n", uid, m_lMoney);
			break;
		}
	}	
	return 0;	
}

REGISTER_PROCESS(S2S_MONEY_NOTIFY_UPDATE_MONEY, UpdateMoneyProc)