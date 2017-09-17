#ifndef _MAHJONG_LOGIC_H_ 
#define _MAHJONG_LOGIC_H_

#include "MahjongDef.h"
#include "Array.h"

//类型子项
struct tagKindItem
{
	uint8_t							cbWeaveKind;						// 组合类型
	uint8_t							cbCenterCard;						// 中心扑克
	uint8_t							cbCardIndex[3];						// 扑克索引
	uint8_t							cbValidIndex[3];					// 实际扑克索引
};

//组合子项
struct tagWeaveItem
{
	uint8_t							cbWeaveKind;						// 组合类型
	uint8_t							cbCenterCard;						// 中心扑克
	uint8_t							cbPublicCard;						// 公开标志
	uint16_t						wProvideUser;						// 供应用户
};

//杠牌结果
struct tagGangCardResult
{
	uint8_t							cbCardCount;						// 扑克数目
	uint8_t							cbCardData[MAX_WEAVE];				// 扑克数据
};

//分析子项
struct tagAnalyseItem
{
	uint8_t							cbCardEye;							// 牌眼扑克
	bool                            bMagicEye;                          // 牌眼是否是王霸
	uint8_t							cbWeaveKind[MAX_WEAVE];				// 组合类型
	uint8_t							cbCenterCard[MAX_WEAVE];			// 中心扑克
};

//听牌参数
struct tagListenItem
{
	uint8_t							cbListenCard;						// 待听的牌
	uint8_t							cbCardLeft;							// 待听的牌还剩的数量（仅依据桌面计算）
	uint8_t							cbPoints;							// 听牌的番数（此处番数只是牌型的番数，可能小于最终结算的番数）
};

//////////////////////////////////////////////////////////////////////////

#define MASK_CHI_HU_RIGHT			0x0fffffff

/*
//	权位类。
//  注意，在操作仅位时最好只操作单个权位.例如
//  CChiHuRight chr;
//  chr |= (chr_zi_mo|chr_peng_peng)，这样结果是无定义的。
//  只能单个操作:
//  chr |= chr_zi_mo;
//  chr |= chr_peng_peng;
*/
class CChiHuRight
{
	//静态变量
private:
	static bool						m_bInit;
	static uint32_t					m_dwRightMask[MAX_RIGHT_COUNT];

	//权位变量
private:
	uint32_t						m_dwRight[MAX_RIGHT_COUNT];

public:
	//构造函数 
	CChiHuRight();
	CChiHuRight(const CChiHuRight &other);

	//运算符重载
public:
	//赋值符
	CChiHuRight & operator = (const CChiHuRight &other);
	CChiHuRight & operator = (uint32_t dwRight);

	//与等于
	CChiHuRight & operator &= (uint32_t dwRight);
	//或等于
	CChiHuRight & operator |= (uint32_t dwRight);

	//与
	CChiHuRight operator & (uint32_t dwRight);
	CChiHuRight operator & (uint32_t dwRight) const;

	//或
	CChiHuRight operator | (uint32_t dwRight);
	CChiHuRight operator | (uint32_t dwRight) const;

public:
	//是否权位为空
	bool IsEmpty();

	//设置权位为空
	void SetEmpty();

	//获取权位数值
	uint8_t GetRightData(uint32_t dwRight[], uint8_t cbMaxCount);

	//设置权位数值
	bool SetRightData(const uint32_t dwRight[], uint8_t cbRightCount);

private:
	//检查权位是否正确
	bool IsValidRight(uint32_t dwRight);
};


//////////////////////////////////////////////////////////////////////////
//数组说明
typedef CWHArray<tagAnalyseItem, tagAnalyseItem &> CAnalyseItemArray;

//麻将通用逻辑类
class CMahjongPlayer;
class CMahjongLogic
{
public:
	//构造函数
	CMahjongLogic();
	
	//析构函数
	virtual ~CMahjongLogic();


public:
	//混乱扑克
	void RandCardData(uint8_t *pCardData, uint8_t nMaxCount);

	//删除扑克
	bool RemoveCard(uint8_t *pCardIndex, uint8_t nRemoveCard);

	//删除扑克
	bool RemoveCard(uint8_t *pCardIndex, const uint8_t* pRemoveCard, uint8_t nRemoveCount);

	//删除扑克
	bool RemoveCard(uint8_t *pCardIndex, uint8_t nCardCount, const uint8_t* pRemoveCard, uint8_t cbRemoveCount);

	//设置钻牌
	void SetMagicIndex(uint8_t nMagicIndex) { m_nMagicIndex = nMagicIndex; }

	//钻牌
	bool IsMagicCard(uint8_t nCardData);

