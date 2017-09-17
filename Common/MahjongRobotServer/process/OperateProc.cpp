#include "OperateProc.h"
#include "HallHandler.h"
#include "Logger.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include "GameLogic.h"
#include <string>
using namespace std;

REGISTER_PROCESS(CLIENT_MSG_OPERATE, OperateProc)

OperateProc::OperateProc()
{
	this->name = "OperateProc";
}

OperateProc::~OperateProc()
{

} 

int OperateProc::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{

	HallHandler* clientHandler = reinterpret_cast <HallHandler*> (client);
	Player* player = PlayerManager::getInstance()->getPlayer(clientHandler->uid);
	if(player == NULL)
		return 0;

	tagOutCardResult OutCardResult;
	WORD wMeChairId = player->m_TableIndex;

	BYTE byCardCount = player->game_logic_.GetCardCount(player->m_cbCardIndex[wMeChairId]);
	if (wMeChairId == player->current_user_pos_)
	{
		if ((byCardCount + 1) % 3 != 0)
		{
			return 0;
		}	
	}
	else
	{
		if (byCardCount % 3 != 1)
		{
			return 0;
		}
	}

	try
	{

		//搜索出牌
		if (player->SearchOutCard(OutCardResult))
		{
			if (OutCardResult.cbOperateCode != WIK_NULL)
			{
				if (!(OutCardResult.cbOperateCode&player->action_)) throw 0;

				//响应操作
				player->OnOperateCard(OutCardResult.cbOperateCode, OutCardResult.cbOperateCard);
			}
			else
			{
				if (player->m_cbCardIndex[wMeChairId][player->game_logic_.SwitchToCardIndex(OutCardResult.cbOperateCard)] == 0) throw 0;

				//出牌
				player->OnOutCard(OutCardResult.cbOperateCard);
			}
		}
		else
		{
			//效验
			if (wMeChairId == player->current_user_pos_) throw 0;

			//响应操作
			player->OnOperateCard(WIK_NULL, 0);
		}
	}
	catch (...)
	{
		assert(false);
		//异常处理
		if (wMeChairId == player->current_user_pos_)
		{
			for (BYTE i = 0; i < MAX_INDEX; i++)
			{
				if (player->m_cbCardIndex[wMeChairId][i] > 0)
				{
					player->OnOutCard(player->game_logic_.SwitchToCardData(i));
					return 0;
				}
			}
		}
		else player->OnOperateCard(WIK_NULL, 0);
	}
	
	return 0;
}

int OperateProc::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*_LOG_DEBUG_("Recv OperateProc Packet From Server\n");
	_LOG_DEBUG_("Data Recv: retcode=[%d]\n",retcode);
	_LOG_DEBUG_("Data Recv: retmsg=[%s]\n",retmsg.c_str());*/
	if(retcode == -9)
		return 0;
	if(retcode < 0 || player == NULL)
	{
		return EXIT;
	}
	int tempUid = inputPacket->ReadInt();
	short tempUserState = inputPacket->ReadShort();

	int tempTid = inputPacket->ReadInt();
	short tempTableState = inputPacket->ReadShort();	
	if(tempTid != player->m_TableId)
		return EXIT;

	int tempLastUid = inputPacket->ReadInt();
	int tempHandCount = inputPacket->ReadByte();
	int tempNextUid = inputPacket->ReadInt();
	int tempCardType = inputPacket->ReadByte();
	int tempCardCounter = inputPacket->ReadByte();
	short tempLastTableIndex;
	for(int i = 0; i < 4; i++)
	{
		if(player->m_AllUid[i] == tempLastUid)
			tempLastTableIndex = i;
	}		
	if(player->m_Uid == tempNextUid)
	{
		player->startOutCardTimer(tempUid, 3 + rand() % 8);
	}
	return 0;
}

