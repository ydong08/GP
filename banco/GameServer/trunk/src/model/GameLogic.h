#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE


#include <vector>
#include "Typedef.h"

//////////////////////////////////////////////////////////////////////////

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////
#define CopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define CountArray(array) ((sizeof(array))/(sizeof(array[0])))

//游戏逻辑
class CGameLogic
{
	//变量定义
private:
	static const BYTE				m_cbCardListData[52*8];				//扑克定义

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//控制函数
public:
	//混乱扑克
	//void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount,LONGLONG dwUserID);
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//洗牌
	BYTE* Shuffle(size_t& count, int randSeed);

	//逻辑函数
public:
    std::string GetCardValueName(BYTE card);
    std::string GetCardColorName(BYTE card);
    std::string getCardInfo(BYTE card);

	//获取牌点
	BYTE GetCardPip(BYTE cbCardData);
	//获取牌点
	BYTE GetCardListPip(const BYTE cbCardData[], BYTE cbCardCount);

    //获取扑克总数
    static int GetCardCount() { return 52 * 8; }
};

//////////////////////////////////////////////////////////////////////////

#endif
