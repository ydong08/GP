
#include "Packet.h"
#include "Player.h"
#include "PlayerManager.h"
#include "ProcessManager.h"
#include "Logger.h"
#include "GameCmd.h"
#include "GameLogic.h"
#include "MahjongAI.h"
#include "Protocol.h"

static std::string card_data_to_string(uint8_t* cards, size_t len)
{
	std::string tmp;
	for (size_t i = 0; i < len; i++)
	{
		tmp += TOHEX(cards[i]);
		tmp += "-";
	}
	return tmp;
}

//====================================================================================================

Player::Player()
{
	this->play_count = 0;
}

Player::~Player()
{
}

void Player::init()
{
	timer.init(this);
	this->play_count = 0;
}

int Player::login()
{
	BaseProcess* process = ProcessManager::getInstance().getProcesser(HALL_USER_LOGIN);
    return process->doRequest(this->handler, NULL, NULL);
}

int Player::logout()
{
	BaseProcess* process = ProcessManager::getInstance().getProcesser(CLIENT_MSG_LEAVE);
    return process->doRequest(this->handler, NULL, NULL);
}

int Player::heartbeat()
{
	BaseProcess* process = ProcessManager::getInstance().getProcesser(USER_HEART_BEAT);
    return process->doRequest(this->handler, NULL, NULL);
}

int Player::Operate()
{
	BaseProcess* process = ProcessManager::getInstance().getProcesser(CLIENT_MSG_OPERATE);
    return process->doRequest(this->handler, NULL, NULL);
}

void Player::startHeartTimer(int uid,int timeout)
{
	timer.startHeartTimer(uid, timeout);
}

void Player::stopHeartTimer()
{
	timer.stopHeartTimer();
}

void Player::startOutCardTimer(int uid,int timeout)
{
	timer.startOutCardTimer(uid, timeout);
}

void Player::stopOutCardTimer()
{
	timer.stopOutCardTimer();
}

void Player::startLeaveTimer(int timeout)
{
	timer.startLeaveTimer(timeout);
}

void Player::stopLeaveTimer()
{
	timer.stopLeaveTimer();
}

void Player::startChangeCardTimer(int uid, int timeout)
{
	timer.startChangeCardTimer(uid, timeout);
}

void Player::stopChangeCardTimer()
{
	timer.stopChangeCardTimer();
}

void Player::startDefineLackTimer(int uid, int timeout)
{
	timer.startDefineLackTimer(uid, timeout);
}

void Player::stopDefineLackTimer()
{
	timer.stopDefineLackTimer();
}

