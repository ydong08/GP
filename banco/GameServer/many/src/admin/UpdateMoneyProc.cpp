#include "UpdateMoneyProc.h"
#include "GameCmd.h"
#include "Room.h"
#include "Logger.h"
#include "HallManager.h"
#include "ProcessManager.h"

int UpdateMoneyProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt )
{
	short cmd = inputPacket->GetCmdType();

	_LOG_DEBUG_("==>[UpdateMoneyProc] [0x%04x] \n", cmd);
	
	int uid = inputPacket->ReadInt();
	int64_t money = inputPacket->ReadInt64();
	int64_t safemoney = inputPacket->ReadInt64();
	int roll = inputPacket->ReadInt();
	int roll1 = inputPacket->ReadInt();
	int coin = inputPacket->ReadInt();
	int exp = inputPacket->ReadInt();

	_LOG_DEBUG_("[DATA Parse] uid=[%d]\n", uid);
	_LOG_DEBUG_("[DATA Parse] money=[%ld]\n", money);
	
	Room* room = Room::getInstance();
	Table *table = room->getTable();

	for(int i=0; i< room->getMaxUsers() ;++i)
	{
		Player * player = &room->players[i];
		if(player && player->id == uid)
		{
			//在开牌时间其实已经这局已经结算了
//			if(player->m_lBetArray[0] == 0 || table->isOpen())
//				player->m_lMoney = money;
//			else
//				player->m_lMoney = money - player->m_lBetArray[0];
			player->m_lMoney += money;

			_LOG_INFO_("Found uid=[%d] update money[%d] newmoney:%d \n", uid, money, player->m_lMoney);
//			OutputPacket response;
//			response.Begin(SERVER_MSG_UPDATE_MOENY, player->id);
//			response.WriteShort(0);
//			response.WriteString("");
//			response.WriteInt(player->id);
//			response.WriteInt64(player->m_lMoney);
//			response.End();
//			HallManager::getInstance()->sendToHall(player->m_nHallid, &response, false);
			break;
		}
	}	
	return 0;	
}  

REGISTER_PROCESS(MONEYSERVER_UPDATE_USERCOIN, UpdateMoneyProc)