
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "GameLogic.h"


//////////////////////////////////////////////////////////////////////////

//扑克数据
BYTE GameLogic::m_cbCardListData[52]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//黑桃 A - K
};

Card GameLogic::CardList[52];

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

static void backCard(int index)
{
	Card& c = GameLogic::CardList[index];
	c.used = false;
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

static void get10Card(const Card& c1, const Card& c2, Card& c3)
{
	for (int i = 0; i < 52; i++)
	{
		int index = i + c1.index;
		if (index >= 52)
		{
			index -= 52;
		}
		Card& c = GameLogic::CardList[index];
		if (c.used)
		{
			continue;
		}
		int sum = GameLogic::GetCardLogicValue(c1.value) + GameLogic::GetCardLogicValue(c2.value) + GameLogic::GetCardLogicValue(c.value);
		if (sum % 10 == 0)
		{
			c.used = true;
			c3 = c;
			break;
		}
	}
}

static void get10Card(const Card& c1, Card& c2)
{
	for (int i = 0; i < 52; i++)
	{
		int index = i + c1.index;
		if (index >= 52)
		{
			index -= 52;
		}
		Card& c = GameLogic::CardList[index];
		if (c.used)
		{
			continue;
		}
		int sum = GameLogic::GetCardLogicValue(c1.value) + GameLogic::GetCardLogicValue(c.value);
		if (sum % 10 == 0)
		{
			c.used = true;
			c2 = c;
			break;
		}
	}
}

static void getNot10CardByCardType(const Card& c3, BYTE ct, Card& c4)
{
	for (int i = 0; i < 52; i++)
	{
		int index = i + c3.index;
		if (index >= 52)
		{
			index -= 52;
		}
		Card& c = GameLogic::CardList[index];
		if (c.used)
		{
			continue;
		}
		int sum = GameLogic::GetCardLogicValue(c3.value) + GameLogic::GetCardLogicValue(c.value);
		if (sum % 10 == ct)
		{
			c.used = true;
			c4 = c;
			break;
		}
	}
}

static void getNot10Card(const Card& c1, const Card& c2, Card& c3)
{
	for (int i = 0; i < 52; i++)
	{
		int index = i + c2.index;
		if (index >= 52)
		{
			index -= 52;
		}
		Card& c = GameLogic::CardList[index];
		if (c.used)
		{
			continue;
		}
		int sum = GameLogic::GetCardLogicValue(c1.value) + GameLogic::GetCardLogicValue(c2.value) + GameLogic::GetCardLogicValue(c.value);
		if (sum % 10 != 0)
		{
			c.used = true;
			c3 = c;
			break;
		}
	}
}

static bool have10Card(const Card* arr, int count)
{
	if (count > 5 || count < 3)
	{
		return false;
	}
	if (count == 3)
	{
		int sum = 0;
		for (int i = 0; i < count; i++)
		{
			sum += GameLogic::GetCardLogicValue(arr[i].value);
		}
		if (sum % 10 == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (count == 4)
	{
		int sum[4] = { 0 };
		for (int i = 0; i < count; i++)
		{
			if (i != 0)
			{
				sum[0] += GameLogic::GetCardLogicValue(arr[i].value);
			}
			if (i != 1)
			{
				sum[1] += GameLogic::GetCardLogicValue(arr[i].value);
			}
			if (i != 2)
			{
				sum[2] += GameLogic::GetCardLogicValue(arr[i].value);
			}
			if (i != 3)
			{
				sum[3] += GameLogic::GetCardLogicValue(arr[i].value);
			}
		}
		bool have10 = false;
		for (int i = 0; i < count; i++)
		{
			if (sum[i] % 10 == 0)
			{
				have10 = true;
			}
		}
		return have10;
	}
	else
	{
		int sum[10] = { 0 };
		for (int i = 0; i < count; i++)
		{
			if (i != 0 && i != 1)
			{
				sum[0] += GameLogic::GetCardLogicValue(arr[i].value);
			}
			if (i != 0 && i != 2)
			{
				sum[1] += GameLogic::GetCardLogicValue(arr[i].value);
			}
			if (i != 0 && i != 3)
			{
				sum[2] += GameLogic::GetCardLogicValue(arr[i].value);
			}
			if (i != 0 && i != 4)
			{
				sum[3] += GameLogic::GetCardLogicValue(arr[i].value);
			}
			if (i != 1 && i != 2)
			{
				sum[4] += GameLogic::GetCardLogicValue(arr[i].value);
			}
			if (i != 1 && i != 3)
			{
				sum[5] += GameLogic::GetCardLogicValue(arr[i].value);
			}
			if (i != 1 && i != 4)
			{
				sum[6] += GameLogic::GetCardLogicValue(arr[i].value);
			}
			if (i != 2 && i != 3)
			{
				sum[7] += GameLogic::GetCardLogicValue(arr[i].value);
			}
			if (i != 2 && i != 4)
			{
				sum[8] += GameLogic::GetCardLogicValue(arr[i].value);
			}
			if (i != 3 && i != 4)
			{
				sum[9] += GameLogic::GetCardLogicValue(arr[i].value);
			}
		}
		bool have10 = false;
		for (int i = 0; i < 10; i++)
		{
			if (sum[i] % 10 == 0)
			{
				have10 = true;
			}
		}
		return have10;
	}
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
}

//构造函数
GameLogic::GameLogic()
{
	initCardList();
}

//析构函数
GameLogic::~GameLogic()
{
}

//获取类型
BYTE GameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount)
{
	//五小牛判断
	BYTE bCount = 0;
	BYTE bNum = 0;
	BYTE bTempCount[MAX_COUNT];
	BYTE bTempCardDate[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++)
	{
		bTempCount[i]=GetCardLogicValue(cbCardData[i]);
		bCount+=bTempCount[i];
	}

	for(BYTE i=0;i<cbCardCount;i++)
	{
		if(GetCardValue(cbCardData[i]) < 5)
		{
			bNum++;
		}
	}
	if(bNum == 5 && bCount <= 10) return OX_FIVESMALL;

	memcpy(bTempCardDate, cbCardData, cbCardCount);
	SortCardList(bTempCardDate,cbCardCount);

	BYTE bKingCount=0,bTenCount=0;
	for(BYTE i=0;i<cbCardCount;i++)
	{
		if(GetCardValue(bTempCardDate[i])>10)
		{
			bKingCount++;
		}
		else if(GetCardValue(bTempCardDate[i])==10)
		{
			bTenCount++;
		}
	}
	if(bKingCount==MAX_COUNT) return OX_FIVEKING;

	////炸弹牌型
	BYTE bSameCount = 0;
	BYTE bSecondValue = GetCardValue(bTempCardDate[MAX_COUNT/2]);//已排序若相同点数必以花色不同并列ABBBB/BBBBA
	for(BYTE i=0;i<cbCardCount;i++)
	{
		if(bSecondValue == GetCardValue(bTempCardDate[i]))
		{
			bSameCount++;
		}
	}
	if(bSameCount==4)return OX_FOUR_SAME;
	//else if(bKingCount==MAX_COUNT-1 && bTenCount==1) return OX_FOURKING;

	////葫芦牌型
	//if(bSameCount==3)
	//{
	//	if((bSecondValue!=GetCardValue(bTempCardDate[3]) && GetCardValue(bTempCardDate[3])==GetCardValue(bTempCardDate[4]))
	//	||(bSecondValue!=GetCardValue(bTempCardDate[1]) && GetCardValue(bTempCardDate[1])==GetCardValue(bTempCardDate[0])))
	//		return OX_THREE_SAME;
	//}

	BYTE bTemp[MAX_COUNT];
	BYTE bSum=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		bTemp[i]=GetCardLogicValue(cbCardData[i]);
		bSum+=bTemp[i];
	}

	for (BYTE i=0;i<cbCardCount-1;i++)
	{
		for (BYTE j=i+1;j<cbCardCount;j++)
		{
			if((bSum-bTemp[i]-bTemp[j])%10==0)	// 有牛
			{
				return ((bTemp[i]+bTemp[j])>10)?(bTemp[i]+bTemp[j]-10):(bTemp[i]+bTemp[j]);
			}
		}
	}

	return OX_VALUE0;
}

BYTE GameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount, BYTE& m, BYTE &n)
{
	//五小牛判断
	BYTE bCount = 0;
	BYTE bNum = 0;
	BYTE bTempCount[MAX_COUNT];
	BYTE bTempCardDate[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++)
	{
		bTempCount[i]=GetCardLogicValue(cbCardData[i]);
		bCount+=bTempCount[i];
	}

	for(BYTE i=0;i<cbCardCount;i++)
	{
		if(GetCardValue(cbCardData[i]) < 5)
		{
			bNum++;
		}
	}
	if(bNum == 5 && bCount <= 10) return OX_FIVESMALL;

	memcpy(bTempCardDate, cbCardData, cbCardCount);
	SortCardList(bTempCardDate,cbCardCount);

	BYTE bKingCount=0,bTenCount=0;
	for(BYTE i=0;i<cbCardCount;i++)
	{
		if(GetCardValue(bTempCardDate[i])>10)
		{
			bKingCount++;
		}
		else if(GetCardValue(bTempCardDate[i])==10)
		{
			bTenCount++;
		}
	}
	if(bKingCount==MAX_COUNT) return OX_FIVEKING;

	////炸弹牌型
	BYTE bSameCount = 0;
	BYTE bSecondValue = GetCardValue(bTempCardDate[MAX_COUNT/2]);
	for(BYTE i=0;i<cbCardCount;i++)
	{
		if(bSecondValue == GetCardValue(bTempCardDate[i]))
		{
			bSameCount++;
		}
	}
	if(bSameCount==4)return OX_FOUR_SAME;
	//else if(bKingCount==MAX_COUNT-1 && bTenCount==1) return OX_FOURKING;

	////葫芦牌型
	//if(bSameCount==3)
	//{
	//	if((bSecondValue!=GetCardValue(bTempCardDate[3]) && GetCardValue(bTempCardDate[3])==GetCardValue(bTempCardDate[4]))
	//	||(bSecondValue!=GetCardValue(bTempCardDate[1]) && GetCardValue(bTempCardDate[1])==GetCardValue(bTempCardDate[0])))
	//		return OX_THREE_SAME;
	//}

	BYTE bTemp[MAX_COUNT];
	BYTE bSum=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		bTemp[i]=GetCardLogicValue(cbCardData[i]);
		bSum+=bTemp[i];
	}

	for (BYTE i=0;i<cbCardCount-1;i++)
	{
		for (BYTE j=i+1;j<cbCardCount;j++)
		{
			if((bSum-bTemp[i]-bTemp[j])%10==0)
			{
				m = i;
				n = j;
				return ((bTemp[i]+bTemp[j])>10)?(bTemp[i]+bTemp[j]-10):(bTemp[i]+bTemp[j]);
			}
		}
	}

	return OX_VALUE0;
}

