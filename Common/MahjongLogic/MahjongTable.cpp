#include "MahjongTable.h"
#include "MahjongPlayer.h"

#include "AllocSvrConnect.h"
#include "Logger.h"

#ifndef _WIN32
#define __min(a,b) (((a) < (b)) ? (a) : (b))
#endif

static std::string CardDataToString(uint8_t* cards, size_t len)
{
	std::string tmp;
	for (size_t i = 0; i < len; i++)
	{
		tmp += TOHEX(cards[i]);
		tmp += "-";
	}
	return tmp;
}

CMahjongTable::CMahjongTable(uint32_t id, CMahjongLogic *pMahjongLogic)
{
	for (int i = 0; i < GAME_PLAYER; i++)
		m_players[i] = NULL;
	m_nCurPlayerCount = 0;
	m_bPlaying = false;
	m_capabilitis = 0;

	ASSERT(pMahjongLogic);
	m_pGameLogic = pMahjongLogic;

	ResetData();
}

CMahjongTable::~CMahjongTable()
{

}

bool CMahjongTable::AddPlayer(CMahjongPlayer *pPlayer)
{
	ASSERT(!m_bPlaying);
	ASSERT(pPlayer);
	ASSERT(m_nCurPlayerCount < GAME_PLAYER);

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (NULL == m_players[i])
		{
			m_players[i] = pPlayer;
			pPlayer->m_pTable = this;
			pPlayer->m_nSeatPos = i;
			m_nCurPlayerCount++;
			return true;
		}
	}

	return false;
}

void CMahjongTable::RemovePlayer(CMahjongPlayer *pPlayer)
{
	ASSERT(!m_bPlaying);
	ASSERT(pPlayer);
	ASSERT(m_nCurPlayerCount > 0);

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (m_players[i] == pPlayer)
		{
			m_nCurPlayerCount--;
			m_players[i] = NULL;
			break;
		}
	}
}

CMahjongPlayer* CMahjongTable::GetAt(uint8_t nPos)
{
	ASSERT(nPos < GAME_PLAYER);
	return m_players[nPos];
}

CMahjongPlayer* CMahjongTable::Find(uint32_t uid)
{
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if ((NULL != m_players[i]) && (uid == m_players[i]->m_id))
			return m_players[i];
	}

	return NULL;
}

void CMahjongTable::Clear()
{
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (m_players[i])
		{
			m_players[i]->Clear();
			m_players[i] = NULL;
		}
	}
	m_nCurPlayerCount = 0;
	m_bPlaying = false;
}

void CMahjongTable::GameStart()
{
	// 复位数据
	ResetData();

	// 发牌
	DealCard();
	this->OnDealCardCompleted();

	m_bPlaying = true;
}

void CMahjongTable::BankStartOutCard()
{
	//杠牌判断
	ASSERT(INVALID_CHAIR != m_nBankerPos);
	
	CMahjongPlayer*		pBanker = m_players[m_nBankerPos];
		
	AnalyseBeforeOutCard(pBanker);
	this->OnBankStartOutCard();
}

void CMahjongTable::ResetData()
{
	m_nBankerPos = INVALID_CHAIR;
	m_nReplacePos = 0;
	m_nOutCardPos = 0;
	m_nOutCardData = 0;
	m_nOutCardCount = 0;
	
	m_nSendCardData = 0;
	m_nSendCardCount = 0;
	
	m_nLeftCardCount = 0;
	m_nEndLeftCount = 0;
	ZeroMemory(m_allCardData, sizeof(m_allCardData));

	m_nHeapHead = INVALID_CHAIR;
	m_nHeapTail = INVALID_CHAIR;
	ZeroMemory(m_heapinfo, sizeof(m_heapinfo));

	m_nResumePos = INVALID_CHAIR;
	m_nCurrentPos = INVALID_CHAIR;
	m_nProvidePos = INVALID_CHAIR;
	m_nProvideData = 0;
	
	m_bQiangGang = false;
	m_bGangPao = false;
	m_bWaitForOperate = false;
}

