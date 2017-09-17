#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Logger.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const BYTE	GameLogic::m_bCardArray[28]=									//扑克数组
{
	0x01,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K (14,8,9,10,11,12,13)
	0x11,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K (14,8,9,10,11,12,13)
	0x21,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K (14,8,9,10,11,12,13)
	0x31,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K (14,8,9,10,11,12,13)
};

//////////////////////////////////////////////////////////////////////////

//获取扑克花色
BYTE GameLogic::GetHuaKind(BYTE bCard)
{
	return bCard>>4;
}

//获取扑克数值
BYTE GameLogic::GetCardValoe(BYTE bCard)
{
	BYTE bVolea=bCard & 0x0F;
	if (bVolea==1) bVolea=14;
	return bVolea;
}

//获取扑克类型
BYTE GameLogic::GetCardKind(BYTE bCardList[], BYTE bCardCount)
{
	if (IsTongHuaShun(bCardList,bCardCount)) return TONG_HUA_SHUN;
	if (IsTieZhi(bCardList,bCardCount)) return TIE_ZHI;
	if (IsHuLu(bCardList,bCardCount)) return HU_LU;
	if (IsTongHua(bCardList,bCardCount)) return TONG_HUA;
	if (IsShunZhi(bCardList,bCardCount)) return SHUN_ZI;
	if (IsSanTiao(bCardList,bCardCount)) return SAN_TIAO;
	if (IsLiangDui(bCardList,bCardCount)) return LAING_DUI;
	if (IsDuiZhi(bCardList,bCardCount)) return DUI_ZI;
	return SHAN;
}

//查找对牌
BYTE GameLogic::FindDoubleCard(BYTE bCardList[], BYTE bCardCount, BYTE * bOutDoubleCard)
{
	BYTE bDoubleCount=0;
	if (bCardCount<2) return 0;
	for (BYTE i=0;i<bCardCount-1;i++)
	{
		if (GetCardValoe(bCardList[i])==GetCardValoe(bCardList[i+1]))
		{
			*(bOutDoubleCard+bDoubleCount*2)=bCardList[i];
			*(bOutDoubleCard+bDoubleCount*2+1)=bCardList[i+1];
			bDoubleCount++;
			i++;
		}
	}
	return bDoubleCount;
}

//是否同花顺
bool GameLogic::IsTongHuaShun(BYTE bCardList[], BYTE bCardCount)
{
	if (bCardCount!=5) return false;
	
	BYTE bBaseHua=GetHuaKind(bCardList[0]);
	BYTE bFirstCardVoloe=GetCardValoe(bCardList[0]);
	for (BYTE i=1;i<bCardCount;i++)
	{
		if (GetHuaKind(bCardList[i])!=bBaseHua) return false;
		if (bFirstCardVoloe!=(GetCardValoe(bCardList[i])+i)) return false;
	}
	return true;
}

//是否铁支
bool GameLogic::IsTieZhi(BYTE bCardList[], BYTE bCardCount)
{
	if (bCardCount<4) return false;
	
	BYTE bBaseVoloe=GetCardValoe(bCardList[1]),bNoFixCount=0;
	for (BYTE i=0;i<bCardCount;i++)
	{
		if (GetCardValoe(bCardList[i])!=bBaseVoloe) bNoFixCount++;
		if ((bCardCount-bNoFixCount)<4) return false;
	}
	return true;
}

//是否葫芦
bool GameLogic::IsHuLu(BYTE bCardList[], BYTE bCardCount)
{
	if (bCardCount!=5) return false;

	BYTE bFirstVoloe=GetCardValoe(bCardList[0]);
	BYTE bMidVoloe=GetCardValoe(bCardList[2]);
	BYTE bLastVoloe=GetCardValoe(bCardList[4]);
	if (GetCardValoe(bCardList[1])!=bFirstVoloe) return false;
	if (GetCardValoe(bCardList[3])!=bLastVoloe) return false;
	if ((bMidVoloe!=bFirstVoloe)&&(bMidVoloe!=bLastVoloe)) return false;
	
	return true;
}

//是否同花
bool GameLogic::IsTongHua(BYTE bCardList[], BYTE bCardCount)
{
	if (bCardCount!=5) return false;
	
	BYTE bBaseHua=GetHuaKind(bCardList[0]);
	for (BYTE i=1;i<bCardCount;i++)
	{
		if (GetHuaKind(bCardList[i])!=bBaseHua) return false;
	}
	return true;
}