void Player::reset_data()
{
	//出牌信息
	m_cbOutCardData = 0;
	m_wOutCardUser = INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard, sizeof(this->m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount, sizeof(this->m_cbDiscardCount));

	//组合扑克
	ZeroMemory(m_cbWeaveCount, sizeof(this->m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray, sizeof(this->m_WeaveItemArray));

	m_Card.clear();

	//牌堆信息
	heap_head_ = INVALID_CHAIR;						//牌堆头
	heap_tail_ = INVALID_CHAIR;						//牌堆尾
	memset(heap_info_,0,sizeof(heap_info_));				//牌堆信息

	dice_point_ = 0;					//筛子点数
	banker_pos_ = INVALID_CHAIR;					//庄家方位

	current_user_pos_ = INVALID_CHAIR;
	left_card_count_ = 0;

	action_ = 0;
	m_cbActionCard = 0;						//操作牌;

	OperateCode_ = 0;
	OperateCard_ = 0;						//操作牌;

	memset(m_cbCardIndex, 0, sizeof(m_cbCardIndex));
	memset(m_cbRepertoryCard, 0, sizeof(m_cbRepertoryCard)); //库存扑克

	memset(m_cbWeaveCount, 0, sizeof(m_cbWeaveCount));//组合数目
	memset(m_WeaveItemArray, 0, sizeof(m_WeaveItemArray)); //组合扑克

}

//出牌
int Player::OnOutCard(BYTE cbOutCard)
{
	BaseProcess* process = ProcessManager::getInstance().getProcesser(CLIENT_MSG_OUT_CARD);

	OperateCard_ = cbOutCard;
	return process->doRequest(this->handler, NULL, NULL);

	return 0;
}

//操作牌
int Player::OnOperateCard(BYTE cbOperateCode, BYTE cbOperateCard)
{
	BaseProcess* process = ProcessManager::getInstance().getProcesser(CLIENT_MSG_OPERATE_CARD);

	OperateCode_ = cbOperateCode;
	OperateCard_ = cbOperateCard;
	return process->doRequest(this->handler, NULL, NULL);
}

//搜索听牌
bool Player::SearchTingCard(tagTingCardResult &TingCardResult)
{
	//变量定义
	ZeroMemory(&TingCardResult, sizeof(TingCardResult));
	BYTE bAbandonCardCount = 0;

	//构造扑克
	WORD wMeChairId = m_TableIndex;
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp, m_cbCardIndex[wMeChairId], sizeof(cbCardIndexTemp));

	BYTE cbCardCount = game_logic_.GetCardCount(cbCardIndexTemp);
	if ((cbCardCount - 2) % 3 == 0)
	{
		//听牌分析
		for (BYTE i = 0; i < MAX_INDEX - ZI_PAI_COUNT; i++)
		{
			//空牌过滤
			if (cbCardIndexTemp[i] == 0) continue;

			//听牌处理
			cbCardIndexTemp[i]--;

			//听牌判断
			bool bHuCard = false;
			bAbandonCardCount = TingCardResult.bAbandonCount;
			CChiHuRight chr;
			for (BYTE j = 0; j < MAX_INDEX - ZI_PAI_COUNT; j++)
			{
				//胡牌分析
				BYTE cbCurrentCard = game_logic_.SwitchToCardData(j);
				BYTE cbHuCardKind = game_logic_.AnalyseChiHuCard(cbCardIndexTemp, m_WeaveItemArray[wMeChairId], m_cbWeaveCount[wMeChairId], cbCurrentCard, chr);

				//结果判断
				if (cbHuCardKind != WIK_NULL)
				{
					bHuCard = true;
					TingCardResult.bTingCard[bAbandonCardCount][TingCardResult.bTingCardCount[bAbandonCardCount]++] = cbCurrentCard;
				}
			}
			if (bHuCard == true)
			{
				TingCardResult.bAbandonCard[TingCardResult.bAbandonCount++] = game_logic_.SwitchToCardData(i);
			}
			//还原处理
			cbCardIndexTemp[i]++;
		}
	}
	else
	{
		//听牌判断
		bAbandonCardCount = TingCardResult.bAbandonCount;
		CChiHuRight chr;
		for (BYTE j = 0; j < MAX_INDEX - ZI_PAI_COUNT; j++)
		{
			//胡牌分析
			BYTE cbCurrentCard = game_logic_.SwitchToCardData(j);
			BYTE cbHuCardKind = game_logic_.AnalyseChiHuCard(cbCardIndexTemp, m_WeaveItemArray[wMeChairId], m_cbWeaveCount[wMeChairId], cbCurrentCard, chr);

			//结果判断
			if (cbHuCardKind != WIK_NULL)
			{
				TingCardResult.bTingCard[bAbandonCardCount][TingCardResult.bTingCardCount[bAbandonCardCount]++] = cbCurrentCard;
			}
		}
	}

	//计算剩余牌
	BYTE n = 0;
	while (TingCardResult.bTingCardCount[n] > 0)
	{
		TingCardResult.bRemainCount[n] = TingCardResult.bTingCardCount[n] * 4;

		for (BYTE i = 0; i < TingCardResult.bTingCardCount[n]; i++)
		{
			BYTE bCardData = TingCardResult.bTingCard[n][i];
			//减自己牌
			if (cbCardIndexTemp[game_logic_.SwitchToCardIndex(bCardData)] > 0)
			{
				TingCardResult.bRemainCount[n] -= cbCardIndexTemp[game_logic_.SwitchToCardIndex(bCardData)];
				ASSERT(TingCardResult.bRemainCount[n] >= 0);
			}

			for (BYTE j = 0; j < GAME_PLAYER; j++)
			{
				//减组合牌
				for (BYTE k = 0; k < m_cbWeaveCount[j]; k++)
				{
					if (m_WeaveItemArray[j][k].cbCenterCard == bCardData)
					{
						TingCardResult.bRemainCount[n] -= m_WeaveItemArray[j][k].cbWeaveKind == WIK_GANG ? 4 : 3;
						ASSERT(TingCardResult.bRemainCount[n] >= 0);
					}
				}
				//减丢弃牌
				for (BYTE k = 0; k < m_cbDiscardCount[j]; k++)
				{
					if (bCardData == m_cbDiscardCard[j][k])
					{
						TingCardResult.bRemainCount[n]--;
						ASSERT(TingCardResult.bRemainCount[n] >= 0);
					}
				}
			}
		}

		n++;
	}

	return true;
}