//获取倍数
BYTE GameLogic::GetTimes(BYTE cbCardData[], BYTE cbCardCount)
{
	if(cbCardCount!=MAX_COUNT)return 0;

	BYTE bTimes=GetCardType(cbCardData,MAX_COUNT);
	/*if(bTimes<8)return 1;
	else if(bTimes==8)return 2;
	else if(bTimes==9)return 2;
	else if(bTimes==10)return 3;
	//else if(bTimes==OX_THREE_SAME)return 5;
	else if(bTimes==OX_FOUR_SAME)return 4;
	//else if(bTimes==OX_FOURKING)return 5;
	else if(bTimes==OX_FIVEKING)return 5;
	else if(bTimes==OX_FIVESMALL)return 8;*/

	//if(bTimes == 0)
	//	return 1;
	//if(bTimes <= 10)
	//	return bTimes;
	//else
	//	return 10;

	if (bTimes < 10) return 2;	// 无牛 ~ 牛9
	else return 3;
}

//获取牛牛
bool GameLogic::GetOxCard(BYTE cbCardData[], BYTE cbCardCount)
{
	//设置变量
	BYTE bTemp[MAX_COUNT],bTempData[MAX_COUNT];
	memcpy(bTempData,cbCardData,sizeof(bTempData));
	BYTE bSum=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		bTemp[i]=GetCardLogicValue(cbCardData[i]);
		bSum+=bTemp[i];
	}

	//查找牛牛
	for (BYTE i=0;i<cbCardCount-1;i++)
	{
		for (BYTE j=i+1;j<cbCardCount;j++)
		{
			if((bSum-bTemp[i]-bTemp[j])%10==0)
			{
				BYTE bCount=0;
				for (BYTE k=0;k<cbCardCount;k++)
				{
					if(k!=i && k!=j)
					{
						cbCardData[bCount++] = bTempData[k];
					}
				}

				cbCardData[bCount++] = bTempData[i];
				cbCardData[bCount++] = bTempData[j];

				return true;
			}
		}
	}

	return false;
}

