#include "MahjongLogic.h"
#include "MahjongPlayer.h"

//静态变量
bool		CChiHuRight::m_bInit = false;
uint32_t	CChiHuRight::m_dwRightMask[MAX_RIGHT_COUNT];

//构造函数
CChiHuRight::CChiHuRight()
{
	ZeroMemory(m_dwRight, sizeof(m_dwRight));

	if (!m_bInit)
	{
		m_bInit = true;
		for (uint8_t i = 0; i < CountArray(m_dwRightMask); i++)
		{
			if (0 == i)
				m_dwRightMask[i] = 0;
			else
				m_dwRightMask[i] = (uint32_t(pow(2, i - 1))) << 28;
		}
	}
}

CChiHuRight::CChiHuRight(const CChiHuRight &other)
{
	for (uint32_t i = 0; i < MAX_RIGHT_COUNT; i++)
		m_dwRight[i] = other.m_dwRight[i];
}

CChiHuRight & CChiHuRight::operator = (const CChiHuRight &other)
{
	if (this != &other)
	{
		for (uint32_t i = 0; i < MAX_RIGHT_COUNT; i++)
			m_dwRight[i] = other.m_dwRight[i];
	}

	return *this;
}

//赋值符重载
CChiHuRight& CChiHuRight::operator = (uint32_t dwRight)
{
	uint32_t dwOtherRight = 0;
	//验证权位
	if (!IsValidRight(dwRight))
	{
		//验证取反权位
		ASSERT(IsValidRight(~dwRight));
		if (!IsValidRight(~dwRight)) return *this;
		dwRight = ~dwRight;
		dwOtherRight = MASK_CHI_HU_RIGHT;
	}

	for (uint8_t i = 0; i < CountArray(m_dwRightMask); i++)
	{
		if ((dwRight&m_dwRightMask[i]) || (i == 0 && dwRight<0x10000000))
			m_dwRight[i] = dwRight&MASK_CHI_HU_RIGHT;
		else m_dwRight[i] = dwOtherRight;
	}

	return *this;
}

//与等于
CChiHuRight & CChiHuRight::operator &= (uint32_t dwRight)
{
	bool bNavigate = false;
	//验证权位
	if (!IsValidRight(dwRight))
	{
		//验证取反权位
		ASSERT(IsValidRight(~dwRight));
		if (!IsValidRight(~dwRight)) 
			return *this;

		//调整权位
		uint32_t dwHeadRight = (~dwRight) & 0xF0000000;
		uint32_t dwTailRight = dwRight&MASK_CHI_HU_RIGHT;
		dwRight = dwHeadRight | dwTailRight;
		bNavigate = true;
	}

	for (uint8_t i = 0; i < CountArray(m_dwRightMask); i++)
	{
		if ((dwRight&m_dwRightMask[i]) || (i == 0 && dwRight<0x10000000))
		{
			m_dwRight[i] &= (dwRight&MASK_CHI_HU_RIGHT);
		}
		else if (!bNavigate)
			m_dwRight[i] = 0;
	}

	return *this;
}

//或等于
CChiHuRight & CChiHuRight::operator |= (uint32_t dwRight)
{
	//验证权位
	if (!IsValidRight(dwRight)) 
		return *this;

	for (uint8_t i = 0; i < CountArray(m_dwRightMask); i++)
	{
		if ((dwRight&m_dwRightMask[i]) || (i == 0 && dwRight<0x10000000))
			m_dwRight[i] |= (dwRight&MASK_CHI_HU_RIGHT);
	}

	return *this;
}