//搜索出牌
bool Player::SearchOutCard(tagOutCardResult &OutCardResult)
{
	//初始化
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	WORD wMeChairId = m_TableIndex;
	if (wMeChairId == INVALID_CHAIR) return false;

	//判断胡
	if ((action_&WIK_CHI_HU) != WIK_NULL)
	{
		OutCardResult.cbOperateCode = WIK_CHI_HU;
		OutCardResult.cbOperateCard = m_cbActionCard;
		return true;
	}

	//转换索引
	BYTE byCard[MAX_COUNT], byCardCount = 0;
	for (BYTE i = 0; i < MAX_INDEX; i++)
	{
		for (BYTE j = 0; j < m_cbCardIndex[wMeChairId][i]; j++)
		{
			byCard[byCardCount++] = i;
		}
	}

	//判断听
	if (WIK_LISTEN == game_logic_.AnalyseTingCard(m_cbCardIndex[wMeChairId], m_WeaveItemArray[wMeChairId], m_cbWeaveCount[wMeChairId]))
	{
		tagTingCardResult TingCardResult;
		if (SearchTingCard(TingCardResult))
		{
			BYTE bMostIndex = 0;
			int nMostCount = -1;
			BYTE i = 0;
			while (true)
			{
				if (TingCardResult.bTingCardCount[i] == 0) break;

				if (TingCardResult.bRemainCount[i] > nMostCount)
				{
					bMostIndex = i;
					nMostCount = TingCardResult.bRemainCount[i];
				}

				i++;
			}
			//有牌可听
			if (nMostCount > 0)
			{
				//放弃操作
				if (wMeChairId != current_user_pos_) return false;

				OutCardResult.cbOperateCode = WIK_NULL;
				OutCardResult.cbOperateCard = TingCardResult.bAbandonCard[bMostIndex];
				return true;
			}
			//听死牌
			else if (wMeChairId == current_user_pos_)
			{
				//机器AI
				CMahjongAI AndroidAi;
				AndroidAi.SetCardData(byCard, byCardCount, m_WeaveItemArray[wMeChairId], m_cbWeaveCount[wMeChairId]);
				AndroidAi.Think();
				//从组合中拆牌
				BYTE byBadlyCard = 0xff;
				if (byCardCount <= 2)
					byBadlyCard = AndroidAi.GetBadlyCard();
				else
					byBadlyCard = AndroidAi.GetBadlyIn2Card();
				if (0xff != byBadlyCard)
				{
					OutCardResult.cbOperateCode = WIK_NULL;
					OutCardResult.cbOperateCard = game_logic_.SwitchToCardData(byBadlyCard);
					return true;
				}
				else
				{
					//从最佳三只组合中拆牌
					byBadlyCard = AndroidAi.GetBadlyIn3Card();
					if (0xff != byBadlyCard)
					{
						OutCardResult.cbOperateCode = WIK_NULL;
						OutCardResult.cbOperateCard = game_logic_.SwitchToCardData(byBadlyCard);
						return true;
					}
				}
			}
		}
	}

	BYTE cbActionCard = m_cbActionCard;

	//计算各种操作得分
	BYTE cbOperateCode[] = { WIK_GANG,WIK_PENG,WIK_LEFT,WIK_CENTER,WIK_RIGHT,WIK_NULL };
	int nOperateScore[] = { 0,0,0,0,0,0 };
	//计算原始分
	CMahjongAI AndroidAi;
	AndroidAi.SetEnjoinOutCard(NULL, 0);
	AndroidAi.SetCardData(byCard, byCardCount, m_WeaveItemArray[wMeChairId], m_cbWeaveCount[wMeChairId]);
	AndroidAi.Think();
	int nOrgScore = AndroidAi.GetMaxScore();
	//判断杠
	if (action_ & WIK_GANG)
	{
		if (current_user_pos_ == wMeChairId)
		{
			tagGangCardResult GangCardResult;
			game_logic_.AnalyseGangCard(m_cbCardIndex[wMeChairId], m_WeaveItemArray[wMeChairId], m_cbWeaveCount[wMeChairId], GangCardResult);

			//寻找最高分杠牌
			BYTE cbGangCard = 0;
			int nMaxGangScore = -1;
			for (BYTE i = 0; i < GangCardResult.cbCardCount; i++)
			{
				ASSERT(m_cbCardIndex[wMeChairId][game_logic_.SwitchToCardIndex(GangCardResult.cbCardData[i])] > 0);
				if (m_cbCardIndex[wMeChairId][game_logic_.SwitchToCardIndex(GangCardResult.cbCardData[i])] == 0) throw 0;

				//计算杠后得分
				AndroidAi.SetCardData(byCard, byCardCount, m_WeaveItemArray[wMeChairId], m_cbWeaveCount[wMeChairId]);
				AndroidAi.SetAction(WIK_GANG, game_logic_.SwitchToCardIndex(GangCardResult.cbCardData[i]));
				AndroidAi.Think();

				int nScore = AndroidAi.GetMaxScore();
				if (nScore > nMaxGangScore)
				{
					nMaxGangScore = nScore;
					cbGangCard = GangCardResult.cbCardData[i];
				}
			}

			ASSERT(nMaxGangScore != -1 && cbGangCard != 0);
			cbActionCard = cbGangCard;
			nOperateScore[0] = nMaxGangScore - nOrgScore;
		}
		else
		{
			ASSERT(m_cbCardIndex[wMeChairId][game_logic_.SwitchToCardIndex(cbActionCard)] > 0);
			if (m_cbCardIndex[wMeChairId][game_logic_.SwitchToCardIndex(cbActionCard)] == 0) throw 0;

			//计算杠后得分
			AndroidAi.SetCardData(byCard, byCardCount, m_WeaveItemArray[wMeChairId], m_cbWeaveCount[wMeChairId]);
			AndroidAi.SetAction(WIK_GANG, game_logic_.SwitchToCardIndex(cbActionCard));
			AndroidAi.Think();
			nOperateScore[0] = AndroidAi.GetMaxScore() - nOrgScore;
		}
	}
	//判断碰
	if (action_ & WIK_PENG)
	{
		AndroidAi.SetCardData(byCard, byCardCount, m_WeaveItemArray[wMeChairId], m_cbWeaveCount[wMeChairId]);
		AndroidAi.SetAction(WIK_PENG, game_logic_.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if (byBadlyIndex == 0xff)
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if (byBadlyIndex == 0xff)
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if (byBadlyIndex != 0xff)
		{
			AndroidAi.RemoveCardData(byBadlyIndex);
			AndroidAi.Think();
			nOperateScore[1] = AndroidAi.GetMaxScore() - nOrgScore;
		}
	}

	//左吃
	if ((E_SICHUAN_MAHJONG_GAME_ID != PacketBase::game_id) && (E_HEBEI_MAHJONG_GAME_ID != PacketBase::game_id) && (E_ZHENGZHOU_MAHJONG_GAME_ID != PacketBase::game_id))
	{
		if (action_ & WIK_LEFT)
		{
			AndroidAi.SetCardData(byCard, byCardCount, m_WeaveItemArray[wMeChairId], m_cbWeaveCount[wMeChairId]);
			AndroidAi.SetAction(WIK_LEFT, game_logic_.SwitchToCardIndex(cbActionCard));
			AndroidAi.Think();
			BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
			if (byBadlyIndex == 0xff)
			{
				byBadlyIndex = AndroidAi.GetBadlyIn2Card();
				if (byBadlyIndex == 0xff)
					byBadlyIndex = AndroidAi.GetBadlyIn3Card();
			}
			if (byBadlyIndex != 0xff)
			{
				AndroidAi.RemoveCardData(byBadlyIndex);
				AndroidAi.Think();
				nOperateScore[2] = AndroidAi.GetMaxScore() - nOrgScore;
			}
		}
		//中吃
		if (action_ & WIK_CENTER)
		{
			AndroidAi.SetCardData(byCard, byCardCount, m_WeaveItemArray[wMeChairId], m_cbWeaveCount[wMeChairId]);
			AndroidAi.SetAction(WIK_CENTER, game_logic_.SwitchToCardIndex(cbActionCard));
			AndroidAi.Think();
			BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
			if (byBadlyIndex == 0xff)
			{
				byBadlyIndex = AndroidAi.GetBadlyIn2Card();
				if (byBadlyIndex == 0xff)
					byBadlyIndex = AndroidAi.GetBadlyIn3Card();
			}
			if (byBadlyIndex != 0xff)
			{
				AndroidAi.RemoveCardData(byBadlyIndex);
				AndroidAi.Think();
				nOperateScore[3] = AndroidAi.GetMaxScore() - nOrgScore;
			}
		}
		//右吃
		if (action_ & WIK_RIGHT)
		{
			AndroidAi.SetCardData(byCard, byCardCount, m_WeaveItemArray[wMeChairId], m_cbWeaveCount[wMeChairId]);
			AndroidAi.SetAction(WIK_RIGHT, game_logic_.SwitchToCardIndex(cbActionCard));
			AndroidAi.Think();
			BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
			if (byBadlyIndex == 0xff)
			{
				byBadlyIndex = AndroidAi.GetBadlyIn2Card();
				if (byBadlyIndex == 0xff)
					byBadlyIndex = AndroidAi.GetBadlyIn3Card();
			}
			if (byBadlyIndex != 0xff)
			{
				AndroidAi.RemoveCardData(byBadlyIndex);
				AndroidAi.Think();
				nOperateScore[4] = AndroidAi.GetMaxScore() - nOrgScore;
			}
		}
	}

	//搜索废牌
	BYTE cbOutCardData = cbActionCard;
	if (current_user_pos_ == wMeChairId)
	{
		AndroidAi.SetCardData(byCard, byCardCount, m_WeaveItemArray[wMeChairId], m_cbWeaveCount[wMeChairId]);
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if (byBadlyIndex == 0xff)
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if (byBadlyIndex == 0xff)
			{
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
			
				if (byBadlyIndex == 0xff) return false;
			}
		}
		AndroidAi.RemoveCardData(byBadlyIndex);
		AndroidAi.Think();
		nOperateScore[5] = AndroidAi.GetMaxScore() - nOrgScore;
		cbOutCardData = game_logic_.SwitchToCardData(byBadlyIndex);
	}

	//获取最高分操作
	BYTE cbIndex = 0;
	for (BYTE i = 1; i < CountArray(nOperateScore); i++)
	{
		if (nOperateScore[cbIndex] < nOperateScore[i])
			cbIndex = i;
	}
	if ((cbOperateCode[cbIndex] & action_) &&
		(cbOperateCode[cbIndex] == WIK_GANG || nOperateScore[cbIndex] > 0))
	{
		OutCardResult.cbOperateCode = cbOperateCode[cbIndex];
		OutCardResult.cbOperateCard = cbActionCard;
		return true;
	}
	else
	{
		if (current_user_pos_ == wMeChairId)
		{
			OutCardResult.cbOperateCard = cbOutCardData;
			return true;
		}
		else return false;
	}
	return false;
}