//获取整数
bool GameLogic::IsIntValue(BYTE cbCardData[], BYTE cbCardCount)
{
	BYTE sum=0;
	for(BYTE i=0;i<cbCardCount;i++)
	{
		sum+=GetCardLogicValue(cbCardData[i]);
	}
	return (sum%10==0);
}

//排列扑克
void GameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//转换数值
	BYTE cbLogicValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbLogicValue[i]=GetCardValue(cbCardData[i]);	

	//排序操作 //TODO: 使用冒泡是否可以优化
	bool bSorted=true;
	BYTE cbTempData,bLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			//点数相同则按花色排序
			if ((cbLogicValue[i]<cbLogicValue[i+1])||
				((cbLogicValue[i]==cbLogicValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//交换位置
				cbTempData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbTempData;

				cbTempData=cbLogicValue[i];
				cbLogicValue[i]=cbLogicValue[i+1];
				cbLogicValue[i+1]=cbTempData;

				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);

	return;
}

//混乱扑克
void GameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//memcpy(cbCardBuffer,m_cbCardListData,cbBufferCount);
	//混乱准备
	BYTE cbCardData[CountArray(m_cbCardListData)];
	memcpy(cbCardData,m_cbCardListData,sizeof(m_cbCardListData));

	//混乱扑克
	BYTE bRandCount=0,bPosition=0;
	do
	{
		bPosition=rand()%(CountArray(m_cbCardListData)-bRandCount);
		cbCardBuffer[bRandCount++]=cbCardData[bPosition];
		cbCardData[bPosition]=cbCardData[CountArray(m_cbCardListData)-bRandCount];//相同随机数不同的花/点
	} while (bRandCount<cbBufferCount);

	return;
}