void CMahjongTable::DealCard()
{
	// 投掷骰子,确定牌墙位置与庄家
	uint8_t nRand1 = rand() % 6 + 1;
	uint8_t nRand2 = rand() % 6 + 1;
	if (INVALID_CHAIR == m_nBankerPos)
	{
		uint8_t nRand3 = rand() % 6 + 1;
		uint8_t nRand4 = rand() % 6 + 1;

		m_nDicePoint = MAKELONG(MAKEWORD(nRand1, nRand2), MAKEWORD(nRand3, nRand4));
		m_nBankerPos = ((BYTE)(m_nDicePoint >> 24) + (BYTE)(m_nDicePoint >> 16) - 1) % GAME_PLAYER;
	}
	else
	{
		m_nDicePoint = MAKELONG(MAKEWORD(nRand1, nRand2), 0);
	}

	// 洗牌
	m_nLeftCardCount = g_maxRepertory;
	m_pGameLogic->RandCardData(m_allCardData, g_maxRepertory);
	LOGGER(E_LOG_DEBUG) << CardDataToString(m_allCardData, g_maxRepertory);

	// 分发牌
	for (uint8_t i = 0; i < GAME_PLAYER; i++)
	{
		ASSERT(m_players[i]);

		m_nLeftCardCount -= (MAX_COUNT - 1);
		if (m_players[i])
			m_pGameLogic->SwitchToCardIndex(&m_allCardData[m_nLeftCardCount], MAX_COUNT - 1, m_players[i]->m_handCardIndex);
	}

	// 庄家在多发一张
	m_nSendCardCount++;
	m_nSendCardData = m_allCardData[--m_nLeftCardCount];
	CMahjongPlayer* pBanker = m_players[m_nBankerPos];
	uint8_t sendCardIndex = m_pGameLogic->SwitchToCardIndex(m_nSendCardData);
	pBanker->m_handCardIndex[sendCardIndex]++;

	// 初始化位置信息
	m_nProvideData = 0;
	m_nProvidePos = INVALID_CHAIR;
	m_nCurrentPos = m_nBankerPos;

	// 牌墙信息
	uint16_t nDice = (uint16_t)(m_nDicePoint & 0xffff);
#ifdef _WIN32
	uint8_t nDiceTakeCount = HIBYTE(nDice) + LOBYTE(nDice);
#else
	uint8_t nDiceTakeCount = HIGHBYTE(nDice) + LOWBYTE(nDice);
#endif

	uint8_t nTakeChairPos = (m_nBankerPos + nDiceTakeCount - 1) % GAME_PLAYER;
	uint8_t nTakeCount = g_maxRepertory - m_nLeftCardCount;
	for (uint16_t i = 0; i < GAME_PLAYER; i++)
	{
		//计算数目
		uint8_t nValidCount = g_nHeapFullCount - m_heapinfo[nTakeChairPos][1] - ((i == 0) ? (nDiceTakeCount) * 2 : 0);
		uint8_t nRemoveCount = __min(nValidCount, nTakeCount);

		//提取扑克
		nTakeCount -= nRemoveCount;
		m_heapinfo[nTakeChairPos][(i == 0) ? 1 : 0] += nRemoveCount;

		//完成判断
		if (0 == nTakeCount)
		{
			m_nHeapHead = nTakeChairPos;
			m_nHeapTail = (m_nBankerPos + nDiceTakeCount - 1) % GAME_PLAYER;
			break;
		}

		//切换索引
		nTakeChairPos = (nTakeChairPos + 1) % 4;
	}
}

void CMahjongTable::OutCardAssist(CMahjongPlayer* pPlayer)
{
	LOGGER(E_LOG_INFO) << "== out_card_assist == ";

	pPlayer->m_outCanTing.clear();
	for (BYTE i = 0; i < MAX_INDEX; i++)
	{
		pPlayer->m_outCanTingCard[i].clear();

		if (0 == pPlayer->m_handCardIndex[i])
			continue;

		pPlayer->m_handCardIndex[i]--;
		LOGGER(E_LOG_INFO) << "== if out card = " << TOHEX(m_pGameLogic->SwitchToCardData(i));
		for (BYTE j = 0; j < MAX_INDEX; j++)
		{
			if (j != i)
			{
				CChiHuRight chr;
				uint8_t cbCurrentCard = m_pGameLogic->SwitchToCardData(j);

				if (WIK_CHI_HU == m_pGameLogic->AnalyseChiHuCard(pPlayer, cbCurrentCard, chr, true))
				{
					tagListenItem	item;

					LOGGER(E_LOG_INFO) << "== AnalyseTingCard Success == ";
					item.cbListenCard = cbCurrentCard;
					item.cbCardLeft = GetLeftCount(pPlayer, cbCurrentCard);
					if (0 != item.cbCardLeft)
					{
						item.cbPoints = this->GetCommonPoints(pPlayer->m_handCardIndex, cbCurrentCard, chr);
						pPlayer->m_outCanTingCard[i].push_back(item);
					}
				}
			}
		}
		pPlayer->m_handCardIndex[i]++;

		if (0 != pPlayer->m_outCanTingCard[i].size())
		{
			pPlayer->m_outCanTing.push_back(i);
			pPlayer->m_action |= WIK_LISTEN;
		}
	}

	LOGGER(E_LOG_DEBUG) << "out_can_ting == " << pPlayer->m_outCanTing.size();
}

