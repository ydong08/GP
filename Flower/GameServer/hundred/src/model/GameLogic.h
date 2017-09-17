#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#include "Typedef.h"
#ifdef WIN32
#include "Windows.h"
#endif

//////////////////////////////////////////////////////////////////////////
//宏定义

#define MAX_COUNT					3									//最大数目
#define	DRAW						2									//和局类型

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//扑克类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对子类型
#define	CT_SHUN_ZI					3									//顺子类型
#define CT_JIN_HUA					4									//金花类型
#define	CT_SHUN_JIN					5									//顺金类型
#define	CT_BAO_ZI					6									//豹子类型
#define CT_SPECIAL					7									//特殊类型

//////////////////////////////////////////////////////////////////////////
#define CountArray(array) ((sizeof(array))/(sizeof(array[0])))
#define ZeroMemory(src,len) (memset((src), 0, (len)))
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y)) 

struct Card
{
	BYTE value;
	bool used;
	int index;

	Card()
	{
		used = false;
		index = -1;
	}
};

//游戏逻辑类
class GameLogic
{
	//变量定义
private:
	static BYTE						m_cbCardListData[52];				//扑克定义

																		//函数定义
public:
	//构造函数
	GameLogic();
	//析构函数
	virtual ~GameLogic();

	static Card						CardList[52];
	static void initCardList();
	//类型函数
public:
	//获取类型
	BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount);
	//获取数值
	static BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//获取花色
	static BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//控制函数
public:
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);

	void RandSpecialCardList(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE ct);
	void RandSpecialCard(BYTE cbCardBuffer[], BYTE cbBufferCount, short jackcardtype);
	//对子类型
	//void RandCardSpecialDoubleType(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//特殊类型、地龙
	void RandCardSpecialDiLongType(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//顺子类型
	void RandCardSpecialShunZiType(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//金花类型
	void RandCardSpecialJinHuaType(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//顺金类型
	void RandCardSpecialShunJinType(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//豹子类型
	void RandCardSpecialBaoZiType(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE CardValue = 0);

	//获取倍数
	BYTE GetTimes(BYTE cbCardData[], BYTE cbCardCount);

	//功能函数
public:
	//逻辑数值
	static BYTE GetCardLogicValue(BYTE cbCardData);
	//对比扑克
	BYTE CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount);

	void CoppyCardList();
	//
	void SetAvailCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//
	BYTE ChangeCard(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE cbCardValue);
public:
	//可用的牌（用于换牌）
	BYTE m_bAvailCardList[52];
};

//////////////////////////////////////////////////////////////////////////

#endif
