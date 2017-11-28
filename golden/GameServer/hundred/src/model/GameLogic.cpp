
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <iostream>
#include <vector>
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//扑克数据
BYTE GameLogic::m_cbCardListData[52] =
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//黑桃 A - K
};

static std::vector<int> usedCards;

static int getUsedCount(BYTE value)
{
	int count = 0;
	BYTE v = GameLogic::GetCardLogicValue(value);
	for (auto it = usedCards.begin(); it != usedCards.end(); it++)
	{
		BYTE vv = GameLogic::GetCardLogicValue(GameLogic::CardList[*it].value);
		if (vv == v)
		{
			count++;
		}
	}
	return count;
}

static BYTE getCardByValue(int value)
{
	for (int i = 0; i < 52; i++)
	{
		Card& c = GameLogic::CardList[i];
		if (c.used)
		{
			continue;
		}
		if (GameLogic::GetCardLogicValue(c.value) == value)
		{
			c.used = true;
			return c.value;
		}
	}
	return 0;
}

static void backCard()
{
	for (int i = 0; i < 52; i++) //归还
	{
		GameLogic::CardList[i].used = false;
		for (auto it = usedCards.begin(); it != usedCards.end(); it++)
		{
			if (GameLogic::CardList[i].index == *it)
			{
				GameLogic::CardList[i].used = true;
			}
		}
	}
}

static void getRandCard(Card& c)
{
	int index = rand() % 52;
	while (GameLogic::CardList[index].used)
	{
		index = rand() % 52;
	}
	GameLogic::CardList[index].used = true;
	c = GameLogic::CardList[index];
}

Card GameLogic::CardList[52];

static std::string get_color_string(uint8_t color)
{
	switch (color)
	{
	case 0x00:
		return "方块";
	case 0x10:
		return "梅花";
	case 0x20:
		return "红桃";
	case 0x30:
		return "黑桃";
	}
	return "";
}

//////////////////////////////////////////////////////////////////////////

void GameLogic::initCardList()
{
	for (int i = 0; i < 52; i++)
	{
		CardList[i].value = GameLogic::m_cbCardListData[i];
		CardList[i].used = false;
		CardList[i].index = i;
	}
	usedCards.clear();
	std::vector<int> tmp;
	usedCards.swap(tmp);
}

//构造函数
GameLogic::GameLogic()
{
}

//析构函数
GameLogic::~GameLogic()
{
}

