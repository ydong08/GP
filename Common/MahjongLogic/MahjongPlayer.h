#ifndef _MAHJONG_PLAYER_H_
#define _MAHJONG_PLAYER_H_

#include "MahjongDef.h"
#include "MahjongLogic.h"

class CMahjongTable;
class CMahjongPlayer
{
public:
	CMahjongPlayer();
	virtual ~CMahjongPlayer();

	bool EnterTable(CMahjongTable *pTable);

	void Clear();
	void ResetGameData();

public:
	uint32_t					m_id;							// 玩家id

	CMahjongTable*				m_pTable;						// 玩家所在的桌子
	uint8_t						m_nSeatPos;						// 座位id
	
	uint8_t						m_handCardIndex[MAX_INDEX];		// 玩家手牌
	bool						m_trustee;						// 是否托管
	bool						m_listen;						// 是否听牌

	uint8_t						m_discardCard[FULL_COUNT];		// 玩家的弃牌记录
	uint8_t						m_discardCount;					// 玩家的弃牌数量
	
	bool						m_forbidChihu;					// 禁止吃胡
	bool						m_forbidPeng;					// 禁止吃碰
								
	//玩家状态
	bool						m_response;						// 响应标志
	uint8_t						m_action;						// 玩家动作
	uint8_t						m_operateCard;					// 操作麻将牌
	uint8_t						m_performAction;				// 执行动作
																
	tagWeaveItem				m_weaveItems[MAX_WEAVE];		// 组合扑克
	uint8_t						m_weaveCount;					// 组合数
	
																
	uint8_t						m_chihuKind;					// 吃胡结果
	CChiHuRight					m_chihuRight;					// 吃胡权位

public:
	std::vector<uint8_t>		m_outCanTing;					// 可以听胡的出牌方案
	std::vector<tagListenItem>	m_outCanTingCard[MAX_INDEX];	// 可能胡牌的详细信息
	std::vector<tagListenItem>	m_tingCardInfo;					// 当前的听牌方案
};

#endif
