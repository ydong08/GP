#ifndef _MAHJONG_TABLE_H_
#define _MAHJONG_TABLE_H_

#include "MahjongLogic.h"

class CMahjongPlayer;
class CMahjongTable
{
public:
	enum CapabilityType
	{
		support_chi			= 0x0001,	// 支持吃
		support_out_assist	= 0x0002,	// 支持出牌辅助
		support_listen		= 0x0004,	// 支持叫听
	};

	enum EstimatType
	{
		estimat_out_card,			//出牌效验
		estimat_gang_card,			//杠牌效验
	};

public:
	CMahjongTable(uint32_t id, CMahjongLogic *pMahjongLogic);
	virtual ~CMahjongTable();

	// 桌子编号
	uint16_t ID() { return m_id; }

	// 添加一个玩家
	bool AddPlayer(CMahjongPlayer *pPlayer);

	// 移除一个玩家
	void RemovePlayer(CMahjongPlayer *pPlayer);

	// 获取桌子上玩家的数量
	uint8_t GetPlayerCount() const { return m_nCurPlayerCount; }

	// 查找指定的玩家
	CMahjongPlayer* GetAt(uint8_t nPos);

	// 查找指定的玩家
	CMahjongPlayer* Find(uint32_t uid);
	
	// 桌子是否为空
	bool IsEmpty() const { return (0 == m_nCurPlayerCount); }

	// 桌子是否满
	bool IsFull() const { return (GAME_PLAYER == m_nCurPlayerCount); }

	// 桌子是否处于游戏状态
	bool IsPlaying() const { return m_bPlaying; }

	// 游戏开始
	void GameStart();

	// 庄家开始出牌
	void BankStartOutCard();

	// 游戏结束
	void GameOver();

	// 玩家出牌
	void OutCard(CMahjongPlayer *pPlayer, uint8_t nOutCard, bool bListen);

	// 玩家动作判断
	bool EstimateUserResponse(uint8_t nCurPos, uint8_t nCurCard, EstimatType eEstimatType);

	// 给指定的玩家发牌
	bool DispatchCard(uint8_t uCurUser, bool bTail);

	// 主动动作(由用户抓牌导致的动作)
	bool OperateCardActively(CMahjongPlayer *pPlayer, uint8_t actionCard, uint8_t action);

	// 被动动作(由某个玩家出牌导致的动作)
	bool OperateCardPassively(CMahjongPlayer *pPlayer, uint8_t actionCard, uint8_t action);

	// 清空桌子和游戏有关的数据
	void ResetData();

	// 清空桌子的所有数据
	void Clear();

public:
	// 发牌完成
	virtual void OnDealCardCompleted() = 0;
	
	// 庄家开始出牌
	virtual void OnBankStartOutCard() = 0;

	// 玩家出牌完成
	virtual void OnOutCardCompleted() = 0;

	// 玩家摸牌完成
	virtual void OnDispatchCardCompleted(bool bTail) = 0;

	// 吃、碰牌的动作完成
	virtual void OnOperateCardCompleted(uint8_t nPlayerPos, uint8_t nTargetUser, uint8_t nTargetAction, uint8_t nTargetCard, uint8_t nNewAction, bool bActively) = 0;

	// 玩家吃胡
	virtual void OnProcessChihu(uint8_t nPlayerPos) = 0;

	// 游戏结束
	virtual void OnGameOver() = 0;

	// 获取当前牌的胡牌番数
	virtual uint16_t GetCommonPoints(const uint8_t* pCardIndex, uint8_t cbCurrentCard, CChiHuRight &chr) = 0;

private:
	void DealCard();
	void AnalyseBeforeOutCard(CMahjongPlayer* pPlayer);
	void OutCardAssist(CMahjongPlayer* pPlayer);
	uint8_t GetLeftCount(CMahjongPlayer* p, uint8_t card);

protected:
	uint16_t				m_id;								// 桌子id

	CMahjongPlayer*			m_players[GAME_PLAYER];				// 当前桌子上的玩家 
	uint8_t					m_nCurPlayerCount;					// 当前桌子上玩家的数量

	uint32_t				m_nDicePoint;						// 筛子

	uint8_t					m_nBankerPos;						// 庄家的位置
	uint8_t					m_nReplacePos;
	uint8_t					m_nOutCardPos;
	uint8_t					m_nOutCardData;
	uint8_t					m_nOutCardCount;

	uint8_t					m_nSendCardData;
	uint8_t					m_nSendCardCount;

	uint8_t					m_nLeftCardCount;
	uint8_t					m_nEndLeftCount;
	uint8_t					m_allCardData[FULL_COUNT];

	uint16_t				m_nHeapHead;						// 牌堆头
	uint16_t				m_nHeapTail;						// 牌堆尾
	uint8_t					m_heapinfo[GAME_PLAYER][2];			// 牌堆信息

	uint8_t					m_nResumePos;						// 还原用户方位
	uint8_t					m_nCurrentPos;						// 当前用户方位
	uint8_t					m_nProvidePos;						// 供应用户方位
	uint8_t					m_nProvideData;						// 供应牌
	uint8_t					m_nChihuData;

	bool					m_bQiangGang;						// 是否抢杠状态
	bool					m_bGangPao;							// 是否为杠上炮状态
	bool					m_bWaitForOperate;

	bool					m_bPlaying;
	CMahjongLogic*			m_pGameLogic;
	uint16_t				m_capabilitis;
};

#endif