//是否顺子
bool GameLogic::IsShunZhi(BYTE bCardList[], BYTE bCardCount)
{
	if (bCardCount!=5) return false;
	
	BYTE bBaseHua=GetHuaKind(bCardList[0]);
	BYTE bFirstCardVoloe=GetCardValoe(bCardList[0]);
	for (BYTE i=1;i<bCardCount;i++)
	{
		if (bFirstCardVoloe!=(GetCardValoe(bCardList[i])+i)) return false;
	}
	return true;
}

//是否三条
bool GameLogic::IsSanTiao(BYTE bCardList[], BYTE bCardCount)
{
	if (bCardCount<3) return false;
	
	BYTE bBaseVoloe=GetCardValoe(bCardList[2]),bBaseCount=0;
	for (BYTE i=0;i<bCardCount;i++)
	{
		if (GetCardValoe(bCardList[i])==bBaseVoloe) bBaseCount++;
	}
	return (bBaseCount==3);
}

//是否两对
bool GameLogic::IsLiangDui(BYTE bCardList[], BYTE bCardCount)
{
	if (bCardCount<4) return FALSE;
	
	BYTE bDoubleCount=0;
	for (BYTE i=0;i<bCardCount-1;i++)
	{
		if (GetCardValoe(bCardList[i])==GetCardValoe(bCardList[i+1]))
		{
			i++;
			bDoubleCount++;
			if (bDoubleCount==2) return true;
		}
	}
	return false;
}

//是否对子
bool GameLogic::IsDuiZhi(BYTE bCardList[], BYTE bCardCount)
{
	if (bCardCount<2) return false;
	
	BYTE bDoubleCount=0;
	for (BYTE i=0;i<bCardCount-1;i++)
	{
		if (GetCardValoe(bCardList[i])==GetCardValoe(bCardList[i+1])) return true;
	}
	return false;
}

//对比单只扑克
bool GameLogic::CompareOnlyOne(BYTE bFirstCard, BYTE bLastCard)
{
	BYTE bFirstVolae=GetCardValoe(bFirstCard);
	BYTE bLastVolae=GetCardValoe(bLastCard);
	if (bFirstVolae==bLastVolae) return bFirstCard>bLastCard;
	return bFirstVolae>bLastVolae;
}

//对比同花顺
bool GameLogic::CompareTongHuaShun(BYTE bFirstList[], BYTE bLastList[])
{
	return CompareOnlyOne(bFirstList[0],bLastList[0]);
}

//对比铁支
bool GameLogic::CompareTeiZhi(BYTE bFirstList[], BYTE bLastList[])
{
	return CompareOnlyOne(bFirstList[2],bLastList[2]);
}

//对比葫芦
bool GameLogic::CompareHuLu(BYTE bFirstList[], BYTE bLastList[])
{
	return CompareOnlyOne(bFirstList[2],bLastList[2]);
}

//对比同花
bool GameLogic::CompareTongHua(BYTE bFirstList[], BYTE bLastList[])
{
	BYTE bFirst=0,bLast=0;
	for (BYTE i=0;i<5;i++)
	{
		bFirst=GetCardValoe(bFirstList[i]);
		bLast=GetCardValoe(bLastList[i]);
		if (bFirst!=bLast) return (bFirst>bLast);
	}
	return GetHuaKind(bFirstList[0])>GetHuaKind(bLastList[0]);
}

//对比顺子
bool GameLogic::CompareShunZhi(BYTE bFirstList[], BYTE bLastList[])
{
	return CompareOnlyOne(bFirstList[0],bLastList[0]);
}

//对比三条
bool GameLogic::CompareSanTiao(BYTE bFirstList[], BYTE bLastList[])
{
	return CompareOnlyOne(bFirstList[2],bLastList[2]);
}