void GameLogic::RandSpecialCardList(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE ct)
{
	Card cards[3];
	getRandCard(cards[0]);	//抽第一张
	usedCards.push_back(cards[0].index); // record
	getRandCard(cards[1]);  //第二张
	BYTE v0 = GetCardLogicValue(cards[0].value);
	BYTE c0 = GetCardColor(cards[0].value);
	BYTE v1 = GetCardLogicValue(cards[1].value);
	BYTE c1 = GetCardColor(cards[1].value);
	if (ct == 0) //散牌
	{
		while (v1 == v0) //不允许出现对子
		{
			getRandCard(cards[1]);
			v1 = GetCardLogicValue(cards[1].value);
			c1 = GetCardColor(cards[1].value);
		}
		usedCards.push_back(cards[1].index); // record
		backCard();
		getRandCard(cards[2]);
		BYTE c2 = GetCardColor(cards[2].value);
		BYTE v2 = GetCardLogicValue(cards[2].value);
		while ((c2 == c1 && c2 == c0) || (v2 == v1) || (v2 == v0)) //不允许出现同花
		{
			getRandCard(cards[2]);
			c2 = GetCardColor(cards[2].value);
			v2 = GetCardLogicValue(cards[2].value);
		}
	}
	else if (ct == 5) //豹子
	{
		while (v1 != v0)
		{
			getRandCard(cards[1]);
			v1 = GetCardLogicValue(cards[1].value);
		}
		usedCards.push_back(cards[1].index); // record
		backCard();
		getRandCard(cards[2]);  //第三张
		BYTE v2 = GetCardLogicValue(cards[2].value);
		while (v2 != v0)
		{
			getRandCard(cards[2]);
			v2 = GetCardLogicValue(cards[2].value);
		}
	}
	else if (ct == 4) //顺金
	{
		while (c1 != c0 || (v1 != (v0 - 1) && v1 != (v0 + 1))) //花色相同，并且数值不同
		{
			getRandCard(cards[1]);
			c1 = GetCardColor(cards[1].value);
			v1 = GetCardLogicValue(cards[1].value);
		}
		usedCards.push_back(cards[1].index);
		backCard();
		getRandCard(cards[2]);  //第三张
		BYTE c2 = GetCardColor(cards[2].value);
		BYTE v2 = GetCardLogicValue(cards[2].value);
		while (c2 != c0 || ((v0 > v1 && v2 != v1 - 1 && v2 != v0 + 1) || (v1 > v0 && v2 != v1 + 1 && v2 != v0 - 1)))
		{
			getRandCard(cards[2]);
			c2 = GetCardColor(cards[2].value);
			v2 = GetCardLogicValue(cards[2].value);
		}
	}
	else if (ct == 3) //金花
	{
		while (c1 != c0)
		{
			getRandCard(cards[1]);
			c1 = GetCardColor(cards[1].value);
			v1 = GetCardLogicValue(cards[1].value);
		}
		usedCards.push_back(cards[1].index);
		backCard();
		getRandCard(cards[2]);  //第三张
		BYTE c2 = GetCardColor(cards[2].value);
		BYTE v2 = GetCardLogicValue(cards[2].value);
		while (c2 != c0 || ((v0 > v1 && (v2 == v1 - 1 || v2 == v0 + 1)) || (v1 > v0 && (v2 == v1 + 1 || v2 == v0 - 1))))
		{
			getRandCard(cards[2]);
			c2 = GetCardColor(cards[2].value);
			v2 = GetCardLogicValue(cards[2].value);
		}
	}
	else if (ct == 2) //顺子
	{
		while (v1 != v0 + 1 && v1 != v0 - 1)
		{
			getRandCard(cards[1]);
			c1 = GetCardColor(cards[1].value);
			v1 = GetCardLogicValue(cards[1].value);
		}
		usedCards.push_back(cards[1].index);
		backCard();
		getRandCard(cards[2]);  //第三张
		BYTE c2 = GetCardColor(cards[2].value);
		BYTE v2 = GetCardLogicValue(cards[2].value);
		while ((c2 == c0 && c2 == c1) || ((v0 > v1 && v2 != v1 - 1 && v2 != v0 + 1) || (v1 > v0 && v2 != v1 + 1 && v2 != v0 - 1)))
		{
			getRandCard(cards[2]);
			c2 = GetCardColor(cards[2].value);
			v2 = GetCardLogicValue(cards[2].value);
		}
	}
	else if (ct == 1) //对子
	{
		while (getUsedCount(cards[1].value) > 2) //如果已经被前面用过超过3张，则重新抽牌
		{
			getRandCard(cards[1]);
			v1 = GetCardLogicValue(cards[1].value);
			c1 = GetCardColor(cards[1].value);
		}
		usedCards.push_back(cards[1].index);
		backCard();
		getRandCard(cards[2]);  //第三张
		BYTE c2 = GetCardColor(cards[2].value);
		BYTE v2 = GetCardLogicValue(cards[2].value);
		while ((v1 == v0 && v2 == v0) || (v2 != v0 && v2 != v1 && v1 != v0))
		{
			getRandCard(cards[2]);
			c2 = GetCardColor(cards[2].value);
			v2 = GetCardLogicValue(cards[2].value);
		}
	}

	for (int i = 0; i < cbBufferCount; i++)
	{
		cbCardBuffer[i] = cards[i].value;
		//std::cout << (int)cards[i].value << std::endl;
	}
	usedCards.push_back(cards[2].index); // record
	backCard();
}