uint8_t CMahjongTable::GetLeftCount(CMahjongPlayer* pPlayer, uint8_t card)
{
	int			nCount = 0;

	ULOGGER(E_LOG_DEBUG, pPlayer->m_id) << "card == " << card;

	if (pPlayer)
	{
		uint8_t		index = m_pGameLogic->SwitchToCardIndex(card);

		nCount += pPlayer->m_handCardIndex[index];
		ULOGGER(E_LOG_DEBUG, pPlayer->m_id) << "hand card, count == " << nCount;
	}

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		uint8_t			nOperateCard = 0;

		for (uint8_t j = 0; j < m_players[i]->m_discardCount; j++)
		{
			if (card == m_players[i]->m_discardCard[j])
			{
				nCount++;
				ULOGGER(E_LOG_DEBUG, pPlayer->m_id) << "discard card, count == " << nCount;
			}
		}

		for (uint8_t j = 0; j < m_players[i]->m_weaveCount; j++)
		{
			nOperateCard = m_players[i]->m_weaveItems[j].cbCenterCard;
			switch (m_players[i]->m_weaveItems[j].cbWeaveKind)
			{
				case WIK_LEFT:
					if ((card == nOperateCard) || (card == (nOperateCard + 1)) || (card == (nOperateCard + 2)))
					{
						nCount++;
						ULOGGER(E_LOG_DEBUG, pPlayer->m_id) << "weave item == " << m_players[i]->m_weaveItems[j].cbWeaveKind << "count == " << nCount;
					}
					break;
				case WIK_CENTER:
					if ((card == (nOperateCard-1)) || (card == nOperateCard) || (card == (nOperateCard + 2)))
					{
						nCount++;
						ULOGGER(E_LOG_DEBUG, pPlayer->m_id) << "weave item == " << m_players[i]->m_weaveItems[j].cbWeaveKind << "count == " << nCount;
					}
					break;
				case WIK_RIGHT:
					if ((card == (nOperateCard - 2)) || (card == (nOperateCard - 1)) || (card == nOperateCard))
					{
						nCount++;
						ULOGGER(E_LOG_DEBUG, pPlayer->m_id) << "weave item == " << m_players[i]->m_weaveItems[j].cbWeaveKind << "count == " << nCount;
					}
					break;
				case WIK_PENG:
					if (card == nOperateCard)
					{
						nCount += 3;
						ULOGGER(E_LOG_DEBUG, pPlayer->m_id) << "weave item == " << m_players[i]->m_weaveItems[j].cbWeaveKind << " count == " << nCount;
					}
					break;
				case WIK_GANG:
					if (card == nOperateCard)
					{
						nCount += 4;
						ULOGGER(E_LOG_DEBUG, pPlayer->m_id) << "weave item == " << m_players[i]->m_weaveItems[j].cbWeaveKind << " count == " << nCount;
					}
					break;
				default:
					break;
			}
		}
	}

	ULOGGER(E_LOG_DEBUG, pPlayer->m_id) << "left count == " << (4 - nCount);
	return (4 - nCount);
}