//逻辑数值
BYTE GameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//扑克属性
	//BYTE bCardColor=GetCardColor(cbCardData);
	BYTE bCardValue=GetCardValue(cbCardData);

	//转换数值
	return (bCardValue>10)?(10):bCardValue;
}

//对比扑克
bool GameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount,bool FirstOX,bool NextOX, bool bSpecial)
{
	if(FirstOX!=NextOX)return (FirstOX>NextOX);

	//特殊处理当两个人都没有牛(用于私人房)，cbNextData大于cbFirstData
	if(bSpecial)
	{
		if(NextOX == 0 && FirstOX== 0)
			return false;
	}
	//比较牛大小
	if(FirstOX==true)
	{
		//获取点数
		BYTE cbNextType=GetCardType(cbNextData,cbCardCount);
		BYTE cbFirstType=GetCardType(cbFirstData,cbCardCount);

		//点数判断
		if (cbFirstType!=cbNextType) return (cbFirstType>cbNextType);

		switch(cbNextType)
		{
		case OX_FOUR_SAME:		//炸弹牌型	
			{
				//排序大小
				BYTE bFirstTemp[MAX_COUNT],bNextTemp[MAX_COUNT];
				memcpy(bFirstTemp,cbFirstData,cbCardCount);
				memcpy(bNextTemp,cbNextData,cbCardCount);
				SortCardList(bFirstTemp,cbCardCount);
				SortCardList(bNextTemp,cbCardCount);

				return GetCardValue(bFirstTemp[MAX_COUNT/2])>GetCardValue(bNextTemp[MAX_COUNT/2]);

				break;
			}
		}
	}

	//排序大小
	BYTE bFirstTemp[MAX_COUNT],bNextTemp[MAX_COUNT];
	memcpy(bFirstTemp,cbFirstData,cbCardCount);
	memcpy(bNextTemp,cbNextData,cbCardCount);
	SortCardList(bFirstTemp,cbCardCount);
	SortCardList(bNextTemp,cbCardCount);

	//比较数值
	BYTE cbNextMaxValue=GetCardValue(bNextTemp[0]);
	BYTE cbFirstMaxValue=GetCardValue(bFirstTemp[0]);
	if(cbNextMaxValue!=cbFirstMaxValue)return cbFirstMaxValue>cbNextMaxValue;

	//比较颜色
	return GetCardColor(bFirstTemp[0]) > GetCardColor(bNextTemp[0]);

	return false;
}

//////////////////////////////////////////////////////////////////////////