//获取类型
BYTE GameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount)
{

	if (cbCardCount == MAX_COUNT)
	{
		//变量定义
		bool cbSameColor = true, bLineCard = true;
		BYTE cbFirstColor = GetCardColor(cbCardData[0]);
		BYTE cbFirstValue = GetCardLogicValue(cbCardData[0]);

		//牌形分析
		for (BYTE i = 1; i<cbCardCount; i++)
		{
			//数据分析
			if (GetCardColor(cbCardData[i]) != cbFirstColor) cbSameColor = false;
			if (cbFirstValue != (GetCardLogicValue(cbCardData[i]) + i)) bLineCard = false;

			//结束判断
			if ((cbSameColor == false) && (bLineCard == false)) break;
		}

		//特殊A32
		if (!bLineCard)
		{
			bool bOne = false, bTwo = false, bThree = false;
			for (BYTE i = 0; i<MAX_COUNT; i++)
			{
				if (GetCardValue(cbCardData[i]) == 1)		bOne = true;
				else if (GetCardValue(cbCardData[i]) == 2)	bTwo = true;
				else if (GetCardValue(cbCardData[i]) == 3)	bThree = true;
			}
			if (bOne && bTwo && bThree)bLineCard = true;
		}

		//顺金类型
		if ((cbSameColor) && (bLineCard)) return CT_SHUN_JIN;

		//顺子类型
		if ((!cbSameColor) && (bLineCard)) return CT_SHUN_ZI;

		//金花类型
		if ((cbSameColor) && (!bLineCard)) return CT_JIN_HUA;

		//牌形分析
		bool bDouble = false, bPanther = true;

		//对牌分析
		for (BYTE i = 0; i<cbCardCount - 1; i++)
		{
			for (BYTE j = i + 1; j<cbCardCount; j++)
			{
				if (GetCardLogicValue(cbCardData[i]) == GetCardLogicValue(cbCardData[j]))
				{
					bDouble = true;
					break;
				}
			}
			if (bDouble)break;
		}

		//三条(豹子)分析
		for (BYTE i = 1; i<cbCardCount; i++)
		{
			if (bPanther && cbFirstValue != GetCardLogicValue(cbCardData[i])) bPanther = false;
		}

		//对子和豹子判断
		if (bDouble == true) return (bPanther) ? CT_BAO_ZI : CT_DOUBLE;

		//特殊235
		bool bTwo = false, bThree = false, bFive = false;
		for (BYTE i = 0; i<cbCardCount; i++)
		{
			if (GetCardValue(cbCardData[i]) == 2)	bTwo = true;
			else if (GetCardValue(cbCardData[i]) == 3)bThree = true;
			else if (GetCardValue(cbCardData[i]) == 5)bFive = true;
		}
		if (bTwo && bThree && bFive) return CT_SPECIAL;
	}

	return CT_SINGLE;
}

//排列扑克
void GameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//转换数值
	BYTE cbLogicValue[MAX_COUNT];
	for (BYTE i = 0; i<cbCardCount; i++) cbLogicValue[i] = GetCardLogicValue(cbCardData[i]);

	//排序操作
	bool bSorted = true;
	BYTE cbTempData, bLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i = 0; i<bLast; i++)
		{
			if ((cbLogicValue[i]<cbLogicValue[i + 1]) ||
				((cbLogicValue[i] == cbLogicValue[i + 1]) && (cbCardData[i]<cbCardData[i + 1])))
			{
				//交换位置
				cbTempData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbTempData;
				cbTempData = cbLogicValue[i];
				cbLogicValue[i] = cbLogicValue[i + 1];
				cbLogicValue[i + 1] = cbTempData;
				bSorted = false;
			}
		}
		bLast--;
	} while (bSorted == false);

	return;
}

//混乱扑克
void GameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//memcpy(cbCardBuffer,m_cbCardListData,cbBufferCount);

	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardListData)];
	memcpy(cbCardData, m_cbCardListData, sizeof(m_cbCardListData));

	//混乱扑克
	BYTE bRandCount = 0, bPosition = 0;
	do
	{
		bPosition = rand() % (CountArray(m_cbCardListData) - bRandCount);
		cbCardBuffer[bRandCount++] = cbCardData[bPosition];
		cbCardData[bPosition] = cbCardData[CountArray(m_cbCardListData) - bRandCount];
	} while (bRandCount<cbBufferCount);

	return;
}