void CMahjongTable::OutCard(CMahjongPlayer *pPlayer, uint8_t nOutCard, bool bListen)
{
	//删除麻将
	if (!m_pGameLogic->RemoveCard(pPlayer->m_handCardIndex, nOutCard))
	{
		ULOGGER(E_LOG_ERROR, pPlayer->m_id) << "remove card failed!";
		return;
	}

	//设置状态
	if (m_bQiangGang)
		m_bQiangGang = false;

	if (m_capabilitis&support_listen)
	{
		if ((!pPlayer->m_listen) && bListen)
			pPlayer->m_listen = bListen;
	}

	pPlayer->m_action = WIK_NULL;
	pPlayer->m_performAction = WIK_NULL;

	//出牌记录
	m_nOutCardCount++;
	m_nOutCardPos = pPlayer->m_nSeatPos;
	m_nOutCardData = nOutCard;
	m_nProvidePos = pPlayer->m_nSeatPos;
	m_nProvideData = nOutCard;

	//用户切换
	m_nCurrentPos = (pPlayer->m_nSeatPos + GAME_PLAYER - 1) % GAME_PLAYER;

	this->OnOutCardCompleted();
}

bool CMahjongTable::EstimateUserResponse(uint8_t nCurPos, uint8_t nCurCard, EstimatType eEstimatType)
{
	bool		bAction = false;
	uint8_t		nEatUser = (m_capabilitis&support_chi) ? (nCurPos + GAME_PLAYER - 1) % GAME_PLAYER : 0;

	for (uint8_t i = 0; i < GAME_PLAYER; i++)
	{
		if (m_players[i])
		{
			CMahjongPlayer* p = m_players[i];

			//用户状态
			p->m_response = false;
			p->m_action = WIK_NULL;
			p->m_performAction = WIK_NULL;

			if (nCurPos == p->m_nSeatPos)
				continue;

			if (estimat_out_card == eEstimatType)
			{
				//吃碰判断
				if (!p->m_forbidPeng)
				{
					//碰牌判断(听牌后不能碰)
					if (!p->m_listen)
						p->m_action |= m_pGameLogic->EstimatePengCard(p->m_handCardIndex, nCurCard);

					//杠牌判断
					if (m_nLeftCardCount > m_nEndLeftCount)
					{
						int8_t curAction = m_pGameLogic->EstimateGangCard(p->m_handCardIndex, nCurCard);
						
						if (WIK_GANG == curAction)
						{
							if (p->m_listen)
							{
								//复制数据
								BYTE			cardIndexTemp[MAX_INDEX] = { 0 };
								tagWeaveItem	tempWaveItems[MAX_WEAVE];
								uint8_t			tempWaveCount = 0;

								CopyMemory(cardIndexTemp, p->m_handCardIndex, g_nMaxIndex);
								cardIndexTemp[m_pGameLogic->SwitchToCardIndex(nCurCard)] = 0;

								ZeroMemory(tempWaveItems, sizeof(tempWaveItems));
								tempWaveCount = p->m_weaveCount;
								if (tempWaveCount > 0)
									CopyMemory(tempWaveItems, p->m_weaveItems, tempWaveCount * sizeof(tagWeaveItem));
								tempWaveItems[tempWaveCount].cbWeaveKind = WIK_GANG;
								tempWaveItems[tempWaveCount].cbPublicCard = true;
								tempWaveItems[tempWaveCount].cbCenterCard = nCurCard;
								tempWaveItems[tempWaveCount].wProvideUser = nCurCard;
								tempWaveCount++;

								if (m_pGameLogic->IsTingCard(cardIndexTemp, tempWaveItems, tempWaveCount))
									p->m_action |= WIK_GANG;
							}
							else
							{
								p->m_action |= WIK_GANG;
							}
						}
					}

					//吃牌判断(只能是出牌的下家能吃)
					if (m_capabilitis&support_chi)
					{
						if ((!p->m_listen) && (nEatUser == i))
							p->m_action |= m_pGameLogic->EstimateEatCard(p->m_handCardIndex, nCurCard);
					}
				}
			}

			//胡牌判断--2
			if (!p->m_forbidChihu)
			{
				//吃胡判断
				CChiHuRight chr;
				uint8_t nAction = m_pGameLogic->AnalyseChiHuCard(p, nCurCard, chr, false);
				p->m_action |= nAction;
			}

			//结果判断
			if (WIK_NULL != p->m_action)
				bAction = true;
		}
	}

	//结果处理
	if (bAction)
	{
		//设置变量
		m_nProvidePos = nCurPos;
		m_nProvideData = nCurCard;
		m_nResumePos = m_nCurrentPos;
		m_nCurrentPos = INVALID_CHAIR;
		m_bWaitForOperate = true;
		return true;
	}
	else
	{
		return false;
	}
}

