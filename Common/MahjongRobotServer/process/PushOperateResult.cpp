#include "PushOperateResult.h"
#include "HallHandler.h"
#include "Logger.h"
#include "GameCmd.h"
#include "GameServerConnect.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include <string>
using namespace std;

REGISTER_PROCESS(CLIENT_MSG_OPERATE_RESULT, PushOperateResult)

PushOperateResult::PushOperateResult()
{
	this->name = "PushOperateResult";
}

PushOperateResult::~PushOperateResult()
{

} 

int PushOperateResult::doRequest(CDLSocketHandler* client, InputPacket* pPacket, Context* pt )
{
	return 0;
}

int PushOperateResult::doResponse(CDLSocketHandler* clientHandler, InputPacket* inputPacket,Context* pt)  
{
	HallHandler* hallHandler = reinterpret_cast <HallHandler*> (clientHandler);
	Player* player = PlayerManager::getInstance()->getPlayer(hallHandler->uid);
	int retcode = inputPacket->ReadShort();
	string retmsg = inputPacket->ReadString();
	/*printf("Recv SendCardProc Packet From Server\n");
	printf("Data Recv: retcode=[%d]\n",retcode);
	printf("Data Recv: retmsg=[%s]\n",retmsg.c_str());*/
	if (retcode < 0 || player == NULL)
	{
		return EXIT;
	}
	int uid = inputPacket->ReadInt();

	if (uid != player->m_Uid)
	{
		LOGGER(E_LOG_ERROR) << "PushOperateResult doResponse error[" << uid << "!=" << player->m_Uid << "]";
		return -1;
	}

	//变量定义
	BYTE cbPublicCard = true;
	WORD wOperateUser = inputPacket->ReadInt();
	WORD provide_user_pos = inputPacket->ReadInt();
	BYTE cbOperateCode = inputPacket->ReadByte();
	BYTE cbOperateCard = inputPacket->ReadByte();
	//出牌变量
	if (cbOperateCode != WIK_NULL)
	{
		player->m_cbOutCardData = 0;
		player->m_wOutCardUser = INVALID_CHAIR;
	}

	//设置变量
	player->action_ = WIK_NULL;
	player->m_cbActionCard = 0;

	//设置组合
	if ((cbOperateCode&WIK_GANG) != 0)
	{
		//设置变量
		player->current_user_pos_ = INVALID_CHAIR;

		//组合扑克
		BYTE cbWeaveIndex = 0xFF;
		for (BYTE i = 0; i < player->m_cbWeaveCount[wOperateUser]; i++)
		{
			BYTE cbWeaveKind = player->m_WeaveItemArray[wOperateUser][i].cbWeaveKind;
			BYTE cbCenterCard = player->m_WeaveItemArray[wOperateUser][i].cbCenterCard;
			if ((cbCenterCard == cbOperateCard) && (cbWeaveKind == WIK_PENG))
			{
				cbWeaveIndex = i;
				player->m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard = true;
				player->m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind = cbOperateCode;
				player->m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser = provide_user_pos;
				break;
			}
		}

		//组合扑克
		if (cbWeaveIndex == 0xFF)
		{
			//暗杠判断
			cbPublicCard = (provide_user_pos == wOperateUser) ? false : true;

			//设置扑克
			cbWeaveIndex = player->m_cbWeaveCount[wOperateUser]++;
			player->m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard = cbPublicCard;
			player->m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard = cbOperateCard;
			player->m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind = cbOperateCode;
			player->m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser = provide_user_pos;
		}

		//扑克设置
		player->m_cbCardIndex[wOperateUser][player->game_logic_.SwitchToCardIndex(cbOperateCard)] = 0;
	}
	else if (cbOperateCode != WIK_NULL&&cbOperateCode != WIK_CHI_HU)
	{
		//设置变量
		player->current_user_pos_ = wOperateUser;

		//设置组合
		BYTE cbWeaveIndex = player->m_cbWeaveCount[wOperateUser]++;
		player->m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard = true;
		player->m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard = cbOperateCard;
		player->m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind = cbOperateCode;
		player->m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser = provide_user_pos;

		//组合界面
		BYTE cbWeaveCard[4] = { 0,0,0,0 }, cbWeaveKind = cbOperateCode;
		BYTE cbWeaveCardCount = player->game_logic_.GetWeaveCard(cbWeaveKind, cbOperateCard, cbWeaveCard);

		//删除扑克
		player->game_logic_.RemoveCard(cbWeaveCard, cbWeaveCardCount, &cbOperateCard, 1);
		player->game_logic_.RemoveCard(player->m_cbCardIndex[wOperateUser], cbWeaveCard, cbWeaveCardCount - 1);
	}

	//设置时间
	if (player->current_user_pos_ == player->m_TableIndex)
	{
		//设置时间
		player->startOutCardTimer(uid, 2 + rand() % 3);
	}
	return 0;
}