void GameLogic::RandSpecialCard(BYTE cbCardBuffer[], BYTE cbBufferCount, short jackcardtype)
{
	printf("RandSpecialCard jackcardtype:%d \n", jackcardtype);
	if (jackcardtype <= 0)
	{
		RandCardList(cbCardBuffer, cbBufferCount);
		return;
	}

	if (jackcardtype <= CT_DOUBLE)
	{
		BYTE cbTypeCard[3] = { 0 };
		int bStackCount = 0;
		BYTE bRestCardList[52] = { 0 };
		int i = 0;
		int j = 0;
		memcpy(bRestCardList, m_cbCardListData, sizeof(m_cbCardListData));
		while (true)
		{
			if (bStackCount > 200)
				break;
			for (i = 51; i > 0; --i)
			{
				j = rand() % i;
				BYTE bTemp = bRestCardList[i];
				bRestCardList[i] = bRestCardList[j];
				bRestCardList[j] = bTemp;
			}
			memcpy(cbTypeCard, bRestCardList, 3);
			memcpy(cbCardBuffer, bRestCardList, cbBufferCount);
			bStackCount++;
			if (GetCardType(cbTypeCard, 3) == jackcardtype)
				break;
		}
		printf("bStackCount:%d \n", bStackCount);
		return;
	}

	if (jackcardtype == CT_SHUN_ZI)
	{
		RandCardSpecialShunZiType(cbCardBuffer, cbBufferCount);
		return;
	}

	if (jackcardtype == CT_JIN_HUA)
	{
		RandCardSpecialJinHuaType(cbCardBuffer, cbBufferCount);
		return;
	}

	if (jackcardtype == CT_SHUN_JIN)
	{
		RandCardSpecialShunJinType(cbCardBuffer, cbBufferCount);
		return;
	}
	if (jackcardtype == CT_BAO_ZI)
	{
		RandCardSpecialBaoZiType(cbCardBuffer, cbBufferCount);
		return;
	}

	if (jackcardtype == CT_SPECIAL)
	{
		RandCardSpecialDiLongType(cbCardBuffer, cbBufferCount);
		return;
	}

	//豹子A
	if (jackcardtype == 8)
	{
		RandCardSpecialBaoZiType(cbCardBuffer, cbBufferCount, 1);
		return;
	}

	RandCardList(cbCardBuffer, cbBufferCount);
	return;
}

/*//对子类型
void GameLogic::RandCardSpecialDoubleType(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
int bStackCount = 0;
BYTE doublevalue = 0;
BYTE cbTypeCard[4] = {0};
BYTE bRestCardList[48] = {0};

BYTE i = 0;
BYTE j = 0;
BYTE index = 0;
BYTE rindex = 0;
doublevalue = rand()%13 +1;
for (i = 0; i < 52; i++)
{
if (GetCardValue(m_cbCardListData[i]) == doublevalue)
cbTypeCard[index++] = m_cbCardListData[i];
else
bRestCardList[rindex++] = m_cbCardListData[i];
}

BYTE bTemp = 0;
for (i = 3; i > 0; i--)
{
index = rand()%i;
bTemp = cbTypeCard[index];
cbTypeCard[index] = cbTypeCard[i];
cbTypeCard[i] = bTemp;
}

index = rand()%48;
memcpy(cbCardBuffer, cbTypeCard, 2);
memcpy(cbCardBuffer+2, bRestCardList+index, 1);
for(i = 2; i > 0; --i)
{
index = rand()%i;
BYTE bTemp = cbCardBuffer[i];
cbCardBuffer[i] = cbCardBuffer[index];
cbCardBuffer[index] = bTemp;
}
}*/