bool CMahjongTable::DispatchCard(uint8_t uCurUser, bool bTail)
{
	if ((uCurUser >= m_nCurPlayerCount) || (INVALID_CHAIR == uCurUser))
	{
		LOGGER(E_LOG_ERROR) << "invalid winner pos = " << uCurUser;
		return false;
	}

	//弃牌
	if ((INVALID_CHAIR != m_nOutCardPos) && (0 != m_nOutCardData))
	{
		m_players[m_nOutCardPos]->m_discardCard[m_players[m_nOutCardPos]->m_discardCount] = m_nOutCardData;
		m_players[m_nOutCardPos]->m_discardCount++;
	}

	//荒庄结束
	if (m_nLeftCardCount == m_nEndLeftCount)
	{
		LOGGER(E_LOG_INFO) << "huangzhuang over!";
		m_nProvideData = INVALID_CHAIR;
		m_nChihuData = INVALID_CHAIR;
		GameOver();
		return true;
	}

	//设置变量
	CMahjongPlayer* p = m_players[uCurUser];
	m_bGangPao = false;
	m_nOutCardPos = INVALID_CHAIR;
	m_nOutCardData = 0; 
	m_nCurrentPos = uCurUser;
	p->m_forbidChihu = false;
	p->m_action = WIK_NULL;

	//发牌处理
	//发送扑克
	m_nSendCardCount++;
	m_nSendCardData = m_allCardData[--m_nLeftCardCount];

	//加牌
	p->m_handCardIndex[m_pGameLogic->SwitchToCardIndex(m_nSendCardData)]++;

	//设置变量
	m_nProvidePos = p->m_nSeatPos;
	m_nProvideData = m_nSendCardData;

	//堆立信息
	ASSERT((INVALID_CHAIR != m_nHeapHead) && (INVALID_CHAIR != m_nHeapTail));
	if (!bTail)
	{
		//切换索引
		BYTE cbHeapCount = m_heapinfo[m_nHeapHead][0] + m_heapinfo[m_nHeapHead][1];
		if (cbHeapCount == g_nHeapFullCount)
			m_nHeapHead = (m_nHeapHead + 1) % 4;
		
		m_heapinfo[m_nHeapHead][0]++;
	}
	else
	{
		//切换索引
		BYTE cbHeapCount = m_heapinfo[m_nHeapTail][0] + m_heapinfo[m_nHeapTail][1];
		if (cbHeapCount == g_nHeapFullCount)
			m_nHeapTail = (m_nHeapTail + 3) % 4;
	
		m_heapinfo[m_nHeapTail][1]++;
	}
	
	AnalyseBeforeOutCard(p);

	if (m_capabilitis&support_out_assist)
	{
		if (!p->m_listen)
			OutCardAssist(p);
	}

	this->OnDispatchCardCompleted(bTail);
	return true;
}

void CMahjongTable::GameOver()
{
	this->OnGameOver();
}