int Player::ChangeCard()
{
	BaseProcess* process = ProcessManager::getInstance().getProcesser(CLIENT_MSG_CHANGED_CARD);
	return process->doRequest(this->handler, NULL, NULL);
}

int Player::DefineLack()
{
	BaseProcess* process = ProcessManager::getInstance().getProcesser(CLIENT_MSG_DEFINED_LACK);
	return process->doRequest(this->handler, NULL, NULL);
}

void Player::GetDefaultChangeCard(uint8_t* changed_cards)
{
	uint8_t color_end[3] = { 0 };
	uint8_t i = 0;
	uint8_t color_count[3] = { 0 };
	uint8_t color_gang[3] = { 0 };
	uint8_t wait_changed_count = 0;

	uint8_t				card_data[MAX_COUNT] = { 0 };

	game_logic_.SwitchToCardData(m_cbCardIndex[m_TableIndex], card_data);
	LOGGER(E_LOG_DEBUG) << "Hand card" << card_data_to_string(card_data, CountArray(card_data));

	// 统计牌的数量
	color_end[0] = game_logic_.SwitchToCardIndex(0x09);
	color_end[1] = game_logic_.SwitchToCardIndex(0x19);
	color_end[2] = game_logic_.SwitchToCardIndex(0x29);
	for (; i <= color_end[0]; i++)
	{
		color_count[0] += m_cbCardIndex[m_TableIndex][i];
		if (4 == m_cbCardIndex[m_TableIndex][i])
			color_gang[0]++;
	}

	for (; i <= color_end[1]; i++)
	{
		color_count[1] += m_cbCardIndex[m_TableIndex][i];
		if (4 == m_cbCardIndex[m_TableIndex][i])
			color_gang[1]++;
	}

	for (; i <= color_end[2]; i++)
	{
		color_count[2] += m_cbCardIndex[m_TableIndex][i];
		if (4 == m_cbCardIndex[m_TableIndex][i])
			color_gang[2]++;
	}

	// 组合选中的牌
	int	nSelect = SelectDefaultChangeCardColor(color_count, color_gang);
	int color_start = ((0 == nSelect) ? 0 : (color_end[nSelect - 1] + 1));
	uint8_t	color_card_index[9];
	tagSimpleItem simple_items[MAX_WEAVE];
	uint8_t	item_count = 0;

	for (int k = 0; k < 9; k++)
		color_card_index[k] = m_cbCardIndex[m_TableIndex][color_start + k];

	ZeroMemory(simple_items, sizeof(simple_items));
	AnalyseHandleColor(color_start, color_card_index, simple_items, item_count);

	//优先挑选单牌
	for (int k = 0; k < 9; k++)
	{
		if (0 != color_card_index[k])
		{
			changed_cards[wait_changed_count] = game_logic_.SwitchToCardData(color_start + k);
			wait_changed_count++;
			if (3 == wait_changed_count)
				return;
		}
	}

	//拆顺子
	for (int k = 0; k < item_count; k++)
	{
		if (group_sequence == simple_items[k].m_type)
		{
			LOGGER(E_LOG_DEBUG) << "group_sequence " << game_logic_.SwitchToCardData(simple_items[k].m_firstCardIndex);
			for (int l = 0; l < 3; l++)
			{
				changed_cards[wait_changed_count] = game_logic_.SwitchToCardData(simple_items[k].m_firstCardIndex + l);
				wait_changed_count++;
				if (3 == wait_changed_count)
					return;
			}
		}
	}

	//拆对子
	for (int k = 0; k < item_count; i++)
	{
		if (group_pair == simple_items[k].m_type)
		{
			LOGGER(E_LOG_DEBUG) << "group_pair " << game_logic_.SwitchToCardData(simple_items[k].m_firstCardIndex);
			for (int l = 0; l < 2; l++)
			{
				changed_cards[wait_changed_count] = game_logic_.SwitchToCardData(simple_items[k].m_firstCardIndex);
				wait_changed_count++;
				if (3 == wait_changed_count)
					return;
			}
		}
	}

	//拆刻子
	for (int k = 0; k < item_count; i++)
	{
		if (group_triplet == simple_items[k].m_type)
		{
			LOGGER(E_LOG_DEBUG) << "group_triplet " << game_logic_.SwitchToCardData(simple_items[k].m_firstCardIndex);
			for (int l = 0; l < 3; l++)
			{
				changed_cards[wait_changed_count] = game_logic_.SwitchToCardData(simple_items[k].m_firstCardIndex);
				wait_changed_count++;
				if (3 == wait_changed_count)
					return;
			}
		}
	}

	//拆杠子
	for (int k = 0; k < item_count; i++)
	{
		LOGGER(E_LOG_DEBUG) << "group_kong " << game_logic_.SwitchToCardData(simple_items[k].m_firstCardIndex);
		if (group_kong == simple_items[k].m_type)
		{
			for (int l = 0; l < 4; l++)
			{
				changed_cards[wait_changed_count] = game_logic_.SwitchToCardData(simple_items[k].m_firstCardIndex);
				wait_changed_count++;
				if (3 == wait_changed_count)
					return;
			}
		}
	}
}