//与
CChiHuRight CChiHuRight::operator & (uint32_t dwRight)
{
	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//与
CChiHuRight CChiHuRight::operator & (uint32_t dwRight) const
{
	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//或
CChiHuRight CChiHuRight::operator | (uint32_t dwRight)
{
	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//或
CChiHuRight CChiHuRight::operator | (uint32_t dwRight) const
{
	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//是否权位为空
bool CChiHuRight::IsEmpty()
{
	for (uint8_t i = 0; i < CountArray(m_dwRight); i++)
	{	
		if (m_dwRight[i]) 
			return false;
	}
	return true;
}

//设置权位为空
void CChiHuRight::SetEmpty()
{
	ZeroMemory(m_dwRight, sizeof(m_dwRight));
	return;
}

//获取权位数值
uint8_t CChiHuRight::GetRightData(uint32_t dwRight[], uint8_t cbMaxCount)
{
	ASSERT(cbMaxCount >= CountArray(m_dwRight));
	if (cbMaxCount < CountArray(m_dwRight)) 
		return 0;

	CopyMemory(dwRight, m_dwRight, sizeof(uint32_t)*CountArray(m_dwRight));
	return CountArray(m_dwRight);
}

//设置权位数值
bool CChiHuRight::SetRightData(const uint32_t dwRight[], uint8_t cbRightCount)
{
	ASSERT(cbRightCount <= CountArray(m_dwRight));
	if (cbRightCount > CountArray(m_dwRight)) 
		return false;

	ZeroMemory(m_dwRight, sizeof(m_dwRight));
	CopyMemory(m_dwRight, dwRight, sizeof(uint32_t)*cbRightCount);

	return true;
}

//检查仅位是否正确
bool CChiHuRight::IsValidRight(uint32_t dwRight)
{
	uint32_t dwRightHead = dwRight & 0xF0000000;
	for (uint8_t i = 0; i < CountArray(m_dwRightMask); i++)
	{	
		if (m_dwRightMask[i] == dwRightHead) 
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CMahjongLogic::CMahjongLogic()
{
	uint8_t				nPos = 0;
	uint8_t				nIndex = 0;
	uint8_t				indexBegin[MASK_COLOR] = { (uint8_t)-1, (uint8_t)-1, (uint8_t)-1, (uint8_t)-1, (uint8_t)-1, (uint8_t)-1 };
	uint8_t				cardBegin[MASK_COLOR] = { 0x01, 0x11, 0x21, 0x31, 0x35, 0x41 };

	m_nMagicIndex = g_nMaxIndex;
	
	indexBegin[nPos] = nIndex;
#if USE_CARD_CHARACTER
	nIndex += 9;
#endif
	nPos++;

	indexBegin[nPos] = nIndex;
#if USE_CARD_BANBOO
	nIndex += 9;
#endif
	nPos++;
	
	indexBegin[nPos] = nIndex;
#if USE_CARD_DOT
	nIndex += 9;
#endif
	nPos++;

	m_honorIndex = nIndex;
	indexBegin[nPos] = nIndex;
#if USE_CARD_WIND
	nIndex += 3;
#endif
	nPos++;

	indexBegin[nPos] = nIndex;
#if USE_CARD_DRAGON
	nIndex += 4;
#endif
	nPos++;

	indexBegin[nPos] = nIndex;

	for (uint8_t i = 0; i < MAX_COLOR; i++)
	{
		m_cardIndexMap[0][i] = indexBegin[i];
		m_cardIndexMap[1][i] = cardBegin[i];
	}
}

//析构函数
CMahjongLogic::~CMahjongLogic()
{
}

//混乱扑克
void CMahjongLogic::RandCardData(uint8_t *pCardData, uint8_t nMaxCount)
{
	//混乱准备
	uint8_t cardDataTemp[FULL_COUNT] = { 0 };
	CopyMemory(cardDataTemp, g_repertoryCard, g_maxRepertory*sizeof(uint8_t));

	//混乱扑克
	uint8_t nRandCount = 0, nPosition = 0;
	do
	{
		nPosition = rand() % (nMaxCount - nRandCount);
		pCardData[nRandCount++] = cardDataTemp[nPosition];
		cardDataTemp[nPosition] = cardDataTemp[nMaxCount - nRandCount];
	} while (nRandCount < nMaxCount);

	return;
}

//删除扑克
bool CMahjongLogic::RemoveCard(uint8_t *pCardIndex, uint8_t nRemoveCard)
{
	//效验扑克
	ASSERT(IsValidCard(nRemoveCard));
	ASSERT(pCardIndex[this->SwitchToCardIndex(nRemoveCard)]>0);

	//删除扑克
	uint8_t cbRemoveIndex = this->SwitchToCardIndex(nRemoveCard);
	if (pCardIndex[cbRemoveIndex]>0)
	{
		pCardIndex[cbRemoveIndex]--;
		return true;
	}

	//失败效验
	ASSERT(false);
	return false;
}

//删除扑克
bool CMahjongLogic::RemoveCard(uint8_t *pCardIndex, const uint8_t* pRemoveCard, uint8_t nRemoveCount)
{
	//删除扑克
	for (uint8_t i = 0; i<nRemoveCount; i++)
	{
		//效验扑克
		ASSERT(IsValidCard(pRemoveCard[i]));
		ASSERT(pCardIndex[this->SwitchToCardIndex(pRemoveCard[i])]>0);

		//删除扑克
		uint8_t cbRemoveIndex = this->SwitchToCardIndex(pRemoveCard[i]);
		if (pCardIndex[cbRemoveIndex] == 0)
		{
			//错误断言
			ASSERT(false);

			//还原删除
			for (uint8_t j = 0; j<i; j++)
			{
				ASSERT(IsValidCard(pRemoveCard[j]));
				pCardIndex[this->SwitchToCardIndex(pRemoveCard[j])]++;
			}

			return false;
		}
		else
		{
			//删除扑克
			pCardIndex[cbRemoveIndex]--;
		}
	}

	return true;
}

//删除扑克
bool CMahjongLogic::RemoveCard(uint8_t* pCardData, uint8_t nCardCount, const uint8_t* pRemoveCard, uint8_t nRemoveCount)
{
	//检验数据
	ASSERT(nCardCount <= 14);
	ASSERT(nRemoveCount <= nCardCount);

	//定义变量
	uint8_t nDeleteCount = 0, pTempCardData[14];
	if (nCardCount > CountArray(pTempCardData))
		return false;

	CopyMemory(pTempCardData, pCardData, nCardCount * sizeof(pCardData[0]));

	//置零扑克
	for (uint8_t i = 0; i<nRemoveCount; i++)
	{
		for (uint8_t j = 0; j<nCardCount; j++)
		{
			if (pRemoveCard[i] == pTempCardData[j])
			{
				nDeleteCount++;
				pTempCardData[j] = 0;
				break;
			}
		}
	}

	//成功判断
	if (nDeleteCount != nRemoveCount)
	{
		ASSERT(false);
		return false;
	}

	//清理扑克
	uint8_t cbCardPos = 0;
	for (uint8_t i = 0; i<nCardCount; i++)
	{
		if (pTempCardData[i] != 0)
			pCardData[cbCardPos++] = pTempCardData[i];
	}

	return true;
}


//扑克转换
uint8_t CMahjongLogic::SwitchToCardData(uint8_t nCardIndex)
{
	ASSERT(nCardIndex < g_nMaxIndex);

	for (uint8_t i = 1; i < MAX_COLOR; i++)
	{
		if (nCardIndex < m_cardIndexMap[0][i])
			return m_cardIndexMap[1][i - 1] + (nCardIndex - m_cardIndexMap[0][i - 1]);
	}

	ASSERT(-1 != m_cardIndexMap[0][MAX_COLOR - 1]);
	return m_cardIndexMap[1][MAX_COLOR - 1] + (nCardIndex - m_cardIndexMap[0][MAX_COLOR - 1]);
}

//扑克转换
uint8_t CMahjongLogic::SwitchToCardIndex(uint8_t nCardData)
{
	ASSERT(IsValidCard(nCardData));
	
	for (uint8_t i = 1; i < MAX_COLOR; i++)
	{
		if (nCardData < m_cardIndexMap[1][i])
			return m_cardIndexMap[0][i - 1] + (nCardData - m_cardIndexMap[1][i - 1]);
	}

	return m_cardIndexMap[0][MAX_COLOR - 1] + (nCardData - m_cardIndexMap[1][MAX_COLOR - 1]);
}

//扑克转换
uint8_t CMahjongLogic::SwitchToCardData(const uint8_t* pCardIndex, uint8_t* pCardData)
{
	//转换扑克
	uint8_t nPosition = 0;
	//钻牌
	if (m_nMagicIndex != g_nMaxIndex)
	{
		for (uint8_t i = 0; i < pCardIndex[m_nMagicIndex]; i++)
			pCardData[nPosition++] = this->SwitchToCardData(m_nMagicIndex);
	}

	for (uint8_t i = 0; i < g_nMaxIndex; i++)
	{
		if (i == m_nMagicIndex) 
			continue;

		if (0 != pCardIndex[i])
		{
			for (uint8_t j = 0; j < pCardIndex[i]; j++)
			{
				ASSERT(nPosition < MAX_COUNT);
				pCardData[nPosition++] = this->SwitchToCardData(i);
			}
		}
	}

	return nPosition;
}

//扑克转换
uint8_t CMahjongLogic::SwitchToCardIndex(const uint8_t* pCardData, uint8_t nCardCount, uint8_t* pCardIndex)
{
	//设置变量
	ZeroMemory(pCardIndex, sizeof(uint8_t)*g_nMaxIndex);

	//转换扑克
	for (uint8_t i = 0; i < nCardCount; i++)
	{
		ASSERT(IsValidCard(pCardData[i]));
		pCardIndex[this->SwitchToCardIndex(pCardData[i])]++;
	}

	return nCardCount;
}

//有效判断
bool CMahjongLogic::IsValidCard(uint8_t cbCardData)
{
	uint8_t cbValue = (cbCardData&MASK_VALUE);
	uint8_t cbColor = (cbCardData&MASK_COLOR) >> 4;
	bool bValidCard = false;
	//基础牌是必然有的
	if ((cbValue >= 1) && (cbValue <= 9) && (cbColor <= 2))
	{
		bValidCard = true;
	}
#if USE_CARD_WIND // 风牌(东、南、西、北)
	if ((cbValue >= 1) && (cbValue <= 4) && (cbColor == 3))
	{
		bValidCard = true;
	}
#endif

#if USE_CARD_DRAGON// 箭牌(中、发、白)
	if ((cbValue >= 5) && (cbValue <= 7) && (cbColor == 3))
	{
		bValidCard = true;
	}
#endif

#if USE_CARD_FLOWER // 花牌（春夏秋冬，梅兰竹菊）
	if ((cbValue >= 1) && (cbValue <= 8) && (cbColor == 4))
	{
		bValidCard = true;
	}
#endif
	return bValidCard;
}

//扑克数目
uint8_t CMahjongLogic::GetCardCount(const uint8_t* pCardIndex)
{
	uint8_t nCardCount = 0;
	
	for (uint8_t i = 0; i<g_nMaxIndex; i++)
		nCardCount += pCardIndex[i];

	return nCardCount;
}

//获取组合
uint8_t CMahjongLogic::GetWeaveCard(uint8_t nWeaveKind, uint8_t nCenterCard, uint8_t cardBuffer[4])
{
	//组合扑克
	switch (nWeaveKind)
	{
		case WIK_LEFT:		//上牌操作
			cardBuffer[0] = nCenterCard;
			cardBuffer[1] = nCenterCard + 1;
			cardBuffer[2] = nCenterCard + 2;
			return 3;
		case WIK_RIGHT:		//上牌操作
			cardBuffer[0] = nCenterCard;
			cardBuffer[1] = nCenterCard - 1;
			cardBuffer[2] = nCenterCard - 2;
			return 3;
		case WIK_CENTER:	//上牌操作
			cardBuffer[0] = nCenterCard;
			cardBuffer[1] = nCenterCard - 1;
			cardBuffer[2] = nCenterCard + 1;
			return 3;
		case WIK_PENG:		//碰牌操作
			cardBuffer[0] = nCenterCard;
			cardBuffer[1] = nCenterCard;
			cardBuffer[2] = nCenterCard;
			return 3;
		case WIK_GANG:		//杠牌操作
			cardBuffer[0] = nCenterCard;
			cardBuffer[1] = nCenterCard;
			cardBuffer[2] = nCenterCard;
			cardBuffer[3] = nCenterCard;
			return 4;
		default:
			ASSERT(false);
	}

	return 0;
}

//动作等级
uint8_t CMahjongLogic::GetUserActionRank(uint8_t nUserAction)
{
	//胡牌等级
	if (nUserAction&WIK_CHI_HU) { return 4; }

	//杠牌等级
	if (nUserAction&WIK_GANG) { return 3; }

	//碰牌等级
	if (nUserAction&WIK_PENG) { return 2; }

	//上牌等级
	if (nUserAction&(WIK_RIGHT | WIK_CENTER | WIK_LEFT)) { return 1; }

	return 0;
}

//吃牌判断
uint8_t CMahjongLogic::EstimateEatCard(const uint8_t* pCardIndex, uint8_t nCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(nCurrentCard));

	//过滤判断
	if (nCurrentCard >= 0x31 || IsMagicCard(nCurrentCard))
		return WIK_NULL;

	//变量定义
	uint8_t excursion[3] = { 0, 1, 2 };
	uint8_t itemKind[3] = { WIK_LEFT, WIK_CENTER, WIK_RIGHT };
	uint8_t nItemCount = CountArray(itemKind);

	//吃牌判断
	uint8_t nEatKind = 0;
	uint8_t nFirstIndex = 0;
	uint8_t nCurrentIndex = SwitchToCardIndex(nCurrentCard);
	for (uint8_t i = 0; i < nItemCount; i++)
	{
		uint8_t cbValueIndex = nCurrentIndex % 9;
		if ((cbValueIndex >= excursion[i]) && ((cbValueIndex - excursion[i]) <= 6))
		{
			//吃牌判断
			nFirstIndex = nCurrentIndex - excursion[i];

			//吃牌不能包含有王霸
			if ((m_nMagicIndex != MAX_INDEX) && (m_nMagicIndex >= nFirstIndex) && (m_nMagicIndex <= nFirstIndex + 2))
				continue;

			if ((nCurrentIndex != nFirstIndex) && (pCardIndex[nFirstIndex] == 0))
				continue;

			if ((nCurrentIndex != (nFirstIndex + 1)) && (pCardIndex[nFirstIndex + 1] == 0))
				continue;

			if ((nCurrentIndex != (nFirstIndex + 2)) && (pCardIndex[nFirstIndex + 2] == 0))
				continue;

			//设置类型
			nEatKind |= itemKind[i];
		}
	}

	return nEatKind;
}

//碰牌判断
uint8_t CMahjongLogic::EstimatePengCard(const uint8_t* pCardIndex, uint8_t nCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(nCurrentCard));

	//过滤判断
	if (IsMagicCard(nCurrentCard))
		return WIK_NULL;

	//碰牌判断
	return (pCardIndex[this->SwitchToCardIndex(nCurrentCard)] >= 2) ? WIK_PENG : WIK_NULL;
}

//杠牌判断
uint8_t CMahjongLogic::EstimateGangCard(const uint8_t* pCardIndex, uint8_t nCurrentCard)
{
	//参数效验
	ASSERT(IsValidCard(nCurrentCard));

	//过滤判断
	if (IsMagicCard(nCurrentCard))
		return WIK_NULL;

	//杠牌判断
	return (pCardIndex[this->SwitchToCardIndex(nCurrentCard)] == 3) ? WIK_GANG : WIK_NULL;
}

//杠牌分析
uint8_t CMahjongLogic::AnalyseGangCard(CMahjongPlayer* pPlayer, tagGangCardResult &gangCardResult)
{
	ASSERT(pPlayer);

	//设置变量
	uint8_t nActionMask = WIK_NULL;

	ZeroMemory(&gangCardResult, sizeof(gangCardResult));

	//手上杠牌
	for (uint8_t i = 0; i<g_nMaxIndex; i++)
	{
		if (i == m_nMagicIndex) 
			continue;

		if (4 == pPlayer->m_handCardIndex[i])
		{
			nActionMask |= WIK_GANG;
			gangCardResult.cbCardData[gangCardResult.cbCardCount++] = this->SwitchToCardData(i);
		}
	}

	//组合杠牌
	for (uint8_t i = 0; i<pPlayer->m_weaveCount; i++)
	{
		if (WIK_PENG == pPlayer->m_weaveItems[i].cbWeaveKind)
		{
			if (1 == pPlayer->m_handCardIndex[this->SwitchToCardIndex(pPlayer->m_weaveItems[i].cbCenterCard)])
			{
				nActionMask |= WIK_GANG;
				gangCardResult.cbCardData[gangCardResult.cbCardCount++] = pPlayer->m_weaveItems[i].cbCenterCard;
			}
		}
	}

	return nActionMask;
}

//吃胡分析
uint8_t CMahjongLogic::AnalyseChiHuCard(CMahjongPlayer* pPlayer, uint8_t nCurrentCard, CChiHuRight &chiHuRight, bool bDetails)
{
	//变量定义
	uint8_t cbChiHuKind = WIK_NULL;
	uint8_t cardIndexTemp[MAX_INDEX] = { 0 };

	ASSERT(pPlayer);

	//构造扑克
	CopyMemory(cardIndexTemp, pPlayer->m_handCardIndex, g_nMaxIndex * sizeof(uint8_t));
	if (0 != nCurrentCard)
		cardIndexTemp[this->SwitchToCardIndex(nCurrentCard)]++;

	ASSERT(0 == ((GetCardCount(cardIndexTemp) - 2) % 3));

	chiHuRight.SetEmpty();
	if (IsChiHuCard(cardIndexTemp, pPlayer->m_weaveItems, pPlayer->m_weaveCount, chiHuRight, bDetails))
		cbChiHuKind = WIK_CHI_HU;
	
	return cbChiHuKind;
}

//听牌分析
uint8_t CMahjongLogic::AnalyseTingCard(CMahjongPlayer* pPlayer)
{
	uint8_t cbCardCount = GetCardCount(pPlayer->m_handCardIndex);
	CChiHuRight chr;

	if (0 == ((cbCardCount - 2) % 3))
	{
		//复制数据
		uint8_t cardIndexTemp[MAX_INDEX] = { 0 };
		CopyMemory(cardIndexTemp, pPlayer->m_handCardIndex, g_nMaxIndex * sizeof(uint8_t));

		for (uint8_t i = 0; i < g_nMaxIndex; i++)
		{
			if (cardIndexTemp[i] == 0)
				continue;
			
			cardIndexTemp[i]--;
			for (uint8_t j = 0; j < g_nMaxIndex; j++)
			{
				uint8_t cbCurrentCard = SwitchToCardData(j);
				if (WIK_CHI_HU == AnalyseChiHuCard(pPlayer, cbCurrentCard, chr, false))
					return WIK_LISTEN;
			}

			cardIndexTemp[i]++;
		}
	}
	else
	{
		for (uint8_t j = 0; j < g_nMaxIndex; j++)
		{
			uint8_t cbCurrentCard = SwitchToCardData(j);
			if (WIK_CHI_HU == AnalyseChiHuCard(pPlayer, cbCurrentCard, chr, false))
				return WIK_LISTEN;
		}
	}

	return WIK_NULL;
}

//是否听牌
bool CMahjongLogic::IsTingCard(const uint8_t* pCardIndex, const tagWeaveItem weaveItem[], uint8_t nWeaveCount)
{
	uint8_t			nCardCount = GetCardCount(pCardIndex);

	ASSERT(0 == (((nCardCount + 1) - 2) % 3));
	for (uint8_t i = 0; i < g_nMaxIndex; i++)
	{
		uint8_t cbCurrentCard = this->SwitchToCardData(i);
		CChiHuRight chiHuRight;

		if (IsChiHuCard(pCardIndex, weaveItem, nWeaveCount, chiHuRight, false))
			return true;
	}

	return false;
}

//是否有杠牌
bool CMahjongLogic::IsGangCard(const uint8_t* pCardIndex)
{
	for (uint8_t i = 0; i < g_nMaxIndex; i++)
	{
		if (4 == pCardIndex[i])
			return true;
	}

	return false;
}

//分析扑克
bool CMahjongLogic::AnalyseCard(const uint8_t* pCardIndex, const tagWeaveItem weaveItem[], uint8_t nItemCount, CAnalyseItemArray &analyseItemArray)
{
	//计算数目
	uint8_t nCardCount = GetCardCount(pCardIndex);

	//效验数目
	if ((nCardCount<2) || (nCardCount>MAX_COUNT) || ((nCardCount - 2) % 3 != 0))
		return false;

	//变量定义
	uint8_t nKindItemCount = 0;
	tagKindItem kindItem[27 * 2 + 7 + 14];
	ZeroMemory(kindItem, sizeof(kindItem));

	//需求判断
	uint8_t cbLessKindItem = (nCardCount - 2) / 3;
	ASSERT(4 == (cbLessKindItem + nItemCount));

	//单吊判断
	if (cbLessKindItem == 0)
	{
		//效验参数
		ASSERT((nCardCount == 2) && (nItemCount == 4));

		//牌眼判断
		for (uint8_t i = 0; i<g_nMaxIndex; i++)
		{
			if ((2 == pCardIndex[i]) || ((m_nMagicIndex != g_nMaxIndex) && (i != m_nMagicIndex) && (2 == (pCardIndex[m_nMagicIndex] + pCardIndex[i]))))
			{
				//变量定义
				tagAnalyseItem analyseItem;
				ZeroMemory(&analyseItem, sizeof(analyseItem));

				//设置结果
				for (uint8_t j = 0; j<nItemCount; j++)
				{
					analyseItem.cbWeaveKind[j] = weaveItem[j].cbWeaveKind;
					analyseItem.cbCenterCard[j] = weaveItem[j].cbCenterCard;
				}

				analyseItem.cbCardEye = this->SwitchToCardData(i);
				if ((pCardIndex[i] < 2) || (i == m_nMagicIndex))
					analyseItem.bMagicEye = true;
				else 
					analyseItem.bMagicEye = false;

				//插入结果
				analyseItemArray.Add(analyseItem);
				return true;
			}
		}

		return false;
	}

	//拆分分析
	uint8_t magicCardIndex[MAX_INDEX] = { 0 };
	uint8_t nMagicCardCount = 0;

	CopyMemory(magicCardIndex, pCardIndex, g_nMaxIndex*sizeof(uint8_t));
	if (m_nMagicIndex != g_nMaxIndex)
	{
		nMagicCardCount = pCardIndex[m_nMagicIndex];
		if (magicCardIndex[m_nMagicIndex])
			magicCardIndex[m_nMagicIndex] = 1;		//减小多余组合
	}

	if (nCardCount >= 3)
	{
		for (uint8_t i = 0; i<g_nMaxIndex; i++)
		{
			//同牌判断
			if (magicCardIndex[i] + nMagicCardCount >= 3)
			{
				ASSERT(nKindItemCount < CountArray(kindItem));
				kindItem[nKindItemCount].cbCardIndex[0] = i;
				kindItem[nKindItemCount].cbCardIndex[1] = i;
				kindItem[nKindItemCount].cbCardIndex[2] = i;
				kindItem[nKindItemCount].cbWeaveKind = WIK_PENG;
				kindItem[nKindItemCount].cbCenterCard = this->SwitchToCardData(i);
				kindItem[nKindItemCount].cbValidIndex[0] = magicCardIndex[i]>0 ? i : m_nMagicIndex;
				kindItem[nKindItemCount].cbValidIndex[1] = magicCardIndex[i]>1 ? i : m_nMagicIndex;
				kindItem[nKindItemCount].cbValidIndex[2] = magicCardIndex[i]>2 ? i : m_nMagicIndex;
				nKindItemCount++;

				if (magicCardIndex[i] + nMagicCardCount >= 6)
				{
					ASSERT(nKindItemCount < CountArray(kindItem));
					kindItem[nKindItemCount].cbCardIndex[0] = i;
					kindItem[nKindItemCount].cbCardIndex[1] = i;
					kindItem[nKindItemCount].cbCardIndex[2] = i;
					kindItem[nKindItemCount].cbWeaveKind = WIK_PENG;
					kindItem[nKindItemCount].cbCenterCard = this->SwitchToCardData(i);
					kindItem[nKindItemCount].cbValidIndex[0] = magicCardIndex[i]>3 ? i : m_nMagicIndex;
					kindItem[nKindItemCount].cbValidIndex[1] = m_nMagicIndex;
					kindItem[nKindItemCount].cbValidIndex[2] = m_nMagicIndex;
					nKindItemCount++;
				}
			}

			//连牌判断
			if ((i < m_honorIndex) && ((i % 9) < 7))
			{
				//只要财神牌数加上3个顺序索引的牌数大于等于3,则进行组合
				if (nMagicCardCount + magicCardIndex[i] + magicCardIndex[i + 1] + magicCardIndex[i + 2] >= 3)
				{
					uint8_t cbIndex[3] = { i == m_nMagicIndex ? (uint8_t)0 : magicCardIndex[i], (i + 1) == m_nMagicIndex ? (uint8_t)0 : magicCardIndex[i + 1],
						(i + 2) == m_nMagicIndex ? (uint8_t)0 : magicCardIndex[i + 2] };
					int nMagicCountTemp = nMagicCardCount;
					uint8_t cbValidIndex[3];
					while (nMagicCountTemp + cbIndex[0] + cbIndex[1] + cbIndex[2] >= 3)
					{
						for (uint8_t j = 0; j < CountArray(cbIndex); j++)
						{
							if (cbIndex[j] > 0)
							{
								cbIndex[j]--;
								cbValidIndex[j] = i + j;
							}
							else
							{
								nMagicCountTemp--;
								cbValidIndex[j] = m_nMagicIndex;
							}
						}

						if (nMagicCountTemp >= 0)
						{
							ASSERT(nKindItemCount < CountArray(kindItem));
							kindItem[nKindItemCount].cbCardIndex[0] = i;
							kindItem[nKindItemCount].cbCardIndex[1] = i + 1;
							kindItem[nKindItemCount].cbCardIndex[2] = i + 2;
							kindItem[nKindItemCount].cbWeaveKind = WIK_LEFT;
							kindItem[nKindItemCount].cbCenterCard = this->SwitchToCardData(i);
							CopyMemory(kindItem[nKindItemCount].cbValidIndex, cbValidIndex, sizeof(cbValidIndex));
							nKindItemCount++;
						}
						else 
						{
							break;
						}
					}
				}
			}
		}
	}

	//组合分析
	if (nKindItemCount >= cbLessKindItem)
	{
		//变量定义
		uint8_t cardIndexTemp[MAX_INDEX] = { 0 };
		
		//变量定义
		uint8_t cbIndex[4] = { 0,1,2,3 };
		tagKindItem * pKindItem[4];
		ZeroMemory(&pKindItem, sizeof(pKindItem));

		//开始组合
		do
		{
			//设置变量
			CopyMemory(cardIndexTemp, pCardIndex, g_nMaxIndex * sizeof(uint8_t));
			for (uint8_t i = 0; i<cbLessKindItem; i++)
				pKindItem[i] = &kindItem[cbIndex[i]];

			//数量判断
			bool bEnoughCard = true;
			for (uint8_t i = 0; i<cbLessKindItem * 3; i++)
			{
				//存在判断
				uint8_t nCardIndex = pKindItem[i / 3]->cbValidIndex[i % 3];
				if (0 == cardIndexTemp[nCardIndex])
				{
					bEnoughCard = false;
					break;
				}
				else
				{	
					cardIndexTemp[nCardIndex]--;
				}
			}

			//胡牌判断
			if (bEnoughCard == true)
			{
				//牌眼判断
				uint8_t cbCardEye = 0;
				bool bMagicEye = false;
				for (uint8_t i = 0; i<g_nMaxIndex; i++)
				{
					if (2 == cardIndexTemp[i])
					{
						cbCardEye = this->SwitchToCardData(i);
						if (i == m_nMagicIndex) 
							bMagicEye = true;
						break;
					}
					else 
					{
						if ((i != m_nMagicIndex) && (m_nMagicIndex != g_nMaxIndex) && (2 == (cardIndexTemp[i] + cardIndexTemp[m_nMagicIndex])))
						{
							cbCardEye = this->SwitchToCardData(i);
							bMagicEye = true;
						}
					}
				}

				//组合类型
				if (0 != cbCardEye)
				{
					//变量定义
					tagAnalyseItem analyseItem;
					ZeroMemory(&analyseItem, sizeof(analyseItem));

					//设置组合
					for (uint8_t i = 0; i<nItemCount; i++)
					{
						analyseItem.cbWeaveKind[i] = weaveItem[i].cbWeaveKind;
						analyseItem.cbCenterCard[i] = weaveItem[i].cbCenterCard;
					}

					//设置牌型
					for (uint8_t i = 0; i<cbLessKindItem; i++)
					{
						analyseItem.cbWeaveKind[i + nItemCount] = pKindItem[i]->cbWeaveKind;
						analyseItem.cbCenterCard[i + nItemCount] = pKindItem[i]->cbCenterCard;
					}

					//设置牌眼
					analyseItem.cbCardEye = cbCardEye;
					analyseItem.bMagicEye = bMagicEye;

					//插入结果
					analyseItemArray.Add(analyseItem);
				}
			}

			//设置索引
			if (cbIndex[cbLessKindItem - 1] == (nKindItemCount - 1))
			{
				uint8_t i = cbLessKindItem - 1;
				for (; i>0; i--)
				{
					if ((cbIndex[i - 1] + 1) != cbIndex[i])
					{
						uint8_t cbNewIndex = cbIndex[i - 1];
						for (uint8_t j = (i - 1); j<cbLessKindItem; j++)
							cbIndex[j] = cbNewIndex + j - i + 2;
						break;
					}
				}

				if (i == 0)
					break;
			}
			else
			{	
				cbIndex[cbLessKindItem - 1]++;
			}
		} while (true);
	}

	return (analyseItemArray.GetCount()>0);
}

//钻牌
bool CMahjongLogic::IsMagicCard(uint8_t nCardData)
{
	if (m_nMagicIndex != g_nMaxIndex)
		return SwitchToCardIndex(nCardData) == m_nMagicIndex;

	return false;
}

//排序,根据牌值排序
bool CMahjongLogic::SortCardList(uint8_t* pCardData, uint8_t nCardCount)
{
	//数目过虑
	if (nCardCount == 0 || nCardCount>MAX_COUNT) return false;

	//排序操作
	bool bSorted = true;
	uint8_t nSwitchData = 0, nLast = nCardCount - 1;
	do
	{
		bSorted = true;
		for (uint8_t i = 0; i<nLast; i++)
		{
			if (pCardData[i]>pCardData[i + 1])
			{
				//设置标志
				bSorted = false;

				//扑克数据
				nSwitchData = pCardData[i];
				pCardData[i] = pCardData[i + 1];
				pCardData[i + 1] = nSwitchData;
			}
		}
		nLast--;
	} while (bSorted == false);

	return true;
}

bool CMahjongLogic::IsChiHuCard(const uint8_t* pCardIndex, const tagWeaveItem weaveItem[], uint8_t nWeaveCount, CChiHuRight &chiHuRight, bool bDetails)
{
	chiHuRight.SetEmpty();
	if (this->AnalyseSpecialCard(pCardIndex, weaveItem, nWeaveCount, chiHuRight))
		return true;
	
	//分析扑克
	CAnalyseItemArray analyseItemArray;
	AnalyseCard(pCardIndex, weaveItem, nWeaveCount, analyseItemArray);

	int32_t nAnalyseCount = (int32_t)analyseItemArray.GetCount();
	if (nAnalyseCount > 0)
	{
		for (int32_t i = nAnalyseCount-1; i >=0; i--)
		{
			if (!this->IsValidCardType(pCardIndex, weaveItem, nWeaveCount, analyseItemArray.GetAt(1)))
				analyseItemArray.RemoveAt(i);
		}
	}

	nAnalyseCount = (int32_t)analyseItemArray.GetCount();
	if (nAnalyseCount > 0)
	{
		CChiHuRight			analyseRight;
		CChiHuRight			curRight;

		if (bDetails)
		{
			analyseRight = this->AnalyseDetailType(pCardIndex, weaveItem, nWeaveCount, analyseItemArray.GetAt(0));

			for (int32_t i = 1; i < nAnalyseCount; i++)
			{
				curRight = this->AnalyseDetailType(pCardIndex, weaveItem, nWeaveCount, analyseItemArray.GetAt(i));

				// add here, 胡牌的权重比较
				if (0 > CompareChiHuRight(analyseRight, curRight))
					analyseRight = curRight;
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool CMahjongLogic::IsPengPeng(const tagAnalyseItem *pAnalyseItem)
{
	for (BYTE i = 0; i < CountArray(pAnalyseItem->cbWeaveKind); i++)
	{
		if (pAnalyseItem->cbWeaveKind[i] & (WIK_LEFT | WIK_CENTER | WIK_RIGHT))
			return false;
	}
	return true;
}

//清一色牌
bool CMahjongLogic::IsQingYiSe(const uint8_t *pCardIndex, const tagWeaveItem weaveItem[], const uint8_t nItemCount, const uint8_t nCurrentCard)
{
	uint8_t nCardColor = 0xFF;

	for (BYTE i = 0; i < g_nMaxIndex; )
	{
		if (i == m_nMagicIndex)
		{
			i++;
			continue;
		}

		if (0 != pCardIndex[i])
		{
			//花色判断
			if (nCardColor != 0xFF)
				return false;

			//设置花色
			nCardColor = (SwitchToCardData(i)&MASK_COLOR);
			i = (i / 9 + 1) * 9 - 1;
		}
		else
		{
			i++;
		}
	}

	//如果手上只有王霸
	if (0xFF == nCardColor)
	{
		ASSERT((m_nMagicIndex != MAX_INDEX) && (pCardIndex[m_nMagicIndex] > 0));
		ASSERT(nItemCount > 0);
		nCardColor = weaveItem[0].cbCenterCard&MASK_COLOR;
	}

	if (0 != nCurrentCard)
	{
		if ((nCurrentCard&MASK_COLOR) != nCardColor && !IsMagicCard(nCurrentCard))
			return false;
	}

	//组合判断
	for (uint8_t i = 0; i < nItemCount; i++)
	{
		uint8_t cbCenterCard = weaveItem[i].cbCenterCard;
		if ((cbCenterCard&MASK_COLOR) != nCardColor)	
			return false;
	}

	return true;
}

//七小对牌
bool CMahjongLogic::IsQiXiaoDui(const uint8_t *pCardIndex, const tagWeaveItem weaveItem[], const uint8_t nWeaveCount, const uint8_t nCurrentCard)
{
	//组合判断
	if (0 != nWeaveCount) 
		return false;

	//单牌数目
	uint8_t nReplaceCount = 0;

	//临时数据
	uint8_t cardIndexTemp[MAX_INDEX] = { 0 };
	CopyMemory(cardIndexTemp, pCardIndex, g_nMaxIndex);

	//插入数据
	uint8_t currentIndex = this->SwitchToCardIndex(nCurrentCard);
	cardIndexTemp[currentIndex]++;

	//计算单牌
	for (uint8_t i = 0; i < MAX_INDEX; i++)
	{
		//王牌过滤
		if (i == m_nMagicIndex)
			continue;

		//单牌统计
		if ((1 == cardIndexTemp[i]) || (3 == cardIndexTemp[i]))
			nReplaceCount++;
	}

	//王牌不够
	if (((m_nMagicIndex != MAX_INDEX) && (nReplaceCount > cardIndexTemp[m_nMagicIndex])) ||
		((m_nMagicIndex == MAX_INDEX) && (nReplaceCount > 0)))
		return false;

	return true;
}