bool CMahjongTable::OperateCardPassively(CMahjongPlayer *pPlayer, uint8_t actionCard, uint8_t action)
{
	uint8_t		nTargetUser = INVALID_CHAIR;
	uint8_t		nTargetAction = WIK_NULL;
	uint8_t		nNewAction = WIK_NULL;

	ASSERT(pPlayer);
	if (pPlayer->m_response)
		return true;

	//效验状态
	if ((WIK_NULL != action) && (0 == (pPlayer->m_action & action)))
	{
		ULOGGER(E_LOG_ERROR, pPlayer->m_id) << "user action not valid!";
		return true;
	}

	//变量定义
	nTargetUser = pPlayer->m_nSeatPos;
	nTargetAction = action;
	nNewAction = WIK_NULL;

	//设置变量
	pPlayer->m_response = true;
	pPlayer->m_performAction = action;
	if (0 == actionCard)
		pPlayer->m_operateCard = m_nProvideData;
	else
		pPlayer->m_operateCard = actionCard;

	//在自己未摸下一张牌的一圈内,不能弃上一家而胡下一家
	if ((WIK_NULL == nTargetAction) && (WIK_CHI_HU&m_players[nTargetUser]->m_action) && (m_nProvidePos != nTargetUser))
		pPlayer->m_forbidChihu = true;

	//执行判断
	for (uint8_t i = 0; i < GAME_PLAYER; i++)
	{
		//获取动作
		uint8_t nUserAction = (!m_players[i]->m_response) ? m_players[i]->m_action : m_players[i]->m_performAction;

		//优先级别
		uint8_t nUserActionRank = m_pGameLogic->GetUserActionRank(nUserAction);
		uint8_t nTargetActionRank = m_pGameLogic->GetUserActionRank(nTargetAction);

		//动作判断
		if (nUserActionRank > nTargetActionRank)
		{
			nTargetUser = i;
			nTargetAction = nUserAction;
		}
	}

	if (!m_players[nTargetUser]->m_response)
		return true;

	//吃胡等待
	if (WIK_CHI_HU == nTargetAction)
	{
		for (uint8_t i = 0; i < GAME_PLAYER; i++)
		{
			if ((!m_players[i]->m_response) && (m_players[i]->m_action & WIK_CHI_HU))
				return true;
		}
	}

	//放弃操作
	if (WIK_NULL == nTargetAction)
	{
		//用户状态
		for (uint8_t i = 0; i < GAME_PLAYER; i++)
		{
			ASSERT(m_players[i]);
			m_players[i]->m_response = false;
			m_players[i]->m_action = WIK_NULL;
			m_players[i]->m_operateCard = 0;
			m_players[i]->m_performAction = WIK_NULL;
		}

		m_bGangPao = false;

		//给下家发送扑克
		DispatchCard(m_nResumePos, false);
		return true;
	}

	//变量定义
	uint8_t nTargetCard = m_players[nTargetUser]->m_operateCard;

	//出牌变量
	m_nOutCardData = 0;
	m_nOutCardPos = INVALID_CHAIR;
	m_players[nTargetUser]->m_forbidChihu = false;

	//胡牌操作
	if (WIK_CHI_HU == nTargetAction)
	{
		//结束信息
		m_nChihuData = nTargetCard;
		for (int i = (m_nProvidePos + GAME_PLAYER - 1) % GAME_PLAYER; i != m_nProvidePos; i = (i + GAME_PLAYER - 1) % GAME_PLAYER)
		{
			//过虑判断
			if (0 == (m_players[i]->m_performAction & WIK_CHI_HU))
				continue;

			//胡牌判断--最后胡了时候检查牌
			uint8_t weaveItemCount = m_players[i]->m_weaveCount;
			tagWeaveItem * pWeaveItem = m_players[i]->m_weaveItems;
			m_players[i]->m_chihuKind = m_pGameLogic->AnalyseChiHuCard(m_players[i], m_nChihuData, m_players[i]->m_chihuRight, true);

			//插入扑克
			if (WIK_NULL != m_players[i]->m_chihuKind)
			{
				m_players[i]->m_handCardIndex[m_pGameLogic->SwitchToCardIndex(m_nChihuData)]++;
				nTargetUser = i;
				this->OnProcessChihu(i);
				return true;
			}
		}
	}

	//组合扑克
	if (pPlayer->m_weaveCount >= MAX_WEAVE)
	{
		ULOGGER(E_LOG_ERROR, pPlayer->m_id) << "weave item count over MAX_WEAVE now, count = " << pPlayer->m_weaveCount;
		return false;
	}

	uint8_t nIndex = pPlayer->m_weaveCount++;
	pPlayer->m_weaveItems[nIndex].cbPublicCard = TRUE;
	pPlayer->m_weaveItems[nIndex].cbCenterCard = nTargetCard;
	pPlayer->m_weaveItems[nIndex].cbWeaveKind = nTargetAction;
	pPlayer->m_weaveItems[nIndex].wProvideUser = (INVALID_CHAIR == m_nProvidePos) ? nTargetUser : m_nProvidePos;

	//删除扑克
	uint8_t removeCard[4] = { 0 };
	uint8_t	removeCount = 0;
	switch (nTargetAction)
	{
		case WIK_LEFT:		//左吃
			removeCard[removeCount++] = nTargetCard + 1;
			removeCard[removeCount++] = nTargetCard + 2;
			break;
		case WIK_RIGHT:		//右吃
			removeCard[removeCount++] = nTargetCard - 2;
			removeCard[removeCount++] = nTargetCard - 1;
			break;
		case WIK_CENTER:	//中吃
			removeCard[removeCount++] = nTargetCard - 1;
			removeCard[removeCount++] = nTargetCard + 1;
			break;
		case WIK_PENG:		//碰牌操作
			removeCard[removeCount++] = nTargetCard;
			removeCard[removeCount++] = nTargetCard;
			break;
		case WIK_GANG:		//杠牌操作（被动动作只可能是其他玩家放杠）
			removeCard[removeCount++] = nTargetCard;
			removeCard[removeCount++] = nTargetCard;
			removeCard[removeCount++] = nTargetCard;
			break;
		default:
			ASSERT(FALSE);
			return false;
	}

	if (!m_pGameLogic->RemoveCard(pPlayer->m_handCardIndex, removeCard, removeCount))
	{
		ASSERT(FALSE);
		return false;
	}

	m_bGangPao = false;
	if ((m_nLeftCardCount > m_nEndLeftCount) && m_pGameLogic->IsGangCard(pPlayer->m_handCardIndex))
		nNewAction |= WIK_GANG;

	if (m_capabilitis&support_out_assist)
	{
		if (!pPlayer->m_listen)
		{
			OutCardAssist(pPlayer);
			if (0 != pPlayer->m_outCanTing.size())
				nNewAction |= WIK_LISTEN;
		}
	}

	this->OnOperateCardCompleted(pPlayer->m_nSeatPos, nTargetUser, nTargetAction, nTargetCard, nNewAction, false);
	
	// 杠牌后需要补牌
	if (WIK_GANG == nTargetAction)
	{
		m_bQiangGang = true;
		m_bGangPao = true;
		DispatchCard(nTargetUser, true);
	}

	m_nCurrentPos = nTargetUser;
	return true;
}