//特殊类型、地龙
void GameLogic::RandCardSpecialDiLongType(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	BYTE bTemp = 0;
	BYTE index = 0;
	BYTE cbCardColor[4] = { 0x00, 0x10, 0x20, 0x30 };

	BYTE i = 0;
	BYTE j = 0;
	for (i = 3; i > 0; i--)
	{
		index = rand() % i;
		bTemp = cbCardColor[index];
		cbCardColor[index] = cbCardColor[i];
		cbCardColor[i] = bTemp;
	}

	cbCardBuffer[0] = cbCardColor[0] + 2;
	cbCardBuffer[1] = cbCardColor[1] + 3;
	cbCardBuffer[2] = cbCardColor[2] + 5;
	for (i = 2; i > 0; --i)
	{
		index = rand() % i;
		BYTE bTemp = cbCardBuffer[i];
		cbCardBuffer[i] = cbCardBuffer[index];
		cbCardBuffer[index] = bTemp;
	}
	index = 0;
	BYTE bRestCardList[49] = { 0 };
	for (i = 0; i < 52; i++)
	{
		if (m_cbCardListData[i] != cbCardBuffer[0] && m_cbCardListData[i] != cbCardBuffer[1]
			&& m_cbCardListData[i] != cbCardBuffer[2])
		{
			bRestCardList[index++] = m_cbCardListData[i];
		}
	}

	for (i = 48; i > 0; --i)
	{
		j = rand() % i;
		BYTE bTemp = bRestCardList[i];
		bRestCardList[i] = bRestCardList[j];
		bRestCardList[j] = bTemp;
	}
	memcpy(cbCardBuffer + 3, bRestCardList, cbBufferCount - 3);
}

//顺子类型
void GameLogic::RandCardSpecialShunZiType(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	int bStackCount = 0;
	BYTE colorvalue = 0;
	BYTE bRandvalue = 0;
	BYTE cbColor[4] = { 0x00, 0x10, 0x20, 0x30 };
	BYTE bColorCard[13] = { 0 };

	BYTE i = 0;
	BYTE j = 0;
	BYTE index = 0;
	colorvalue = rand() % 4;
	for (i = 0; i < 52; i++)
	{
		if (GetCardColor(m_cbCardListData[i]) == cbColor[colorvalue])
			bColorCard[index++] = m_cbCardListData[i];
	}

	BYTE bTemp = 0;
	bRandvalue = rand() % 12 + 1;
	BYTE bCardvalue = 0;
	while (true)
	{
		if (bStackCount > 20)
			break;

		bCardvalue = bRandvalue;
		for (i = 0; i < 3; i++)
		{
			index = rand() % 4;
			if (bCardvalue > 13)
				cbCardBuffer[i] = cbColor[index] + 1;
			else
				cbCardBuffer[i] = cbColor[index] + bCardvalue;

			bCardvalue += 1;
		}
		bStackCount++;
		BYTE bTempCardList[3] = { 0 };
		memcpy(bTempCardList, cbCardBuffer, 3);
		SortCardList(bTempCardList, 3);
		if (GetCardType(bTempCardList, 3) == CT_SHUN_ZI)
			break;
	}

	for (i = 2; i > 0; --i)
	{
		index = rand() % i;
		BYTE bTemp = cbCardBuffer[i];
		cbCardBuffer[i] = cbCardBuffer[index];
		cbCardBuffer[index] = bTemp;
	}

	index = 0;
	BYTE bRestCardList[49] = { 0 };
	for (i = 0; i < 52; i++)
	{
		if (m_cbCardListData[i] != cbCardBuffer[0] && m_cbCardListData[i] != cbCardBuffer[1]
			&& m_cbCardListData[i] != cbCardBuffer[2])
		{
			bRestCardList[index++] = m_cbCardListData[i];
		}
	}

	for (i = 48; i > 0; --i)
	{
		j = rand() % i;
		BYTE bTemp = bRestCardList[i];
		bRestCardList[i] = bRestCardList[j];
		bRestCardList[j] = bTemp;
	}
	memcpy(cbCardBuffer + 3, bRestCardList, cbBufferCount - 3);
}