int Player::SelectDefaultChangeCardColor(uint8_t *color_count, uint8_t *color_gang)
{
	bool	all_have_gang = true;
	int		nSelect = -1;

	//是否全部都有暗杠
	for (int i = 0; i < 3; i++)
	{
		if (0 == color_gang)
		{
			all_have_gang = false;
			break;
		}
	}

	//选择默认换牌的颜色
	for (int i = 0; i < 3; i++)
	{
		// 不满三张牌，不选择
		if (color_count[i] < 3)
			continue;

		// 如果不是全部都有暗杠，有暗杠的颜色不选
		if ((!all_have_gang) && (0 != color_gang[i]))
			continue;

		// 默认选择第一个颜色
		if (-1 == nSelect)
		{
			nSelect = i;
			continue;
		}

		// 优先选择牌数少的颜色
		if (color_count[i] >= color_count[nSelect])
			continue;

		nSelect = i;
	}

	return nSelect;
}

void Player::AnalyseHandleColor(uint8_t start_index, uint8_t *color_card_index, tagSimpleItem *simple_items, uint8_t &item_count)
{
	for (int i = 0; i < 9; i++)
	{
		if (4 == color_card_index[i])
		{
			color_card_index[i] = 0;
			simple_items[item_count].m_type = group_kong;
			simple_items[item_count].m_firstCardIndex = start_index + i;
			item_count++;
		}
	}

	for (int i = 0; i < 9; i++)
	{
		if (3 == color_card_index[i])
		{
			color_card_index[i] = 0;
			simple_items[item_count].m_type = group_triplet;
			simple_items[item_count].m_firstCardIndex = start_index + i;
			item_count++;
		}
	}

	for (int i = 0; i < 9; i++)
	{
		if (2 == color_card_index[i])
		{
			color_card_index[i] = 0;
			simple_items[item_count].m_type = group_pair;
			simple_items[item_count].m_firstCardIndex = start_index + i;
			item_count++;
		}
	}

	for (int i = 0; i < 7; i++)
	{
		if ((0 != color_card_index[i]) && (0 != color_card_index[i+1]) && (0 != color_card_index[i+2]))
		{
			color_card_index[i] -= 1;
			color_card_index[i + 1] -= 1;
			color_card_index[i + 2] -= 1;

			simple_items[item_count].m_type = group_sequence;
			simple_items[item_count].m_firstCardIndex = start_index + i;
			item_count++;
		}
	}
}