bool CMahjongTable::OperateCardActively(CMahjongPlayer *pPlayer, uint8_t actionCard, uint8_t action)
{
	uint8_t		nTargetUser = INVALID_CHAIR;
	uint8_t		nTargetAction = action;
	uint8_t		nNewAction = WIK_NULL;

	//效验操作
	if ((WIK_NULL == action) || (0 == (pPlayer->m_action & action)))
		return false;

	//扑克效验
	if ((WIK_NULL != action) && (WIK_CHI_HU != action) && (!m_pGameLogic->IsValidCard(actionCard)))
		return false;

	//设置变量
	m_players[m_nCurrentPos]->m_action = WIK_NULL;
	m_players[m_nCurrentPos]->m_performAction = WIK_NULL;
	m_players[m_nCurrentPos]->m_forbidChihu = false;

	bool bPublic = false;
	//执行动作
	switch (action)
	{
		case WIK_GANG:			//杠牌操作
		{
			//变量定义
			uint8_t nWeaveIndex = 0xFF;
			uint8_t nCardIndex = m_pGameLogic->SwitchToCardIndex(actionCard);

			//杠牌处理
			if (1 == pPlayer->m_handCardIndex[nCardIndex])
			{
				//寻找组合
				for (uint8_t i = 0; i < pPlayer->m_weaveCount; i++)
				{
					uint8_t nWeaveKind = pPlayer->m_weaveItems[i].cbWeaveKind;
					uint8_t nCenterCard = pPlayer->m_weaveItems[i].cbCenterCard;
					if ((nCenterCard == actionCard) && (WIK_PENG == nWeaveKind))
					{
						bPublic = true;
						nWeaveIndex = i;
						break;
					}
				}

				//效验动作
				ASSERT(0xFF != nWeaveIndex);
				if (0xFF == nWeaveIndex)
					return false;

				//组合扑克
				pPlayer->m_weaveItems[nWeaveIndex].cbPublicCard = TRUE;
				pPlayer->m_weaveItems[nWeaveIndex].cbWeaveKind = action;
				pPlayer->m_weaveItems[nWeaveIndex].cbCenterCard = actionCard;
				pPlayer->m_weaveItems[nWeaveIndex].wProvideUser = pPlayer->m_nSeatPos;
			}
			else
			{
				//扑克效验
				if (4 == pPlayer->m_handCardIndex[nCardIndex])
					return false;

				//设置变量
				bPublic = false;
				nWeaveIndex = pPlayer->m_weaveCount++;
				pPlayer->m_weaveItems[nWeaveIndex].cbPublicCard = FALSE;
				pPlayer->m_weaveItems[nWeaveIndex].wProvideUser = pPlayer->m_nSeatPos;
				pPlayer->m_weaveItems[nWeaveIndex].cbWeaveKind = action;
				pPlayer->m_weaveItems[nWeaveIndex].cbCenterCard = actionCard;
			}

			if ((m_nLeftCardCount > m_nEndLeftCount) && m_pGameLogic->IsGangCard(pPlayer->m_handCardIndex))
				nNewAction |= WIK_GANG;

			//删除扑克
			pPlayer->m_handCardIndex[nCardIndex] = 0;

			m_bQiangGang = true;
			this->OnOperateCardCompleted(pPlayer->m_nSeatPos, pPlayer->m_nSeatPos, action, actionCard, nNewAction, true);

			//效验动作
			bool bAroseAction = false;
			if (bPublic == true)
				bAroseAction = EstimateUserResponse(pPlayer->m_nSeatPos, actionCard, estimat_gang_card);

			//发送扑克
			if (!bAroseAction)
			{
				m_bGangPao = true;
				DispatchCard(pPlayer->m_nSeatPos, true);
			}

			return true;
		}
		case WIK_CHI_HU:		//吃胡操作
		{
			//普通胡牌
			if (!m_pGameLogic->RemoveCard(pPlayer->m_handCardIndex, &m_nProvideData, 1))
			{
				ASSERT(FALSE);
				return false;
			}

			pPlayer->m_chihuKind = m_pGameLogic->AnalyseChiHuCard(pPlayer, m_nProvideData, pPlayer->m_chihuRight, true);
			pPlayer->m_handCardIndex[m_pGameLogic->SwitchToCardIndex(m_nProvideData)]++;
			pPlayer->m_performAction = WIK_CHI_HU;

			//结束信息
			m_nChihuData = m_nProvideData;
			this->OnProcessChihu(pPlayer->m_nSeatPos);
			return true;
		}
	}
	return true;
}