//对比两对
bool GameLogic::CompareLiangDui(BYTE bFirstList[], BYTE bLastList[], BYTE bCardCount)
{
	BYTE bDoubleCard[2][4];
	if (FindDoubleCard(bFirstList,bCardCount,bDoubleCard[0])!=2) return false;
	if (FindDoubleCard(bLastList,bCardCount,bDoubleCard[1])!=2) return true;
	BYTE bFirstVoloe=GetCardValoe(bDoubleCard[0][0]);
	BYTE bLastVoloe=GetCardValoe(bDoubleCard[1][0]);
	if (bFirstVoloe==bLastVoloe) 
	{
		bFirstVoloe=GetCardValoe(bDoubleCard[0][2]);
		bLastVoloe=GetCardValoe(bDoubleCard[1][2]);
		if (bFirstVoloe!=bLastVoloe) return bFirstVoloe>bLastVoloe;
		return (GetHuaKind(bDoubleCard[0][0])==HEI_TAO);
	}
	return bFirstVoloe>bLastVoloe;
}

//对比对子
bool GameLogic::CompareDuiZhi(BYTE bFirstList[], BYTE bLastList[], BYTE bCardCount)
{
	BYTE bDoubleCard[2][4];
	if (FindDoubleCard(bFirstList,bCardCount,bDoubleCard[0])!=1) return false;
	if (FindDoubleCard(bLastList,bCardCount,bDoubleCard[1])!=1) return true;
	BYTE bFirstVoloe=GetCardValoe(bDoubleCard[0][0]);
	BYTE bLastVoloe=GetCardValoe(bDoubleCard[1][0]);
	if (bFirstVoloe==bLastVoloe) return (GetHuaKind(bDoubleCard[0][0])==HEI_TAO);
	return bFirstVoloe>bLastVoloe;
}

//对比散牌
bool GameLogic::CompareSanCard(BYTE bFirstList[], BYTE bLastList[], BYTE bCardCount)
{
	return CompareOnlyOne(bFirstList[0],bLastList[0]);
}

//混乱扑克
void GameLogic::RandCard(BYTE bCardBuffer[], BYTE bBufferCount)
{
	BYTE bSend=0,bStation=0,bCardList[CountArray(m_bCardArray)];
	memcpy(bCardList,m_bCardArray,sizeof(m_bCardArray));
	//static long int dwRandCount=0L;
	//srand((unsigned)time(NULL)+dwRandCount++);
	do
	{
		bStation=rand()%(CountArray(m_bCardArray)-bSend);
		bCardBuffer[bSend++]=bCardList[bStation];
		bCardList[bStation]=bCardList[CountArray(m_bCardArray)-bSend];
	} while (bSend<bBufferCount);

	return;
}

//排列扑克
void GameLogic::SortCard(BYTE bCardList[], BYTE bCardCount)
{
	bool bSorted=true;
	BYTE bTemp,bLast=bCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++) 
		{
			if (CompareOnlyOne(bCardList[i],bCardList[i+1])==FALSE)
			{	
				bTemp=bCardList[i];
				bCardList[i]=bCardList[i+1];
				bCardList[i+1]=bTemp;
				bSorted=false;
			}	
		}
		bLast--;
	} while (bSorted==false);

	return;
}

//对比扑克，前者大于后者 TRUE，否则 FALSE
bool GameLogic::CompareCard(BYTE bFirstList[], BYTE bLastList[], BYTE bCardCount)
{
	//排列扑克
	SortCard(bFirstList,bCardCount);
	SortCard(bLastList,bCardCount);
	
	//获取类型
	BYTE bFirstKind=GetCardKind(bFirstList,bCardCount);
	BYTE bLastKind=GetCardKind(bLastList,bCardCount);
	
	//类型不相同，对比类型
	if (bFirstKind!=bLastKind) return bFirstKind>bLastKind;
	
	//类型相同
	switch(bFirstKind)
	{
		//散牌
	case SHAN: return CompareSanCard(bFirstList,bLastList,bCardCount);
		//对子
	case DUI_ZI: return CompareDuiZhi(bFirstList,bLastList,bCardCount);
		//两对
	case LAING_DUI: return CompareLiangDui(bFirstList,bLastList,bCardCount);
		//三条
	case SAN_TIAO: return CompareSanTiao(bFirstList,bLastList);
		//顺子
	case SHUN_ZI: return CompareShunZhi(bFirstList,bLastList);
		//同花
	case TONG_HUA: return CompareTongHua(bFirstList,bLastList);
		//葫芦
	case HU_LU: return CompareHuLu(bFirstList,bLastList);
		//铁支
	case TIE_ZHI: return CompareTeiZhi(bFirstList,bLastList);
		//同花顺
	case TONG_HUA_SHUN: return CompareTongHuaShun(bFirstList,bLastList);
	}
	
	return false;
}