	//转换函数
public:
	//扑克转换(基础类提供的是一个通用的算法，速度不是最快的，派生类可以提供速度更快的方法)
	virtual uint8_t SwitchToCardData(uint8_t nCardIndex);
	
	//扑克转换(基础类提供的是一个通用的算法，速度不是最快的，派生类可以提供速度更快的方法)
	virtual uint8_t SwitchToCardIndex(uint8_t nCardData);
	
	//扑克转换
	uint8_t SwitchToCardData(const uint8_t* pCardIndex, uint8_t* pCardData);
	
	//扑克转换
	uint8_t SwitchToCardIndex(const uint8_t* pCardData, uint8_t nCardCount, uint8_t* pCardIndex);

public:
	//有效判断
	static bool IsValidCard(uint8_t cbCardData);
	
	//扑克数目
	static uint8_t GetCardCount(const uint8_t* pCardIndex);
	
	//组合扑克
	uint8_t GetWeaveCard(uint8_t nWeaveKind, uint8_t nCenterCard, uint8_t cardBuffer[4]);

	//等级函数
public:
	//动作等级
	uint8_t GetUserActionRank(uint8_t nUserAction);

	//动作判断
public:
	//吃牌判断
	uint8_t EstimateEatCard(const uint8_t* pCardIndex, uint8_t nCurrentCard);

	//碰牌判断
	uint8_t EstimatePengCard(const uint8_t* pCardIndex, uint8_t nCurrentCard);

	//杠牌判断
	uint8_t EstimateGangCard(const uint8_t* pCardIndex, uint8_t nCurrentCard);

	//杠牌分析
	uint8_t AnalyseGangCard(CMahjongPlayer* pPlayer, tagGangCardResult &gangCardResult);
	
	//吃胡分析
	uint8_t AnalyseChiHuCard(CMahjongPlayer* pPlayer, uint8_t nCurrentCard, CChiHuRight &chiHuRight, bool bDetails);
	
	//听牌分析
	uint8_t AnalyseTingCard(CMahjongPlayer* pPlayer);
	
	//是否听牌
	bool IsTingCard(const uint8_t* pCardIndex, const tagWeaveItem weaveItem[], uint8_t nWeaveCount);

	//是否有杠牌
	bool IsGangCard(const uint8_t* pCardIndex);

	//分析番数
public:
	// 是不是一个合法的可以胡的牌
	virtual bool IsValidCardType(const uint8_t* pCardIndex, const tagWeaveItem weaveItem[], const uint8_t nItemCount, const tagAnalyseItem &analyseItem) = 0;

	// 分析特殊胡牌类型（七对、十三幺)
	virtual bool AnalyseSpecialCard(const uint8_t* pCardIndex, const tagWeaveItem weaveItem[], uint8_t nItemCount, CChiHuRight &chiHuRight) = 0;

	// 分析详细牌型
	virtual CChiHuRight AnalyseDetailType(const uint8_t* pCardIndex, const tagWeaveItem weaveItem[], const uint8_t nItemCount, const tagAnalyseItem &analyseItem) = 0;
	
	// 比较牌型的权重
	virtual uint8_t CompareChiHuRight(const CChiHuRight &right1, const CChiHuRight &right2) = 0;

	// 常见牌型分析
public:
	// 碰碰胡
	bool IsPengPeng(const tagAnalyseItem *pAnalyseItem);

	// 清一色
	bool IsQingYiSe(const uint8_t *pCardIndex, const tagWeaveItem weaveItem[], const uint8_t cbItemCount, const uint8_t cbCurrentCard);

	// 七小对
	bool IsQiXiaoDui(const uint8_t *pCardIndex, const tagWeaveItem weaveItem[], const uint8_t nWeaveCount, const uint8_t nCurrentCard);

protected:
	//分析扑克
	bool AnalyseCard(const uint8_t* pCardIndex, const tagWeaveItem weaveItem[], uint8_t nItemCount, CAnalyseItemArray &analyseItemArray);
	
	//排序,根据牌值排序
	bool SortCardList(uint8_t* pCardData, uint8_t nCardCount);

protected:
	bool IsChiHuCard(const uint8_t* pCardIndex, const tagWeaveItem weaveItem[], uint8_t nWeaveCount, CChiHuRight &chiHuRight, bool bDetails);

protected:
	uint8_t								m_honorIndex;						// 字牌的开始索引
	uint8_t								m_cardIndexMap[2][MAX_COLOR];		// 每一种颜色牌的开始序号[0]和开始的牌的值[1]
	uint8_t								m_nMagicIndex;						// 钻牌索引
};

#endif