//混乱特定扑克
void GameLogic::RandCardSpecialType(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE bType)
{
	if(bType > OX_FIVESMALL || bType == 0)
	{
		RandCardList(cbCardBuffer, cbBufferCount);
		return ;
	}
	if(bType <= 10)
	{
		BYTE cbTypeCard[5] = {0};
		int bStackCount = 0;
		BYTE bRestCardList[52] = {0};
		int i = 0;
		int j = 0;
		memcpy(bRestCardList,m_cbCardListData,sizeof(m_cbCardListData));
		while (true)
		{
			if(bStackCount > 100)
				break;
			for(i = 51; i > 0; --i)
			{
				j = rand()%i;
				BYTE bTemp = bRestCardList[i];
				bRestCardList[i] = bRestCardList[j];
				bRestCardList[j] = bTemp;
			}
			memcpy(cbTypeCard, bRestCardList, 5);
			memcpy(cbCardBuffer, bRestCardList, cbBufferCount);		
			bStackCount++;
			if(GetCardType(cbTypeCard,5) >= bType)
				break;
		}
		return ;
	}
	else if(bType == OX_FOUR_SAME)
	{
		RandCardSpecialFourType(cbCardBuffer, cbBufferCount);	
		return;
	}
	else if (bType == OX_FIVEKING)
	{
		RandCardSpecialKingType(cbCardBuffer, cbBufferCount);	
		return;
	}
	else if (bType == OX_FIVESMALL)
	{
		RandCardSpecialSmallType(cbCardBuffer, cbBufferCount);	
		return;
	}
	RandCardList(cbCardBuffer, cbBufferCount);
}

void GameLogic::RandSpecialCardList(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE ct)
{
	int count = 0;
	int sum = 0;
	Card cards[5];
	if (ct == 0) //无牛
	{
		for (int j = 0; j < cbBufferCount - 3; j++) //先抽2张
		{
			getRandCard(cards[j]);
			int value = GetCardLogicValue(cards[j].value);
			sum += value;
		}
		getNot10Card(cards[0], cards[1], cards[2]); //获取新的第三张
		getRandCard(cards[3]);  //获取第4张
		while (have10Card(cards, 4))
		{
			//int record = cards[3].index;
			getRandCard(cards[3]);
			//backCard(record);		//无需归还，因为这张牌能组成10的倍数
		}

		getRandCard(cards[4]);  //获取第5张
		while (have10Card(cards, cbBufferCount))
		{
			//int record = cards[4].index;
			getRandCard(cards[4]);
			//backCard(record);		//无需归还，因为这张牌能组成10的倍数
		}
	}
	else
	{
		for (int j = 0; j < cbBufferCount - 2; j++) //先抽3张
		{
			getRandCard(cards[j]);
			int value = GetCardLogicValue(cards[j].value);
			sum += value;
		}
		if (sum % 10 != 0)
		{
			int record = cards[2].index;				//记录第三张
			get10Card(cards[0], cards[1], cards[2]);	//获取新的第三张
			backCard(record);				//返还先前的第三张
		}
		getRandCard(cards[3]);
		if (ct == 10)	//牛牛
		{
			get10Card(cards[3], cards[4]);
		}
		else  //有牛
		{
			getNot10CardByCardType(cards[3], ct, cards[4]);
		}
	}

	for (int i = 0; i < cbBufferCount; i++)
	{
		cbCardBuffer[i] = cards[i].value;
	}
}

void GameLogic::RandCardSpecialFourType(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	BYTE cbTypeCard[5] = {0};
	int bStackCount = 0;
	BYTE bRestCardList[48] = {0};

	int fournum = rand()%13 + 1;
	int i = 0;
	int j = 0;
	int index = 0;
	for(i = 0; i < 52; i++)
	{
		if(GetCardValue(m_cbCardListData[i]) == fournum)
		{
			cbTypeCard[index++] = m_cbCardListData[i];
		}
		else
		{
			bRestCardList[j++] = m_cbCardListData[i];
		}
	}

	while (true)
	{
		if(bStackCount > 10)
			break;
		for(i = 47; i > 0; --i)
		{
			j = rand()%i;
			BYTE bTemp = bRestCardList[i];
			bRestCardList[i] = bRestCardList[j];
			bRestCardList[j] = bTemp;
		}
		memcpy(cbTypeCard + 4, bRestCardList, 1);
		memcpy(cbCardBuffer, cbTypeCard, 5);
		memcpy(cbCardBuffer + 5, bRestCardList, cbBufferCount - 5);		
		bStackCount++;
		if(GetCardType(cbTypeCard,5) >= OX_FOUR_SAME)
			break;
	}
	for(i = 4; i > 0; --i)
	{
		j = rand()%i;
		BYTE bTemp = cbCardBuffer[i];
		cbCardBuffer[i] = cbCardBuffer[j];
		cbCardBuffer[j] = bTemp;
	}
}