//金花类型
void GameLogic::RandCardSpecialJinHuaType(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	int bStackCount = 0;
	BYTE colorvalue = 0;
	BYTE bRandvalue = 0;
	BYTE cbColor[4] = { 0x00, 0x10, 0x20, 0x30 };
	BYTE bColorCard[13] = { 0 };

	BYTE i = 0;
	BYTE j = 0;
	BYTE index = 0;
	colorvalue = rand() % 4;
	for (i = 0; i < 52; i++)
	{
		if (GetCardColor(m_cbCardListData[i]) == cbColor[colorvalue])
			bColorCard[index++] = m_cbCardListData[i];
	}

	BYTE bTemp = 0;
	while (true)
	{
		if (bStackCount > 30)
			break;

		for (i = 12; i > 0; i--)
		{
			index = rand() % i;
			bTemp = bColorCard[i];
			bColorCard[i] = bColorCard[index];
			bColorCard[index] = bTemp;
		}

		memcpy(cbCardBuffer, bColorCard, 3);
		bStackCount++;
		BYTE bTempCardList[3] = { 0 };
		memcpy(bTempCardList, cbCardBuffer, 3);
		SortCardList(bTempCardList, 3);
		if (GetCardType(cbCardBuffer, 3) == CT_JIN_HUA)
			break;
	}
	index = 0;
	BYTE bRestCardList[49] = { 0 };
	for (i = 0; i < 52; i++)
	{
		if (m_cbCardListData[i] != cbCardBuffer[0] && m_cbCardListData[i] != cbCardBuffer[1]
			&& m_cbCardListData[i] != cbCardBuffer[2])
		{
			bRestCardList[index++] = m_cbCardListData[i];
		}
	}

	for (i = 48; i > 0; --i)
	{
		j = rand() % i;
		BYTE bTemp = bRestCardList[i];
		bRestCardList[i] = bRestCardList[j];
		bRestCardList[j] = bTemp;
	}
	memcpy(cbCardBuffer + 3, bRestCardList, cbBufferCount - 3);
}

//顺金类型
void GameLogic::RandCardSpecialShunJinType(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	BYTE colorvalue = 0;
	BYTE bRandvalue = 0;
	BYTE cbColor[4] = { 0x00, 0x10, 0x20, 0x30 };
	BYTE bColorCard[13] = { 0 };

	BYTE i = 0;
	BYTE j = 0;
	BYTE index = 0;
	colorvalue = rand() % 4;
	for (i = 0; i < 52; i++)
	{
		if (GetCardColor(m_cbCardListData[i]) == cbColor[colorvalue])
			bColorCard[index++] = m_cbCardListData[i];
	}

	BYTE bTemp = 0;
	bRandvalue = rand() % 12 + 1;
	if (bRandvalue == 12)
	{
		memcpy(cbCardBuffer, bColorCard, 1);
		memcpy(cbCardBuffer + 1, bColorCard + 11, 2);
	}
	else
		memcpy(cbCardBuffer, bColorCard + (bRandvalue - 1), 3);

	for (i = 2; i > 0; --i)
	{
		index = rand() % i;
		BYTE bTemp = cbCardBuffer[i];
		cbCardBuffer[i] = cbCardBuffer[index];
		cbCardBuffer[index] = bTemp;
	}

	index = 0;
	BYTE bRestCardList[49] = { 0 };
	for (i = 0; i < 52; i++)
	{
		if (m_cbCardListData[i] != cbCardBuffer[0] && m_cbCardListData[i] != cbCardBuffer[1]
			&& m_cbCardListData[i] != cbCardBuffer[2])
		{
			bRestCardList[index++] = m_cbCardListData[i];
		}
	}

	for (i = 48; i > 0; --i)
	{
		j = rand() % i;
		BYTE bTemp = bRestCardList[i];
		bRestCardList[i] = bRestCardList[j];
		bRestCardList[j] = bTemp;
	}
	memcpy(cbCardBuffer + 3, bRestCardList, cbBufferCount - 3);
}