//======================================Robot Need=========================================================
//对比扑克
bool GameLogic::CompareCard(BYTE bFirstList[], BYTE bfCardCount, BYTE bLastList[], BYTE blCardCount)
{
	//排列扑克
	SortCard(bFirstList,bfCardCount);
	SortCard(bLastList,blCardCount);
	char cardbuff[128]={0};
	for(int i = 0; i < bfCardCount; i++)
		sprintf(cardbuff+5*i, "0x%02x ",bFirstList[i]);
	//_LOG_DEBUG_("bFirstList:%s\n",cardbuff);
	for(int i = 0; i < blCardCount; i++)
		sprintf(cardbuff+5*i, "0x%02x ",bLastList[i]);
//	_LOG_DEBUG_("bLastList:%s\n",cardbuff);
	
	//获取类型
	BYTE bFirstKind=GetCardKind(bFirstList,bfCardCount);
	BYTE bLastKind=GetCardKind(bLastList,blCardCount);
//	_LOG_DEBUG_("bFirstKind:%d bLastKind:%d\n", bFirstKind, bLastKind);
	
	//类型不相同，对比类型
	if (bFirstKind!=bLastKind) return bFirstKind>bLastKind;
	
	//类型相同
	switch(bFirstKind)
	{
		//散牌
	case SHAN: return CompareSanCard(bFirstList,bLastList,blCardCount);
		//对子
	case DUI_ZI: return CompareDuiZhi(bFirstList,bfCardCount,bLastList,blCardCount);
		//两对
	case LAING_DUI: return CompareLiangDui(bFirstList,bfCardCount,bLastList,blCardCount);
		//三条
	case SAN_TIAO: return CompareSanTiao(bFirstList,bLastList);
		//顺子
	case SHUN_ZI: return CompareShunZhi(bFirstList,bLastList);
		//同花
	case TONG_HUA: return CompareTongHua(bFirstList,bLastList);
		//葫芦
	case HU_LU: return CompareHuLu(bFirstList,bLastList);
		//铁支
	case TIE_ZHI: return CompareTeiZhi(bFirstList,bLastList);
		//同花顺
	case TONG_HUA_SHUN: return CompareTongHuaShun(bFirstList,bLastList);
	}
	
	return false;
}

//对比两对
bool GameLogic::CompareLiangDui(BYTE bFirstList[], BYTE bfCardCount, BYTE bLastList[], BYTE blCardCount)
{
	BYTE bDoubleCard[2][4];
	if (FindDoubleCard(bFirstList,bfCardCount,bDoubleCard[0])!=2) return false;
	if (FindDoubleCard(bLastList,blCardCount,bDoubleCard[1])!=2) return true;
	BYTE bFirstVoloe=GetCardValoe(bDoubleCard[0][0]);
	BYTE bLastVoloe=GetCardValoe(bDoubleCard[1][0]);
	if (bFirstVoloe==bLastVoloe) 
	{
		bFirstVoloe=GetCardValoe(bDoubleCard[0][2]);
		bLastVoloe=GetCardValoe(bDoubleCard[1][2]);
		if (bFirstVoloe!=bLastVoloe) return bFirstVoloe>bLastVoloe;
		return (GetHuaKind(bDoubleCard[0][0])==HEI_TAO);
	}
	return bFirstVoloe>bLastVoloe;
}

//对比对子
bool GameLogic::CompareDuiZhi(BYTE bFirstList[], BYTE bfCardCount, BYTE bLastList[], BYTE blCardCount)
{
	BYTE bDoubleCard[2][4];
	if (FindDoubleCard(bFirstList,bfCardCount,bDoubleCard[0])!=1) return false;
	if (FindDoubleCard(bLastList,blCardCount,bDoubleCard[1])!=1) return true;
	BYTE bFirstVoloe=GetCardValoe(bDoubleCard[0][0]);
	BYTE bLastVoloe=GetCardValoe(bDoubleCard[1][0]);
	if (bFirstVoloe==bLastVoloe) return (GetHuaKind(bDoubleCard[0][0])==HEI_TAO);
	return bFirstVoloe>bLastVoloe;
}