void GameLogic::RandCardSpecialKingType(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	BYTE cbTypeCard[5] = {0};
	BYTE cbCardArray[12];
	int bStackCount = 0;
	BYTE bRestCardList[47] = {0};

	int i = 0;
	int j = 0;
	int index = 0;
	for(i = 0; i < 52; i++)
	{
		if(GetCardValue(m_cbCardListData[i]) > 10)
		{
			cbCardArray[index++] = m_cbCardListData[i];
		}
	}

	for(i = 11; i > 0; --i)
	{
		j = rand()%i;
		BYTE bTemp = cbCardArray[i];
		cbCardArray[i] = cbCardArray[j];
		cbCardArray[j] = bTemp;
	}

	memcpy(cbTypeCard, cbCardArray, 5);
	index = 0;
	for(i = 0; i < 52; i++)
	{
		if(m_cbCardListData[i] != cbTypeCard[0] && m_cbCardListData[i] != cbTypeCard[1]
			&& m_cbCardListData[i] != cbTypeCard[2] && m_cbCardListData[i] != cbTypeCard[3]
			&& m_cbCardListData[i] != cbTypeCard[4])
		{
			bRestCardList[index++] = m_cbCardListData[i];
		}
	}

	while (true)
	{
		if(bStackCount > 5)
			break;
		for(i = 46; i > 0; --i)
		{
			j = rand()%i;
			BYTE bTemp = bRestCardList[i];
			bRestCardList[i] = bRestCardList[j];
			bRestCardList[j] = bTemp;
		}
		memcpy(cbCardBuffer, cbTypeCard, 5);
		memcpy(cbCardBuffer + 5, bRestCardList, cbBufferCount - 5);		
		bStackCount++;
		if(GetCardType(cbTypeCard,5) >= OX_FIVEKING)
			break;
	}
}

void GameLogic::RandCardSpecialSmallType(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	BYTE cbTypeCard[5] = {0};
	BYTE cbCardArray[16];
	int bStackCount = 0;
	BYTE bRestCardList[36] = {0};

	int i = 0;
	int j = 0;
	int index = 0;
	for(i = 0; i < 52; i++)
	{
		if(GetCardValue(m_cbCardListData[i]) < 5)
		{
			cbCardArray[index++] = m_cbCardListData[i];
		}
	}

	while (true)
	{
		if(bStackCount > 50)
			break;
		for(i = 11; i > 0; --i)
		{
			j = rand()%i;
			BYTE bTemp = cbCardArray[i];
			cbCardArray[i] = cbCardArray[j];
			cbCardArray[j] = bTemp;
		}
		memcpy(cbTypeCard, cbCardArray, 5);
		if(GetCardType(cbTypeCard,5) == OX_FIVESMALL)
			break;
		bStackCount++;
	}
	index = 0;
	for(i = 0; i < 52; i++)
	{
		if(m_cbCardListData[i] != cbTypeCard[0] && m_cbCardListData[i] != cbTypeCard[1]
			&& m_cbCardListData[i] != cbTypeCard[2] && m_cbCardListData[i] != cbTypeCard[3]
			&& m_cbCardListData[i] != cbTypeCard[4])
		{
			bRestCardList[index++] = m_cbCardListData[i];
		}
	}

	bStackCount = 0;
	while (true)
	{
		if(bStackCount > 5)
			break;
		for(i = 35; i > 0; --i)
		{
			j = rand()%i;
			BYTE bTemp = bRestCardList[i];
			bRestCardList[i] = bRestCardList[j];
			bRestCardList[j] = bTemp;
		}
		memcpy(cbCardBuffer, cbTypeCard, 5);
		memcpy(cbCardBuffer + 5, bRestCardList, cbBufferCount - 5);		
		bStackCount++;
		if(GetCardType(cbTypeCard,5) >= OX_FIVESMALL)
			break;
	}
}