//豹子类型
void GameLogic::RandCardSpecialBaoZiType(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE CardValue)
{
	BYTE index = 0;;
	int bStackCount = 0;
	BYTE cardvalue = 0;
	BYTE cbTypeCard[4] = { 0 };

	if (CardValue == 0)
		cardvalue = rand() % 13 + 1;
	else
		cardvalue = CardValue;
	BYTE i = 0;
	BYTE j = 0;
	for (i = 0; i < 52; i++)
	{
		if (GetCardValue(m_cbCardListData[i]) == cardvalue)
			cbTypeCard[index++] = m_cbCardListData[i];
	}

	BYTE bTemp = 0;
	for (i = 3; i > 0; i--)
	{
		index = rand() % i;
		bTemp = cbTypeCard[index];
		cbTypeCard[index] = cbTypeCard[i];
		cbTypeCard[i] = bTemp;
	}

	memcpy(cbCardBuffer, cbTypeCard, 3);

	index = 0;
	BYTE bRestCardList[49] = { 0 };
	for (i = 0; i < 52; i++)
	{
		if (m_cbCardListData[i] != cbCardBuffer[0] && m_cbCardListData[i] != cbCardBuffer[1]
			&& m_cbCardListData[i] != cbCardBuffer[2])
		{
			bRestCardList[index++] = m_cbCardListData[i];
		}
	}

	for (i = 48; i > 0; --i)
	{
		j = rand() % i;
		BYTE bTemp = bRestCardList[i];
		bRestCardList[i] = bRestCardList[j];
		bRestCardList[j] = bTemp;
	}
	memcpy(cbCardBuffer + 3, bRestCardList, cbBufferCount - 3);
}

//获取倍数
BYTE GameLogic::GetTimes(BYTE cbCardData[], BYTE cbCardCount)
{
	if (cbCardCount != MAX_COUNT)return 0;

	BYTE cbTempCardData[MAX_COUNT];
	memcpy(cbTempCardData, cbCardData, sizeof(BYTE)*MAX_COUNT);
	SortCardList(cbTempCardData, MAX_COUNT);

	BYTE bType = GetCardType(cbTempCardData, MAX_COUNT);
	BYTE bTimes = 1;
	switch (bType)
	{
	case CT_SINGLE:				bTimes = 1; break;
	case CT_DOUBLE:				bTimes = 2; break;
	case CT_SHUN_ZI:			bTimes = 4; break;
	case CT_JIN_HUA:			bTimes = 6; break;
	case CT_SHUN_JIN:			bTimes = 8; break;
	case CT_BAO_ZI:				bTimes = 10; break;
	case CT_SPECIAL:			bTimes = 1; break;
	}

	return bTimes;
}

//逻辑数值
BYTE GameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	BYTE bCardColor = GetCardColor(cbCardData);
	BYTE bCardValue = GetCardValue(cbCardData);

	//转换数值
	return (bCardValue == 1) ? (bCardValue + 13) : bCardValue;
}

