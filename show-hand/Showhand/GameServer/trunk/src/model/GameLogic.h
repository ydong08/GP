#ifndef GameLogic_H
#define GameLogic_H
#include "wtypedef.h"

//////////////////////////////////////////////////////////////////////////

//梭哈扑克类型
#define SHAN								0
#define DUI_ZI								1
#define LAING_DUI							2
#define SAN_TIAO							3
#define SHUN_ZI								4
#define	TONG_HUA							5
#define HU_LU								6
#define TIE_ZHI								7
#define TONG_HUA_SHUN						8

//扑克花色类型
#define FANG_KUAI							0
#define MEI_HUA								1
#define HONG_TAO							2
#define HEI_TAO								3

#define CountArray(array) ((sizeof(array))/(sizeof(array[0])))

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class GameLogic
{
	//变量定义
private:
	static const BYTE				m_bCardArray[28];					//扑克定义

	//基础函数
public:
	//获取扑克花色
	BYTE GetHuaKind(BYTE bCard);
	//获取扑克数值
	BYTE GetCardValoe(BYTE bCard);
	//查找对牌
	BYTE FindDoubleCard(BYTE bCardList[], BYTE bCardCount, BYTE * bOutDoubleCard);

	//类型函数
public:
	//是否同花顺
	bool IsTongHuaShun(BYTE bCardList[], BYTE bCardCount);
	//是否铁支
	bool IsTieZhi(BYTE bCardList[], BYTE bCardCount);
	//是否葫芦
	bool IsHuLu(BYTE bCardList[], BYTE bCardCount);
	//是否同花
	bool IsTongHua(BYTE bCardList[], BYTE bCardCount);
	//是否顺子
	bool IsShunZhi(BYTE bCardList[], BYTE bCardCount);
	//是否三条
	bool IsSanTiao(BYTE bCardList[], BYTE bCardCount);
	//是否两对
	bool IsLiangDui(BYTE bCardList[], BYTE bCardCount);
	//是否对子
	bool IsDuiZhi(BYTE bCardList[], BYTE bCardCount);

	//对比函数
public:
	//对比单只扑克
	bool CompareOnlyOne(BYTE bFirstCard, BYTE bLastCard);
	//对比同花顺
	bool CompareTongHuaShun(BYTE bFirstList[], BYTE bLastList[]);
	//对比铁支
	bool CompareTeiZhi(BYTE bFirstList[], BYTE bLastList[]);
	//对比葫芦
	bool CompareHuLu(BYTE bFirstList[], BYTE bLastList[]);
	//对比同花
	bool CompareTongHua(BYTE bFirstList[], BYTE bLastList[]);
	//对比顺子
	bool CompareShunZhi(BYTE bFirstList[], BYTE bLastList[]);
	//对比三条
	bool CompareSanTiao(BYTE bFirstList[], BYTE bLastList[]);
	//对比两对
	bool CompareLiangDui(BYTE bFirstList[], BYTE bLastList[], BYTE bCardCount);
	//对比对子
	bool CompareDuiZhi(BYTE bFirstList[], BYTE bLastList[], BYTE bCardCount);
	//对比散牌
	bool CompareSanCard(BYTE bFirstList[], BYTE bLastList[], BYTE bCardCount);

	//功能函数
public:
	//混乱扑克
	void RandCard(BYTE bCardBuffer[], BYTE bBufferCount);
	//排列扑克
	void SortCard(BYTE bCardList[], BYTE bCardCount);
	//对比扑克
	bool CompareCard(BYTE bFirstList[], BYTE bLastList[], BYTE bCardCount);
	//获取扑克类型
	BYTE GetCardKind(BYTE bCardList[], BYTE bCardCount);
};

#endif 