uint8_t Player::GetDefaultLackColor()
{
	uint8_t color_end[3] = { 0 };
	uint8_t i = 0;
	uint8_t color_count[3] = { 0 };
	uint8_t color_kong[3] = { 0 };
	uint8_t color_triplet[3] = { 0 };
	uint8_t color_pair[3] = { 0 };
	uint8_t wait_changed_count = 0;

	// 统计牌的数量
	color_end[0] = game_logic_.SwitchToCardIndex(0x09);
	color_end[1] = game_logic_.SwitchToCardIndex(0x19);
	color_end[2] = game_logic_.SwitchToCardIndex(0x29);
	for (; i <= color_end[0]; i++)
	{
		color_count[0] += m_cbCardIndex[m_TableIndex][i];
		switch (m_cbCardIndex[m_TableIndex][i])
		{
		case 4:
			color_kong[0] += 1;
			break;
		case 3:
			color_triplet[0] += 1;
			break;
		case 2:
			color_pair[0] += 1;
			break;
		default:
			break;
		}
	}

	for (; i <= color_end[1]; i++)
	{
		color_count[1] += m_cbCardIndex[m_TableIndex][i];
		switch (m_cbCardIndex[m_TableIndex][i])
		{
		case 4:
			color_kong[1] += 1;
			break;
		case 3:
			color_triplet[1] += 1;
			break;
		case 2:
			color_pair[1] += 1;
			break;
		default:
			break;
		}
	}

	for (; i <= color_end[2]; i++)
	{
		color_count[2] += m_cbCardIndex[m_TableIndex][i];
		switch (m_cbCardIndex[m_TableIndex][i])
		{
		case 4:
			color_kong[2] += 1;
			break;
		case 3:
			color_triplet[2] += 1;
			break;
		case 2:
			color_pair[2] += 1;
			break;
		default:
			break;
		}
	}

	//选择默认换牌的颜色
	int			nSelect = -1;
	for (int i = 0; i < 3; i++)
	{
		// 默认选择第一个颜色
		if (-1 == nSelect)
		{
			nSelect = i;
			continue;
		}

		// 优先选择牌数少的颜色
		if (color_count[i] < color_count[nSelect])
		{
			nSelect = i;
			continue;
		}

		if (color_count[i] > color_count[nSelect])
			continue;

		if (color_kong[i] < color_kong[nSelect])
		{
			nSelect = i;
			continue;
		}

		if (color_kong[i] > color_kong[nSelect])
			continue;

		if (color_triplet[i] < color_triplet[nSelect])
		{
			nSelect = i;
			continue;
		}

		if (color_triplet[i] > color_triplet[nSelect])
			continue;

		if (color_pair[i] < color_pair[nSelect])
		{
			nSelect = i;
			continue;
		}

		if (color_pair[i] > color_pair[nSelect])
		{
			continue;
		}

		if (1 == (rand() % 2))
			nSelect = i;
	}

	return (uint8_t)nSelect;
}