//对比扑克
BYTE GameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount)
{
	//设置变量
	BYTE FirstData[MAX_COUNT], NextData[MAX_COUNT];
	memcpy(FirstData, cbFirstData, sizeof(FirstData));
	memcpy(NextData, cbNextData, sizeof(NextData));

	//大小排序
	SortCardList(FirstData, cbCardCount);
	SortCardList(NextData, cbCardCount);

	//获取类型
	BYTE cbNextType = GetCardType(NextData, cbCardCount);
	BYTE cbFirstType = GetCardType(FirstData, cbCardCount);

	//特殊情况分析
	if ((cbNextType + cbFirstType) == (CT_SPECIAL + CT_BAO_ZI))return (BYTE)(cbFirstType>cbNextType);

	//还原单牌类型
	if (cbNextType == CT_SPECIAL)cbNextType = CT_SINGLE;
	if (cbFirstType == CT_SPECIAL)cbFirstType = CT_SINGLE;

	//类型判断
	if (cbFirstType != cbNextType) return (cbFirstType>cbNextType) ? 1 : 0;

	//简单类型
	switch (cbFirstType)
	{
	case CT_BAO_ZI:			//豹子
	case CT_SINGLE:			//单牌
	case CT_JIN_HUA:		//金花
	{
		//对比数值
		for (BYTE i = 0; i<cbCardCount; i++)
		{
			BYTE cbNextValue = GetCardLogicValue(NextData[i]);
			BYTE cbFirstValue = GetCardLogicValue(FirstData[i]);
			if (cbFirstValue != cbNextValue) return (cbFirstValue>cbNextValue) ? 1 : 0;
		}
		return DRAW;
	}
	case CT_SHUN_ZI:		//顺子
	case CT_SHUN_JIN:		//顺金 432>A32
	{
		BYTE cbNextValue = GetCardLogicValue(NextData[0]);
		BYTE cbFirstValue = GetCardLogicValue(FirstData[0]);

		//特殊A32
		if (cbNextValue == 14 && GetCardLogicValue(NextData[cbCardCount - 1]) == 2)
		{
			cbNextValue = 3;
		}
		if (cbFirstValue == 14 && GetCardLogicValue(FirstData[cbCardCount - 1]) == 2)
		{
			cbFirstValue = 3;
		}

		//对比数值
		if (cbFirstValue != cbNextValue) return (cbFirstValue>cbNextValue) ? 1 : 0;;
		return DRAW;
	}
	case CT_DOUBLE:			//对子
	{
		BYTE cbNextValue = GetCardLogicValue(NextData[0]);
		BYTE cbFirstValue = GetCardLogicValue(FirstData[0]);

		//查找对子/单牌
		BYTE bNextDouble = 0, bNextSingle = 0;
		BYTE bFirstDouble = 0, bFirstSingle = 0;
		if (cbNextValue == GetCardLogicValue(NextData[1]))
		{
			bNextDouble = cbNextValue;
			bNextSingle = GetCardLogicValue(NextData[cbCardCount - 1]);
		}
		else
		{
			bNextDouble = GetCardLogicValue(NextData[cbCardCount - 1]);
			bNextSingle = cbNextValue;
		}
		if (cbFirstValue == GetCardLogicValue(FirstData[1]))
		{
			bFirstDouble = cbFirstValue;
			bFirstSingle = GetCardLogicValue(FirstData[cbCardCount - 1]);
		}
		else
		{
			bFirstDouble = GetCardLogicValue(FirstData[cbCardCount - 1]);
			bFirstSingle = cbFirstValue;
		}

		if (bNextDouble != bFirstDouble)return (bFirstDouble>bNextDouble) ? 1 : 0;
		if (bNextSingle != bFirstSingle)return (bFirstSingle>bNextSingle) ? 1 : 0;
		return DRAW;
	}
	}

	return DRAW;
}

void GameLogic::CoppyCardList()
{
	memcpy(m_bAvailCardList, m_cbCardListData, sizeof(m_bAvailCardList[0]) * 52);
}

void GameLogic::SetAvailCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	for (BYTE i = 0; i<cbBufferCount; i++)
	{
		BYTE cardvalue = cbCardBuffer[i];
		int lie = cardvalue & 0xf0;
		int index = (lie >> 4) * 13 + (cardvalue & 0x0f) - 1;
		//printf("index:%d card:0x%02x\n", index, cardvalue);
		m_bAvailCardList[index] = 0;
	}
}

BYTE GameLogic::ChangeCard(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE cbCardValue)
{
	int index = 0;
	BYTE RetCardValue = m_bAvailCardList[rand() % 52];

	/*for(BYTE i = 0; i<52; i++)
	{
	printf("i:%d 0x%02x ", i, m_bAvailCardList[i]);
	}
	printf("\n");*/

	int RandNum = 0;
	while (true)
	{
		if (RandNum > 20)
			break;
		index = (rand() + RandNum * 10) % 52;
		if (m_bAvailCardList[index] > 0)
		{
			RetCardValue = m_bAvailCardList[index];
			break;
		}
		RandNum++;
	}
	return RetCardValue;
}

//////////////////////////////////////////////////////////////////////////