void CMahjongTable::AnalyseBeforeOutCard(CMahjongPlayer* pPlayer)
{
	//胡牌判断
	CChiHuRight			chr;

	pPlayer->m_action |= m_pGameLogic->AnalyseChiHuCard(pPlayer, 0, chr, false);

	//杠牌判断
	if (!pPlayer->m_forbidPeng && (m_nLeftCardCount > m_nEndLeftCount))
	{
		tagGangCardResult gangCardResult;
		uint8_t curAction = m_pGameLogic->AnalyseGangCard(pPlayer, gangCardResult);

		if (WIK_GANG == curAction)
		{
			if (pPlayer->m_listen)
			{
				if (4 == pPlayer->m_handCardIndex[m_pGameLogic->SwitchToCardIndex(m_nSendCardData)])
				{
					BYTE			cardIndexTemp[MAX_INDEX] = { 0 };
					tagWeaveItem	tempWaveItems[MAX_WEAVE];
					uint8_t			tempWaveCount = 0;

					CopyMemory(cardIndexTemp, pPlayer->m_handCardIndex, g_nMaxIndex);
					cardIndexTemp[m_pGameLogic->SwitchToCardIndex(m_nSendCardData)] = 0;

					ZeroMemory(tempWaveItems, sizeof(tempWaveItems));
					tempWaveCount = pPlayer->m_weaveCount;
					if (tempWaveCount > 0)
						CopyMemory(tempWaveItems, pPlayer->m_weaveItems, tempWaveCount * sizeof(tagWeaveItem));
					tempWaveItems[tempWaveCount].cbWeaveKind = WIK_GANG;
					tempWaveItems[tempWaveCount].cbPublicCard = false;
					tempWaveItems[tempWaveCount].cbCenterCard = m_nSendCardData;
					tempWaveItems[tempWaveCount].wProvideUser = pPlayer->m_nSeatPos;
					tempWaveCount++;

					if (m_pGameLogic->IsTingCard(cardIndexTemp, tempWaveItems, tempWaveCount))
						pPlayer->m_action |= WIK_GANG;
				}
				else
				{
					pPlayer->m_action |= WIK_GANG;
				}
			}
			else
			{
				pPlayer->m_action |= WIK_GANG;
			}
		}
	}
}